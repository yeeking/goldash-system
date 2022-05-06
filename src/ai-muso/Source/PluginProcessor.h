/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PolyLeadFollow.h"

//==============================================================================
/**
*/
class AimusoAudioProcessor  : public juce::AudioProcessor, juce::Timer
{
public:
    //==============================================================================
    AimusoAudioProcessor();
    ~AimusoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

//    void setCurrentImproviser(DinvernoImproviser* improviser);
    //==== timer override
    void timerCallback() override;

private:
    
    bool clearMidiBuffer = false;
    

private: // private fields for PluginProcessor
    //ThreadedImprovisor threadedImprovisor;
    /** initialise a polylead follow*/    
    PolyLeadFollow polyLeadFollow{44100};    
    /** assign the polyleadfollow to the currentImproviser
     * in case at some point we want other improvisers available 
    */
    DinvernoImproviser* currentImproviser{&polyLeadFollow}; 
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AimusoAudioProcessor)
};
