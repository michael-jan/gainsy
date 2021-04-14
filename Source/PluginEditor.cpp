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
    , modeLabel("modeLabel", "mode")
    , channelLabel("channelLabel", "channel")
    , ratioLabel("ratioLabel", "diff")
    , beforeLoudnessLabel("beforeLoudnessLabel", "before")
    , afterLoudnessLabel("afterLoudnessLabel", "after")
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

    setSize(280, 180);
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
    juce::Rectangle<int> bounds = getLocalBounds().withTrimmedBottom(juce::Font(fontSize).getHeight());

    // TODO: proper layout. For now, just arrange the GUI in slices:
    auto w = bounds.getWidth() / 4;

    // NOTE: bounds.removeFromDIRECTION() modifies the rectangle in place,
    // returning the stripped off portion.
    auto r1 = bounds.removeFromLeft(w);
    auto r2 = bounds.removeFromLeft(w);
    auto r3 = bounds.removeFromLeft(w);
    auto r4 = bounds;

    auto modeBox = r1.removeFromTop(r1.getHeight()/2);
    modeBox = modeBox.withSizeKeepingCentre(modeBox.getWidth()/3.5,
                                            modeBox.getHeight()/5);
    modeSwitch.setBounds(modeBox);
    addAndMakeVisible(modeLabel);
    attachToComponent(modeLabel, modeSwitch, 4);

    auto channelBox = r1.withSizeKeepingCentre(r1.getWidth()/3,
                                               r1.getHeight()/4);
    channelNumbox.setBounds(channelBox);
    addAndMakeVisible(channelLabel);
    attachToComponent(channelLabel, channelNumbox, 4);

    auto beforeLoudnessBox = r2.withSizeKeepingCentre(r2.getWidth()/5,
                                                      r2.getHeight()/1.6);
    beforeLoudnessMeter.setBounds(beforeLoudnessBox);
    addAndMakeVisible(beforeLoudnessLabel);
    attachToComponent(beforeLoudnessLabel, beforeLoudnessMeter, 4);

    auto ratioBox = r3.withSizeKeepingCentre(r3.getWidth()/5,
                                             r3.getHeight()/1.6);
    ratioMeter.setBounds(ratioBox);
    addAndMakeVisible(ratioLabel);
    attachToComponent(ratioLabel, ratioMeter, 4);

    auto afterLoudnessBox = r4.withSizeKeepingCentre(r4.getWidth()/5,
                                                     r4.getHeight()/1.6);
    afterLoudnessMeter.setBounds(afterLoudnessBox);
    addAndMakeVisible(afterLoudnessLabel);
    attachToComponent(afterLoudnessLabel, afterLoudnessMeter, 4);

}

void GainsyAudioProcessorEditor::timerCallback()
{
    // TODO: only do anything with ratioMeter in the After instance
    beforeLoudnessMeter.setLevel(audioProcessor.getBeforeLoudness());
    beforeLoudnessMeter.repaint();

    float r = juce::Decibels::gainToDecibels(audioProcessor.getRatio());
//    DBG("Setting ratio: ");
//    DBG(r);
    ratioMeter.setLevel(r);
    ratioMeter.repaint();

    afterLoudnessMeter.setLevel(audioProcessor.getAfterLoudness());
    afterLoudnessMeter.repaint();
}

int GainsyAudioProcessorEditor::getTextWidth(juce::Label& label) {
    return label.getFont().getStringWidth(label.getText());
}

void GainsyAudioProcessorEditor::attachToComponent(juce::Label& label, juce::Component& component, int offset) {
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(fontSize));

    juce::Rectangle<int> labelBounds = component.getBounds();

    int originalCentreX = labelBounds.getCentreX();
    labelBounds.setLeft(originalCentreX - getTextWidth(label));
    labelBounds.setRight(originalCentreX + getTextWidth(label));

    labelBounds.setTop(component.getBounds().getBottom() + offset);
    labelBounds.setBottom(component.getBounds().getBottom() + label.getFont().getHeight() + offset);

    label.setBounds(labelBounds.translated(0, 1));
}
