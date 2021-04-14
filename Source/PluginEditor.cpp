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

    auto modeBox = leftThird.removeFromTop(leftThird.getHeight()/2);
    modeSwitch.setBounds(modeBox);
    addAndMakeVisible(modeLabel);
    
    auto channelBox = leftThird;
    channelNumbox.setBounds(channelBox);
    addAndMakeVisible(channelLabel);
    
    /*
    channelModePanel.addPanel(0, &modeSwitch, false);
    channelModePanel.addPanel(1, &channelNumbox, false);
     */

    auto loudnessBox = centerThird;
    loudnessMeter.setBounds(loudnessBox);
    addAndMakeVisible(loudnessLabel);
    attachToComponent(loudnessLabel, loudnessMeter, 2);
    
    // TODO: only do anything with ratioMeter in the After instance
    auto ratioBox = rightThird;
    ratioMeter.setBounds(ratioBox);
    addAndMakeVisible(ratioLabel);
    attachToComponent(ratioLabel, ratioMeter, 2);

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
    label.setFont(juce::Font(18));

    juce::Rectangle<int> labelBounds = component.getBounds();

    int originalCentreX = labelBounds.getCentreX();
    labelBounds.setLeft(originalCentreX - getTextWidth(label));
    labelBounds.setRight(originalCentreX + getTextWidth(label));

    labelBounds.setTop(component.getBounds().getBottom() + offset);
    labelBounds.setBottom(component.getBounds().getBottom() + label.getFont().getHeight() + offset);

    label.setBounds(labelBounds.translated(0, 1));
}

