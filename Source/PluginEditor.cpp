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
    , modeLabel("modeLabel", "mode")
    , channelLabel("channelLabel", "channel")
    , ratioLabel("ratioLabel", "ratio")
    , loudnessLabel("loudnessLabel", "loudness")
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

    auto w = bounds.getWidth() / 3;

    // NOTE: bounds.removeFromDIRECTION() modifies the rectangle in place,
    // returning the stripped off portion.
    auto leftThird = bounds.removeFromLeft(w);
    auto rightThird = bounds.removeFromRight(w);
    auto centerThird = bounds;

    auto modeBox = leftThird.removeFromTop(leftThird.getHeight()/2);
    modeBox = modeBox.withSizeKeepingCentre(modeBox.getWidth()/4.2,
                                            modeBox.getHeight()/5);
    modeSwitch.setBounds(modeBox);
    addAndMakeVisible(modeLabel);
    attachToComponent(modeLabel, modeSwitch, 4);
    
    auto channelBox = leftThird.withSizeKeepingCentre(leftThird.getWidth()/3,
                                                      leftThird.getHeight()/4);
    channelNumbox.setBounds(channelBox);
    addAndMakeVisible(channelLabel);
    attachToComponent(channelLabel, channelNumbox, 4);

    auto loudnessBox = centerThird.withSizeKeepingCentre(centerThird.getWidth()/5,
                                                         centerThird.getHeight()/1.6);
    loudnessMeter.setBounds(loudnessBox);
    addAndMakeVisible(loudnessLabel);
    attachToComponent(loudnessLabel, loudnessMeter, 4);
    
    // TODO: only do anything with ratioMeter in the After instance
    auto ratioBox = rightThird.withSizeKeepingCentre(rightThird.getWidth()/5,
                                                     rightThird.getHeight()/1.6);
    ratioMeter.setBounds(ratioBox);
    addAndMakeVisible(ratioLabel);
    attachToComponent(ratioLabel, ratioMeter, 4);

}

void GainsyAudioProcessorEditor::timerCallback ()
{
    loudnessMeter.setLevel(juce::Decibels::decibelsToGain(audioProcessor.getLoudness()));
    loudnessMeter.repaint();
    
    // TODO: only do anything with ratioMeter in the After instance
    ratioMeter.setLevel(audioProcessor.getRatio());
    ratioMeter.repaint();
    
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

