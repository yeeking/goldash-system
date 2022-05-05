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
class AimusoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AimusoAudioProcessorEditor (AimusoAudioProcessor&);
    ~AimusoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
        
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AimusoAudioProcessor& audioProcessor;
    // model load and save controls
    juce::TextButton loadModelBtn;
    juce::TextButton saveModelBtn;
    juce::Label currentModelLabel;
    
    // midi channel select controls
    juce::Slider midiInSelector;
    juce::Slider midiOutSelector;
    juce::Label midiInLabel;
    juce::Label midiOutLabel;
    
    
    // quantise
    juce::Slider quantiseSelector;
    juce::Label quantiseLabel;
    
    // group for mode buttons
    juce::GroupComponent modeBox;
    juce::TextButton leadModeBtn;
    juce::TextButton interactModeBtn;
    juce::TextButton followModeBtn;
    
    void setupUI();
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AimusoAudioProcessorEditor)
};
