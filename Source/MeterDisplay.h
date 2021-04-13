#pragma once

#include <JuceHeader.h>

/**
 * A simple vertical level meter component.
 * Given some MeterDisplay m, render it by regularly calling:
 *
 *      m.setLevel(level);
 *      m.repaint();
 */
class MeterDisplay : public juce::Component {
public:
    MeterDisplay(juce::Colour fg, juce::Colour bg)
        : background(bg), foreground(fg), level(0.0)
    {}

    /** Clamps l to be between 0.0 and 1.0 */
    void setLevel(float l);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Colour background;
    juce::Colour foreground;

    float level;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterDisplay)
};
