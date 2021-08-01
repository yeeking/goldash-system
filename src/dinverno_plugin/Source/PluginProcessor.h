/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "dinvernoSystem.h"
#include "ThreadedImprovisor.h"

//==============================================================================
/**
 * Plugin Processor for VST Plugin: Process the incoming Midi information and pass to the Improvisor Thread/
*/
class Dinverno_pluginAudioProcessor  : public juce::AudioProcessor,
                                                Timer
{
public:
    //==============================================================================
    Dinverno_pluginAudioProcessor();
    ~Dinverno_pluginAudioProcessor() override;

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

    //==============================================================================
    void setCurrentImproviser(DinvernoImproviser* improviser);
    void setCurrentImproviser(String improviserName);
    void resetCurrentImproviser();
    void setImprovisersLoginManager(LogginManager *loggin);
    void tickCurrentImproviser();
    void timerCallback() override;  /** Timer callback*/
    
    //==============================================================================
    // markov models
    DinvernoImproviser* currentImproviser;
    //DinvernoPolyMarkov dinvernoPolyMarkov{44100};
    bool improviserReady = false;
    
private:
    //==============================================================================
    bool clearMidiBuffer = false;
    const static int numPrograms = 4;
    int curProgram = 0;
    std::array<const String, numPrograms> programNames = {String("Practice"),String("ForwardFeedback"),String("ReverseFeedback"),String("NoFeedback")};
    
    ThreadedImprovisor threadedImprovisor;
    /*
    // markov models
    DinvernoImproviser* currentImproviser;
    DinvernoMidiParrot dinvernoParrot{44100};
    DinvernoRandomMidi dinvernoRandomMidi{44100};
    DinvernoRandomEnergy dinvernoRandomEnergy{44100};
    */
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dinverno_pluginAudioProcessor)
};
