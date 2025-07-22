/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyFirstVstAudioProcessorEditor::MyFirstVstAudioProcessorEditor (MyFirstVstAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    // bit depth
    
    bitDepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bitDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(bitDepthSlider);
    bitDepthLabel.setText("Bit Depth", juce::dontSendNotification);
    bitDepthLabel.attachToComponent(&bitDepthSlider, false);
    bitDepthLabel.setJustificationType(juce::Justification::centred);
    
    // rate reduction
    
    rateReductionSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    rateReductionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(rateReductionSlider);
    rateReductionLabel.setText("Rate Reduce", juce::dontSendNotification);
    rateReductionLabel.attachToComponent(&rateReductionSlider, false);
    rateReductionLabel.setJustificationType(juce::Justification::centred);
    
    // mix
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(mixSlider);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    mixLabel.setJustificationType(juce::Justification::centred);
    
    // get APVTS from processor
    auto& valueTreeState = audioProcessor.getValueTreeState();
    
    bitDepthAttachment = std::make_unique<SliderAttachment>(valueTreeState, "bitDepth", bitDepthSlider);
    rateReductionAttachment = std::make_unique<SliderAttachment>(valueTreeState, "rateReduction", rateReductionSlider);
    mixAttachment = std::make_unique<SliderAttachment>(valueTreeState, "mix", mixSlider);

    setSize (300, 200);
}

MyFirstVstAudioProcessorEditor::~MyFirstVstAudioProcessorEditor()
{
}

//==============================================================================
void MyFirstVstAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void MyFirstVstAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto sliderWidth = bounds.getWidth() / 3;
    
    bitDepthSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(10));
    rateReductionSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(10));
    mixSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(10));
}
