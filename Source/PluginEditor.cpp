/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
GainsyAudioProcessorEditor::GainsyAudioProcessorEditor(GainsyAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
      // TODO: don't hardcode these colors
    , ratioMeter(juce::Colours::deeppink, juce::Colours::hotpink)
    , loudnessMeter(juce::Colours::mediumslateblue, juce::Colours::lightskyblue)
{
    // TODO: does setting the size of components here even matter..?
    modeSwitch.setSize(40, 120);
    channelNumbox.setSize(40, 120);
    ratioMeter.setSize(20, 120);
    loudnessMeter.setSize(20, 120);

    channelNumbox.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    channelNumbox.setSliderSnapsToMousePosition(false);
    channelNumbox.setMouseDragSensitivity(350);
    channelNumbox.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(modeSwitch);
    addAndMakeVisible(channelNumbox);

    // TODO: only display ratioMeter in the After instance
    addAndMakeVisible(ratioMeter);
    addAndMakeVisible(loudnessMeter);

    modeAttachment.reset(new ButtonAttachment(p.getParams(), "MODE", modeSwitch));
    channelAttachment.reset(new SliderAttachment(p.getParams(), "CHANNEL", channelNumbox));

    startTimerHz(refreshRate);

    setSize(120, 360);
}

GainsyAudioProcessorEditor::~GainsyAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void GainsyAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GainsyAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();

    // TODO: proper layout. For now, just arrange the GUI in slices:
    auto w = bounds.getWidth() / 3;

    // NOTE: bounds.removeFromDIRECTION() modifies the rectangle in place,
    // returning the stripped off portion.
    auto leftThird = bounds.removeFromLeft(w);
    auto rightThird = bounds.removeFromRight(w);
    auto centerThird = bounds;

    modeSwitch.setBounds(leftThird);
    channelNumbox.setBounds(centerThird);

    loudnessMeter.setBounds(rightThird.removeFromLeft(rightThird.getWidth()/2));
    // TODO: only do anything with ratioMeter in the After instance
    ratioMeter.setBounds(rightThird);

    auto JuceCSS = // use the power of CSS for best user experience
        "body {"
        "  font-family: Helvetica, arial, sans-serif;"
        "  font-size: 14px;"
        "  line-height: 1.6;"
        "  padding-top: 10px;"
        "  padding-bottom: 10px;"
        "  background-color: white;"
        "  padding: 30px; }"
        ""
        "h1 {"
        "  font-size: 28px;"
        "  color: black; }"
        ""
        "h2 {"
        "  font-size: 24px;"
        "  border-bottom: 1px solid #cccccc;"
        "  color: black; }"
        ""
        "h3 {"
        "  font-size: 18px; }"
        ""
        "h4 {"
        "  font-size: 16px; }"
        ""
        "h5 {"
        "  font-size: 14px; }"
        ""
        "h6 {"
        "  color: #777777;"
        "  font-size: 14px; }"
        ""
        "p, blockquote, ul, ol, dl, li, table, pre {"
        "  margin: 15px 0; }"
        ""
        "li p.first {"
        "  display: inline-block; }"
        "li {"
        "  margin: 0; }"
        "ul, ol {"
        "  padding-left: 30px; }"
        "";
}

void GainsyAudioProcessorEditor::timerCallback ()
{
    loudnessMeter.setLevel(juce::Decibels::decibelsToGain(audioProcessor.getLoudness()));
    loudnessMeter.repaint();

    // TODO: only do anything with ratioMeter in the After instance
    ratioMeter.setLevel(audioProcessor.getRatio());
    ratioMeter.repaint();
}
