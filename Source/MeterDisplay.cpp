#include "MeterDisplay.h"

static inline float clamp(float v, float min, float max)
{
    return v < min ? min : v > max ? max : v;
}

void MeterDisplay::setLevel(float l)
{
    level = clamp(l, 0.0, 1.0);
}

void MeterDisplay::paint(juce::Graphics& g)
{
    juce::Rectangle<int> bounds = getLocalBounds();

    // Color in background
    g.setColour(background);
    g.fillRect(bounds); // TODO: is it better to use g.fillAll() here?

    // Calculate how tall the level bar should be
    int height = bounds.getHeight();
    int levelHeight = static_cast<int>(height * level);

    // Chop off the top from meter
    bounds.removeFromTop(height - levelHeight);

    // Color in the level bar
    g.setColour(foreground);
    g.fillRect(bounds);
}

void MeterDisplay::resized()
{
    // There shouldn't be anything to do since there's no child components.
    // Painting is stateless and entirely handled in paint().
}
