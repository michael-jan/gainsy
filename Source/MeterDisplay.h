#pragma once

#include <JuceHeader.h>

class MeterDisplay : public juce::Component {
public:
    MeterDisplay() : ratio(0.0), loudness(0.0) {}

    void setLevels(float r, float l)
    { ratio = r; loudness = l; }

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    float ratio;
    float loudness;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterDisplay)
};
