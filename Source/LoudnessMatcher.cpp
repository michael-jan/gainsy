/*
  ==============================================================================

    LoudnessMatcher.cpp
    Created: 9 Apr 2021 10:55:54am
    Author:  Michael Jan

  ==============================================================================
*/

#include "LoudnessMatcher.h"

// global array shared by all instances
std::atomic<float> loudness[128] = {0.0};

void LoudnessMatcher::prepareToPlay(double sampleRate,
                                    int numberOfInputChannels,
                                    int estimatedSamplesPerBlock,
                                    int expectedRequestRate)
{
    prevRatio = 0.0;
    loudnessMeter.prepareToPlay(sampleRate,
                                numberOfInputChannels,
                                estimatedSamplesPerBlock,
                                expectedRequestRate);
}

void LoudnessMatcher::processBlock(juce::AudioBuffer<float>& buffer,
                                   bool isBefore,
                                   int chanIndex)
{
    loudnessMeter.processBlock(buffer);

    float currentLoudness = juce::Decibels::decibelsToGain(loudnessMeter.getMomentaryLoudness());

    if (isBefore) {
        loudness[chanIndex] = currentLoudness;
    } else {
        if (currentLoudness > 0.00001) {
            float ratio = loudness[chanIndex] / currentLoudness;
            buffer.applyGainRamp(0, buffer.getNumSamples(), prevRatio, ratio);
            prevRatio = ratio;
        }
    }
}
