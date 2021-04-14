#include "MeterDisplay.h"

static inline float clamp(float v, float minv, float maxv)
{
    return v < minv ? minv : v > maxv ? maxv : v;
}

void MeterDisplay::setLevel(float l)
{
    if (bipolar)
        level = clamp(l, -limit, limit);
    else
        level = clamp(l, -limit, 0.0);
}

void MeterDisplay::paint(juce::Graphics& g)
{
    juce::Rectangle<int> bounds = getLocalBounds();
    int height = bounds.getHeight();
    int minLim = -limit, maxLim = bipolar ? limit : 0.0;

    // Color in background
    g.setColour(background);
    g.fillRect(bounds); // TODO: is it better to use g.fillAll() here?

    // Draw middle line for bipolar
    if (bipolar) {
        g.setColour(accent);
        g.drawHorizontalLine(height / 2, 0.0, 0.0);
    }

    // Calculate height of foreground rectangle representing the level
    float ratio = (level - minLim) / (maxLim - minLim); // between 0.0 and 1.0
    int levelHeight = static_cast<int>(height * ratio);

    if (bipolar) {
        if (level < 0.0) {
            bounds.removeFromTop(height / 2);
            bounds.removeFromBottom((height / 2) - levelHeight);
        } else {
            bounds.removeFromBottom(height / 2);
            bounds.removeFromTop((height / 2) - levelHeight);
        }
    } else {
        bounds.removeFromTop(height - levelHeight);
    }

    // Fill in the level bar
    g.setColour(foreground);
    g.fillRect(bounds);

}

void MeterDisplay::resized()
{
    // There shouldn't be anything to do since there's no child components.
    // Painting is stateless and entirely handled in paint().
}
