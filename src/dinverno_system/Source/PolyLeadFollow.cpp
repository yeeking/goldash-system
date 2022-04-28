/*
  ==============================================================================

    PolyLeadFollow.cpp
    Created: 9 Mar 2021 1:23:57pm
    Author:  matthewyk

  ==============================================================================
*/

#include "PolyLeadFollow.h"

PolyLeadFollow::PolyLeadFollow(int sampleRate)
 :  DinvernoImproviser{sampleRate},  longTermMarkov{sampleRate}, shortTermMarkov{sampleRate}, noteCounter{0}
 {
   currentPoly = &longTermMarkov;
 }

void PolyLeadFollow::tick()
{
  currentPoly->tick();
}

void PolyLeadFollow::addMidiMessage(const MidiMessage& msg)
{
  DBG("PolyLeadFollow::addMidiMessage " << noteCounter);
  
  currentPoly->addMidiMessage(msg);
  noteCounter ++;
  if (noteCounter > 64)
  {
    DBG("PolyLeadFollow::addMidiMessage resetting short term model");
  
    shortTermMarkov.reset();
    noteCounter = 0;
  }
}

void PolyLeadFollow::reset()
{
  longTermMarkov.reset();
  shortTermMarkov.reset();
}

MidiBuffer PolyLeadFollow::getPendingMidiMessages()
{
  return currentPoly->getPendingMidiMessages();
}     


void PolyLeadFollow::lead()
{
  currentPoly = &longTermMarkov;
}

void PolyLeadFollow::follow()
{
  currentPoly = &shortTermMarkov;
}

