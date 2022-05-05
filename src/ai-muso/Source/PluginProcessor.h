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

//    void setCurrentImproviser(DinvernoImproviser* improviser);

private:
    
    bool clearMidiBuffer = false;
    
    class ThreadedImprovisor : public juce::Thread
    {
    public:
        ThreadedImprovisor() : juce::Thread ("Improvisor Thread") {}
        
        void run() override
        {
            while (! threadShouldExit())
            {
                // Thread actions go here
                // Lock thread while processing midibuffer
                processingBuffer.store(true);
                for (const auto metadata : midiBuffer)
                {
                    // Add current midi message to improvisor
                    auto message = metadata.getMessage();
                    //dinvernoPolyMarkov.addMidiMessage(message);
                }
                
                // Clear processed messages
                midiBuffer.clear();
                
                // Unlock thread and wait for next message to process
                processingBuffer.store(false);
                wait(-1);
            }
        }
        
        void setMidiBuffer(juce::MidiBuffer midiMessages){
            // Thread locked while processing current buffer
            if(processingBuffer.load())
                return;
            
            // Thread not locked: add new midi messages for processing
            if (midiMessages.getNumEvents() > 0) {
                for (const auto metadata : midiMessages)
                {
                    auto message = metadata.getMessage();
                    const auto time = metadata.samplePosition;
                    
                    // Add current midi message to improvisor
                    midiBuffer.addEvent(message,time);
                }
                
                // Notify thread new data to process
                notify();
            }
        }
        
        //float value = 0.0f;
        juce::MidiBuffer midiBuffer;
        std::atomic<bool> processingBuffer {false};
        //DinvernoPolyMarkov dinvernoPolyMarkov{44100};
    };
    
    ThreadedImprovisor threadedImprovisor;    
    PolyLeadFollow polyLeadFollow{44100};    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AimusoAudioProcessor)
};
