/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AimusoAudioProcessor::AimusoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // calculate 
    // threadedImprovisor.setImproviser(currentImproviser);
    // threadedImprovisor.startThread();
    // // call tick on the improviser every 'n'ms
    startTimer(10);
        
}

AimusoAudioProcessor::~AimusoAudioProcessor()
{
    //threadedImprovisor.stopThread(30);
    stopTimer();

}

void AimusoAudioProcessor::timerCallback()
{
    currentImproviser->tick();
}

//==============================================================================
const juce::String AimusoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AimusoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AimusoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AimusoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AimusoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AimusoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AimusoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AimusoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AimusoAudioProcessor::getProgramName (int index)
{
    return {};
}

void AimusoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AimusoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AimusoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AimusoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AimusoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{   
    // if (midiMessages.getNumEvents() > 0)
    //     DBG("AimusoAudioProcessor::processBlock " + std::to_string(midiMessages.getNumEvents()));
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    
    //threadedImprovisor.setMidiBuffer(midiMessages);
    for (const auto meta : midiMessages){
        auto msg = meta.getMessage();
        currentImproviser->addMidiMessage(msg);
    }
        
    // Get Midi Messages from Improvisor: add to buffer if it is time to send
    int sampleNumber;
    //currentImproviser->tick();
    juce::MidiBuffer toSend = currentImproviser->getPendingMidiMessages();
    juce::MidiBuffer generatedMidi{};

    if (toSend.getNumEvents() > 0){
        for (const auto meta : toSend)
        {
            auto msg = meta.getMessage();
            msg.setTimeStamp(juce::Time::getApproximateMillisecondCounter() * 0.001);
            generatedMidi.addEvent(msg, 0);
        }
    }
    
    if (clearMidiBuffer) {
        juce::MidiMessage allOff = juce::MidiMessage::allNotesOff(1);
        midiMessages.addEvent(allOff,0);
        generatedMidi.addEvent(allOff,0);
        clearMidiBuffer = false;
    }
    
    // Remove Raw midi input and only transmit dinverno generated messages
    midiMessages.swapWith(generatedMidi);
    
}


//==============================================================================
bool AimusoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AimusoAudioProcessor::createEditor()
{
    return new AimusoAudioProcessorEditor (*this);
}

//==============================================================================
void AimusoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AimusoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AimusoAudioProcessor();
}
