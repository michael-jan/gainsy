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
    , beforeLoudnessMeter(juce::Colours::lightskyblue, juce::Colours::mediumslateblue, juce::Colours::black, false)
    , ratioMeter(juce::Colours::hotpink, juce::Colours::deeppink, juce::Colours::black, true)
    , afterLoudnessMeter(juce::Colours::lightskyblue, juce::Colours::mediumslateblue, juce::Colours::black, false)
{
    // TODO: does setting the size of components here even matter..?
    /* modeSwitch.setSize(40, 120); */
    /* channelNumbox.setSize(40, 120); */
    /* ratioMeter.setSize(20, 120); */
    /* afterLoudnessMeter.setSize(20, 120); */

    channelNumbox.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    channelNumbox.setSliderSnapsToMousePosition(false);
    channelNumbox.setMouseDragSensitivity(350);
    channelNumbox.setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(modeSwitch);
    addAndMakeVisible(channelNumbox);

    // TODO: only display ratioMeter in the After instance
    addAndMakeVisible(beforeLoudnessMeter);
    addAndMakeVisible(ratioMeter);
    addAndMakeVisible(afterLoudnessMeter);

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

    // more quick and dirty splitting
    bounds = rightThird;
    leftThird = bounds.removeFromLeft(w);
    rightThird = bounds.removeFromRight(w);
    centerThird = bounds;

    // TODO: only do anything with ratioMeter in the After instance
    beforeLoudnessMeter.setBounds(leftThird);
    ratioMeter.setBounds(centerThird);
    afterLoudnessMeter.setBounds(rightThird);

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

void GainsyAudioProcessorEditor::timerCallback()
{
    // TODO: only do anything with ratioMeter in the After instance
    beforeLoudnessMeter.setLevel(audioProcessor.getBeforeLoudness());
    beforeLoudnessMeter.repaint();

    ratioMeter.setLevel(juce::Decibels::gainToDecibels(audioProcessor.getRatio()));
    ratioMeter.repaint();

    afterLoudnessMeter.setLevel(audioProcessor.getAfterLoudness());
    afterLoudnessMeter.repaint();
}
