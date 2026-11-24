#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "PedalLookAndFeel.h"

class GreenScreamAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit GreenScreamAudioProcessorEditor (GreenScreamAudioProcessor&);
    ~GreenScreamAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawScrew (juce::Graphics&, juce::Point<float> centre, float radius);
    bool isActive() const;
    void syncPresetBox();

    GreenScreamAudioProcessor& audioProcessor;
    PedalLookAndFeel lookAndFeel;

    juce::ComboBox presetBox;
    int lastPresetShown = -2;   // -1 = "Custom" (ningun preset coincide)

    juce::Slider driveSlider, toneSlider, levelSlider;
    juce::ToggleButton footswitch;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAttachment> driveAttach, toneAttach, levelAttach;
    std::unique_ptr<ButtonAttachment> bypassAttach;

    juce::Rectangle<int> driveLabel, toneLabel, levelLabel;
    juce::Rectangle<float> bodyRect, ledRect;
    bool lastActive = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GreenScreamAudioProcessorEditor)
};
