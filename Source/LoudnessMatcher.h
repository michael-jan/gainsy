/*
  ==============================================================================

    LoudnessMatcher.h
    Created: 9 Apr 2021 10:55:54am
    Author:  Michael Jan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Ebu128LoudnessMeter.h"

class LoudnessMatcher {
public:
    void prepareToPlay(double sampleRate,
                       int numberOfInputChannels,
                       int estimatedSamplesPerBlock,
                       int expectedRequestRate);

    void processBlock(juce::AudioBuffer<float>& buffer,
                      bool isBefore,
                      int chanIndex);

private:
    Ebu128LoudnessMeter loudnessMeter;
    float prevRatio;
};
