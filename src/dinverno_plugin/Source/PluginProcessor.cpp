/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Dinverno_pluginAudioProcessor::Dinverno_pluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       //.withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       //.withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //setCurrentImproviser(&dinvernoPolyMarkov);
    //setCurrentImproviser(&dinvernoPolyMarkov);
    setCurrentImproviser(&threadedImprovisor.dinvernoPolyMarkov);
    threadedImprovisor.startThread();
    
    programChangeBroadcaster = new ChangeBroadcaster();
}

Dinverno_pluginAudioProcessor::~Dinverno_pluginAudioProcessor()
{
    threadedImprovisor.stopThread(30);
}

//==============================================================================
const juce::String Dinverno_pluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Dinverno_pluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Dinverno_pluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Dinverno_pluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Dinverno_pluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Dinverno_pluginAudioProcessor::getNumPrograms()
{
    // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
    return numPrograms > 0 ? numPrograms : 1;
}

int Dinverno_pluginAudioProcessor::getCurrentProgram()
{
    return curProgram;
}

void Dinverno_pluginAudioProcessor::setCurrentProgram (int index)
{
    if (curProgram != index){
        curProgram = index;
        currentImproviser->setFeedbackMode(curProgram);
        programChangeBroadcaster->sendChangeMessage();
    }
}

const juce::String Dinverno_pluginAudioProcessor::getProgramName (int index)
{
    return programNames[index];
}

void Dinverno_pluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Dinverno_pluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Dinverno_pluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Dinverno_pluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void Dinverno_pluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    MidiBuffer generatedMidi;
    
    if (improviserReady){
        
        threadedImprovisor.setMidiBuffer(midiMessages);
        /*
        for (const auto metadata : midiMessages)
        {
            auto message = metadata.getMessage();
            const auto time = metadata.samplePosition;
            
            // Add current midi message to improvisor
            currentImproviser->addMidiMessage(message);
        }
        */
    
        
        // Get Midi Messages from Improvisor: add to buffer if it is time to send
        int sampleNumber;
        //currentImproviser->tick();
        MidiBuffer toSend = currentImproviser->getPendingMidiMessages();

        if (toSend.getNumEvents() > 0){
            //std::cout << "timerCallback sending " << toSend.getNumEvents() << std::endl;
            // MidiBuffer::Iterator iterator (toSend);
            // MidiMessage message;
            // while (iterator.getNextEvent (message, sampleNumber))
            // {
            //     //sendMidi(message);
            //     message.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
            //     //MIDITimeStamp timeStamp = AudioGetCurrentHostTime();
            //     generatedMidi.addEvent(message,0);
            // }
            // myk: new JUCE 6 style message iteration
            for (const auto meta : toSend)
            {
                auto msg = meta.getMessage();
                msg.setTimeStamp(Time::getApproximateMillisecondCounter() * 0.001);
                generatedMidi.addEvent(msg, 0);
            }
        }
         
    }
    
    if (clearMidiBuffer) {
        MidiMessage allOff = MidiMessage::allNotesOff(1);
        midiMessages.addEvent(allOff,0);
        generatedMidi.addEvent(allOff,0);
        clearMidiBuffer = false;
    }
    
    // Remove Raw midi input and only transmit dinverno generated messages
    midiMessages.swapWith(generatedMidi);
    
}

//==============================================================================
void Dinverno_pluginAudioProcessor::setPosNegFeedbackCC(int cc)
{
    currentImproviser->setPosNegFeedbackController(cc);
}

void Dinverno_pluginAudioProcessor::setLeadFollowFeedbackCC(int cc)
{
    currentImproviser->setLeadFollowFeedbackController(cc);
}

void Dinverno_pluginAudioProcessor::setFeedbackBandwidth(int fbRange)
{
    currentImproviser->setFeedbackBandwidth(fbRange);
}

int Dinverno_pluginAudioProcessor::getPosNegFeedbackCC()
{
    return currentImproviser->getPosNegFeedbackController();
}

int Dinverno_pluginAudioProcessor::getLeadFollowFeedbackCC()
{
    return currentImproviser->getLeadFollowFeedbackController();
}

int Dinverno_pluginAudioProcessor::getFeedbackBandwidth()
{
    return currentImproviser->getFeedbackBandwidthPercent();
}

//==============================================================================
bool Dinverno_pluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Dinverno_pluginAudioProcessor::createEditor()
{
    return new Dinverno_pluginAudioProcessorEditor (*this);
}

//==============================================================================
void Dinverno_pluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Dinverno_pluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
void Dinverno_pluginAudioProcessor::setCurrentImproviser(DinvernoImproviser *improviser)
{
    // Set the current improviser
    currentImproviser = improviser;
    improviserReady = true;
    startTimer(50);
}

void Dinverno_pluginAudioProcessor::setCurrentImproviser(String improvierName)
{
    /*
    if (improvierName == "PARROT"){
        currentImproviser = &dinvernoParrot;
    }else if (improvierName == "RANDOM MIDI"){
        currentImproviser = &dinvernoRandomMidi;
    }else if (improvierName == "RANDOM ENERGY"){
        currentImproviser = &dinvernoRandomEnergy;
    }else if (improvierName == "POLY"){
        currentImproviser = &dinvernoPolyMarkov;
    }
     */
}

void Dinverno_pluginAudioProcessor::resetCurrentImproviser()
{
    // Set the current improviser
    currentImproviser->reset();
    clearMidiBuffer = true;
}

void Dinverno_pluginAudioProcessor::setImprovisersLoginManager(LogginManager *loggin)
{
    // Set all improvisers LogginManager
    /*
    dinvernoParrot.setLogginManager(loggin);
    dinvernoRandomMidi.setLogginManager(loggin);
    dinvernoRandomEnergy.setLogginManager(loggin);
    dinvernoPolyMarkov.setLogginManager(loggin);
     */
}

void Dinverno_pluginAudioProcessor::timerCallback()
{
    tickCurrentImproviser();
}

void Dinverno_pluginAudioProcessor::tickCurrentImproviser()
{
    // Timer event for current improviser
    currentImproviser->tick();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dinverno_pluginAudioProcessor();
}
