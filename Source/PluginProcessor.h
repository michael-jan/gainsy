/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LoudnessMatcher.h"

//==============================================================================
/**
*/
class GainsyAudioProcessor : public juce::AudioProcessor {
public:
    //==============================================================================
    GainsyAudioProcessor();
    ~GainsyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getParams() { return params; }

    inline float getBeforeLoudness(void)
    { return loudnessMatcher.getBeforeLoudness(*channelParam); }

    inline float getAfterLoudness(void)
    { return loudnessMatcher.getAfterLoudness(); }

    inline float getRatio(void)
    { return loudnessMatcher.getRatio(); }

private:
    //==============================================================================

    juce::AudioParameterChoice* modeParam;
    juce::AudioParameterInt* channelParam;

    juce::AudioProcessorValueTreeState params;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    LoudnessMatcher loudnessMatcher;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainsyAudioProcessor)
};
