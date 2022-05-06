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
class AimusoAudioProcessor  : public juce::AudioProcessor
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


    //==== custom stuff relating to controlling the algorithms ///
    /** sets the quantisation on the improvisers in ms */
    void setQuantisationMs(double ms);
    /** set the improviser to lead mode */
    void leadMode();
    /** set the improviser to follow mode */
    void followMode(); 
    /** clears the memories on all the models*/   
    void resetModels();

private: // private fields for PluginProcessor

    bool clearMidiBuffer = false;

    //ThreadedImprovisor threadedImprovisor;
    /** initialise a polylead follow*/    
    PolyLeadFollow polyLeadFollow{44100};    
    /** assign the polyleadfollow to the currentImproviser
     * in case at some point we want other improvisers available 
    */
    DinvernoImproviser* currentImproviser{&polyLeadFollow}; 
    //==============================================================================


    class UpdateTicker : public juce::Timer
    {
        public: 
        void timerCallback() override {
            if (improviser != 0) improviser->updateTick();
            else DBG("UpdateTicker: no improviser. call setImproviser");
        }
        void setImproviser(DinvernoImproviser* impro) {improviser = impro;}
        private:
        DinvernoImproviser* improviser{0}; 
    };
    class GenerateTicker : public juce::Timer
    {
        public: 
        void timerCallback() override {
            if (improviser != 0) improviser->generateTick();
            else DBG("GenerateTicker: no improviser. call setImproviser");
        }        
        void setImproviser(DinvernoImproviser* impro) {improviser = impro;}
        private:
        DinvernoImproviser* improviser{0}; 
    };
    UpdateTicker updateTicker{};
    GenerateTicker generateTicker{};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AimusoAudioProcessor)
};
