#include "MeterDisplay.h"

static inline float clamp(float v, float min, float max)
{
    return v < min ? min : v > max ? max : v;
}

void MeterDisplay::paint(juce::Graphics& g)
{
    juce::Rectangle<int> bounds = getLocalBounds();

    auto w = bounds.getWidth();
    auto h = bounds.getHeight();

    // Halves
    auto leftBound = bounds.removeFromLeft(w / 2);
    auto rightBound = bounds;

    // Colors
    auto lBg = juce::Colours::deeppink;
    auto lFg = juce::Colours::hotpink;
    auto rBg = juce::Colours::mediumslateblue;
    auto rFg = juce::Colours::lightskyblue;


    // Color in background
    g.setColour(lBg);
    g.fillRect(leftBound);

    g.setColour(rBg);
    g.fillRect(rightBound);

    // Calculate relative height the meter should be at
    // Clamps shouldn't be necessary, but just to be safe
    auto lRH = clamp(ratio, 0., 1.);
    auto rRH = clamp(juce::Decibels::decibelsToGain(loudness), 0., 1.);

    auto lH = (int) h * lRH;
    auto rH = (int) h * rRH;

    leftBound.removeFromTop(h - lH);
    rightBound.removeFromTop(h - rH);

    g.setColour(lFg);
    g.fillRect(leftBound);

    g.setColour(rFg);
    g.fillRect(rightBound);
}

void MeterDisplay::resized()
{
    // There shouldn't be anything to do since there's no child components.
    // Painting is stateless and entirely handled in paint().
}
