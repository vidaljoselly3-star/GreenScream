#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
    // Level: 0..10  ->  -24 dB .. +12 dB   (6.7 ~ 0 dB)
    inline float levelToGain (float level)
    {
        return juce::Decibels::decibelsToGain (-24.0f + 3.6f * level);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout GreenScreamAudioProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto knobText = [] (float v, int) { return String (v, 1); };

    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "drive", 1 }, "Drive",
                NormalisableRange<float> (0.0f, 10.0f, 0.01f), 5.0f,
                AudioParameterFloatAttributes().withStringFromValueFunction (knobText)));

    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "tone", 1 }, "Tone",
                NormalisableRange<float> (0.0f, 10.0f, 0.01f), 5.0f,
                AudioParameterFloatAttributes().withStringFromValueFunction (knobText)));

    layout.add (std::make_unique<AudioParameterFloat> (ParameterID { "level", 1 }, "Level",
                NormalisableRange<float> (0.0f, 10.0f, 0.01f), 6.7f,
                AudioParameterFloatAttributes().withStringFromValueFunction (knobText)));

    layout.add (std::make_unique<AudioParameterBool> (ParameterID { "bypass", 1 }, "Bypass", false));

    return layout;
}

GreenScreamAudioProcessor::GreenScreamAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    driveParam  = apvts.getRawParameterValue ("drive");
    toneParam   = apvts.getRawParameterValue ("tone");
    levelParam  = apvts.getRawParameterValue ("level");
    bypassParam = apvts.getRawParameterValue ("bypass");
}

bool GreenScreamAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (in != out)
        return false;

    return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo();
}

void GreenScreamAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const double osRate = sampleRate * (double) oversampler.getOversamplingFactor();

    maxBlockSize = juce::jmax (1, samplesPerBlock);
    coefCounter = 0;
    oversampler.initProcessing ((size_t) maxBlockSize);
    oversampler.reset();

    const int latency = juce::roundToInt (oversampler.getLatencyInSamples());
    setLatencySamples (latency);

    juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, (juce::uint32) kMaxChannels };
    dryDelay.prepare (spec);
    dryDelay.setMaximumDelayInSamples (1024);
    dryDelay.setDelay ((float) latency);
    dryDelay.reset();

    dryBuffer.setSize (kMaxChannels, samplesPerBlock, false, true, true);

    for (auto& c : channels)
        c.prepare (osRate);

    driveSmooth.reset (osRate, 0.03);
    toneSmooth.reset (osRate, 0.03);
    levelSmooth.reset (sampleRate, 0.03);
    activeSmooth.reset (sampleRate, 0.02);

    driveSmooth.setCurrentAndTargetValue (driveParam->load() / 10.0f);
    toneSmooth.setCurrentAndTargetValue (toneParam->load() / 10.0f);
    levelSmooth.setCurrentAndTargetValue (levelToGain (levelParam->load()));
    activeSmooth.setCurrentAndTargetValue (bypassParam->load() > 0.5f ? 0.0f : 1.0f);

    for (auto& c : channels)
    {
        c.setDrive (driveSmooth.getTargetValue());
        c.setTone (toneSmooth.getTargetValue());
    }
}

void GreenScreamAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numCh = juce::jmin (buffer.getNumChannels(), kMaxChannels);
    const int numSamples = buffer.getNumSamples();

    if (numCh == 0 || numSamples == 0)
        return;

    for (int ch = numCh; ch < buffer.getNumChannels(); ++ch)
        buffer.clear (ch, 0, numSamples);

    if (dryBuffer.getNumSamples() < numSamples)
        dryBuffer.setSize (kMaxChannels, numSamples, false, false, true);

    // --- Señal seca retardada (compensa la latencia del sobremuestreo) ---
    for (int ch = 0; ch < numCh; ++ch)
    {
        const float* in = buffer.getReadPointer (ch);
        float* dry = dryBuffer.getWritePointer (ch);

        for (int i = 0; i < numSamples; ++i)
        {
            dryDelay.pushSample (ch, in[i]);
            dry[i] = dryDelay.popSample (ch);
        }
    }

    // --- Parámetros ---
    driveSmooth.setTargetValue (driveParam->load() / 10.0f);
    toneSmooth.setTargetValue (toneParam->load() / 10.0f);
    levelSmooth.setTargetValue (levelToGain (levelParam->load()));
    activeSmooth.setTargetValue (bypassParam->load() > 0.5f ? 0.0f : 1.0f);

    // --- Procesado sobremuestreado x4 ---
    juce::dsp::AudioBlock<float> block (buffer);
    auto subBlock = block.getSubsetChannelBlock (0, (size_t) numCh);

    int start = 0;
    while (start < numSamples)
    {
        const int len = juce::jmin (maxBlockSize, numSamples - start);
        auto chunk = subBlock.getSubBlock ((size_t) start, (size_t) len);

        auto osBlock = oversampler.processSamplesUp (chunk);
        const int osSamples = (int) osBlock.getNumSamples();

        float* osData[kMaxChannels] = { nullptr, nullptr };
        for (int ch = 0; ch < numCh; ++ch)
            osData[ch] = osBlock.getChannelPointer ((size_t) ch);

        for (int i = 0; i < osSamples; ++i)
        {
            const float d = driveSmooth.getNextValue();
            const float t = toneSmooth.getNextValue();

            if (coefCounter == 0)
            {
                for (int ch = 0; ch < numCh; ++ch)
                {
                    channels[ch].setDrive (d);
                    channels[ch].setTone (t);
                }
            }
            if (++coefCounter >= kCoefUpdateInterval)
                coefCounter = 0;

            for (int ch = 0; ch < numCh; ++ch)
                osData[ch][i] = channels[ch].process (osData[ch][i]);
        }

        oversampler.processSamplesDown (chunk);
        start += len;
    }

    // --- Level + mezcla activo/bypass ---
    for (int i = 0; i < numSamples; ++i)
    {
        const float lvl = levelSmooth.getNextValue();
        const float act = activeSmooth.getNextValue();

        for (int ch = 0; ch < numCh; ++ch)
        {
            float* out = buffer.getWritePointer (ch);
            const float dry = dryBuffer.getReadPointer (ch)[i];
            const float wet = out[i] * lvl;
            out[i] = dry + (wet - dry) * act;
        }
    }
}

void GreenScreamAudioProcessor::applyPreset (int index, bool useGestures)
{
    if (index < 0 || index >= gs::kNumFactoryPresets)
        return;

    const auto& preset = gs::kFactoryPresets[(size_t) index];

    auto setParam = [this, useGestures] (const char* id, float value)
    {
        if (auto* param = apvts.getParameter (id))
        {
            if (useGestures) param->beginChangeGesture();
            param->setValueNotifyingHost (param->convertTo0to1 (value));
            if (useGestures) param->endChangeGesture();
        }
    };

    setParam ("drive", preset.drive);
    setParam ("tone",  preset.tone);
    setParam ("level", preset.level);
}

int GreenScreamAudioProcessor::findMatchingPreset() const
{
    constexpr float tol = 0.02f;   // los knobs tienen paso de 0.01
    const float d = driveParam->load();
    const float t = toneParam->load();
    const float l = levelParam->load();

    for (int i = 0; i < gs::kNumFactoryPresets; ++i)
    {
        const auto& p = gs::kFactoryPresets[(size_t) i];
        if (std::abs (d - p.drive) < tol && std::abs (t - p.tone) < tol && std::abs (l - p.level) < tol)
            return i;
    }
    return -1;
}

juce::AudioProcessorEditor* GreenScreamAudioProcessor::createEditor()
{
    return new GreenScreamAudioProcessorEditor (*this);
}

void GreenScreamAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void GreenScreamAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GreenScreamAudioProcessor();
}
