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
    // if bipolar,  -30.0 < level < 30.0
    // else,        -30.0 < level < 0.0
    MeterDisplay(juce::Colour bg, juce::Colour fg, juce::Colour ac, bool b)
        : background(bg), foreground(fg), accent(ac), bipolar(b)
        , level(0.0), limit(60.0)
    {}

    void setLevel(float l);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    const juce::Colour background, foreground, accent;
    const bool bipolar;
    const float limit;

    float level;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterDisplay)
};
