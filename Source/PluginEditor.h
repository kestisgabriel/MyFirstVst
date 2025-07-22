/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MyFirstVstAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MyFirstVstAudioProcessorEditor (MyFirstVstAudioProcessor&);
    ~MyFirstVstAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyFirstVstAudioProcessor& audioProcessor;
    
    juce::Slider bitDepthSlider;
    juce::Slider rateReductionSlider;
    juce::Slider mixSlider;
    
    juce::Label bitDepthLabel;
    juce::Label rateReductionLabel;
    juce::Label mixLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyFirstVstAudioProcessorEditor)
};
