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

    // Color in background
    g.setColour(background);
    g.fillRect(bounds); // TODO: is it better to use g.fillAll() here?

    // Calculate height of foreground rectangle representing the level
    if (bipolar) {
        int halfHeight = height / 2;
        if (level < 0.0) {
            float ratio = clamp((level + limit) / limit, 0.0, 1.0);
            int levelHeight = static_cast<int>(height * ratio / 2);
            bounds.removeFromTop(halfHeight);
            bounds.removeFromBottom(halfHeight - levelHeight);
        } else {
            float ratio = clamp(level / limit, 0.0, 1.0);
            int levelHeight = static_cast<int>(height * ratio / 2);
            bounds.removeFromBottom(halfHeight);
            bounds.removeFromTop(halfHeight - levelHeight);
        }
    } else {
        float ratio = clamp((level + limit) / limit, 0.0, 1.0);
        int levelHeight = static_cast<int>(height * ratio);
        bounds.removeFromTop(height - levelHeight);
    }

    // Fill in the level bar
    g.setColour(foreground);
    g.fillRect(bounds);

    // Draw middle line for bipolar
    if (bipolar) {
        g.setColour(accent);
        g.drawHorizontalLine(height / 2, 0.0, 0.0);
    }
}

void MeterDisplay::resized()
{
    // There shouldn't be anything to do since there's no child components.
    // Painting is stateless and entirely handled in paint().
}
