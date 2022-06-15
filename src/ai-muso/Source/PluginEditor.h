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
class AimusoAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                           juce::Slider::Listener, 
                                           juce::Button::Listener
{
public:
    AimusoAudioProcessorEditor (AimusoAudioProcessor&);
    ~AimusoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    /** respond to the sliders*/
    void sliderValueChanged(Slider* slider) override;
    /** respond to the buttons*/
    void buttonClicked(Button* button) override;  
        
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AimusoAudioProcessor& audioProcessor;
    // model load and save controls
    juce::TextButton loadModelBtn;
    juce::TextButton saveModelBtn;
    juce::FileChooser fChooser{"Select a file..."};
    
    juce::TextButton trainToggle;
        
    
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
    juce::TextButton resetModelBtn;
    
    void setupUI();
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AimusoAudioProcessorEditor)
};
