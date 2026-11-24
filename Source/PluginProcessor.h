#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "TS808Engine.h"
#include "Presets.h"

class GreenScreamAudioProcessor : public juce::AudioProcessor
{
public:
    GreenScreamAudioProcessor();
    ~GreenScreamAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "GreenScream by Vidal"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Presets de fabrica (hilo de mensajes / GUI). No tocan el bypass.
    void applyPreset (int index, bool useGestures = false);
    int  findMatchingPreset() const;   // indice del preset cuyos valores coinciden con los knobs, o -1

    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

private:
    static constexpr int kMaxChannels = 2;
    static constexpr int kCoefUpdateInterval = 32;   // muestras sobremuestreadas

    gs::TS808Channel channels[kMaxChannels];
    int maxBlockSize = 512;
    int coefCounter = 0;

    juce::dsp::Oversampling<float> oversampler { (size_t) kMaxChannels, 2,
                                                 juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
                                                 true, true };

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> dryDelay { 1024 };
    juce::AudioBuffer<float> dryBuffer;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> driveSmooth, toneSmooth; // a frecuencia sobremuestreada
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> levelSmooth, activeSmooth; // a frecuencia base

    std::atomic<float>* driveParam  = nullptr;
    std::atomic<float>* toneParam   = nullptr;
    std::atomic<float>* levelParam  = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GreenScreamAudioProcessor)
};
