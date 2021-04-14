/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "MeterDisplay.h"
#include <JuceHeader.h>

//==============================================================================

using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class GainsyAudioProcessorEditor
    : public juce::AudioProcessorEditor
    , private juce::Timer
{
public:
    GainsyAudioProcessorEditor(GainsyAudioProcessor&);
    ~GainsyAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback () override;

private:
    int getTextWidth(juce::Label& label);
    void attachToComponent(juce::Label& label, juce::Component& component, int offset);
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainsyAudioProcessor& audioProcessor;

    juce::ToggleButton modeSwitch;
    std::unique_ptr<ButtonAttachment> modeAttachment;
    juce::Label modeLabel;

    juce::Slider channelNumbox;
    std::unique_ptr<SliderAttachment> channelAttachment;
    juce::Label channelLabel;
    
    // juce::ConcertinaPanel channelModePanel;

    MeterDisplay ratioMeter;
    juce::Label ratioLabel;

    MeterDisplay loudnessMeter;
    juce::Label loudnessLabel;
    
    int refreshRate = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainsyAudioProcessorEditor)
};
