/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class GainsyAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    GainsyAudioProcessorEditor(GainsyAudioProcessor&);
    ~GainsyAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainsyAudioProcessor& audioProcessor;

    juce::ToggleButton modeSwitch;
    std::unique_ptr<ButtonAttachment> modeAttachment;

    juce::Slider channelNumbox;
    std::unique_ptr<SliderAttachment> channelAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainsyAudioProcessorEditor)
};
