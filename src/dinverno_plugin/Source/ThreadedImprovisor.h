/*
  ==============================================================================

    ThreadedImprovisor.h
    Created: 1 Aug 2021 10:40:53am
    Author:  Sam Trolland

  ==============================================================================
*/

#pragma once

#include "dinvernoSystem.h"

/**
 * Manages The Dinverno Poly Markov Model in its own thread to improve audio quality and avoid audible pops.
 */
class ThreadedImprovisor : public Thread
{
public:
    ThreadedImprovisor() : Thread ("Improvisor Thread") {}
    void run() override;
    void setMidiBuffer(juce::MidiBuffer midiMessages);
    
    //float value = 0.0f;
    MidiBuffer midiBuffer;
    std::atomic<bool> processingBuffer {false};
    DinvernoPolyMarkov dinvernoPolyMarkov{44100};
};
