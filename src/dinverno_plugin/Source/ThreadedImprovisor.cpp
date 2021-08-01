/*
  ==============================================================================

    ThreadedImprovisor.cpp
    Created: 1 Aug 2021 10:40:53am
    Author:  Sam Trolland

  ==============================================================================
*/

#include "ThreadedImprovisor.h"


void ThreadedImprovisor::run()
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
            dinvernoPolyMarkov.addMidiMessage(message);
        }
        
        // Clear processed messages
        midiBuffer.clear();
        
        // Unlock thread and wait for next message to process
        processingBuffer.store(false);
        wait(-1);
    }
}

void ThreadedImprovisor::setMidiBuffer(juce::MidiBuffer midiMessages){
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
