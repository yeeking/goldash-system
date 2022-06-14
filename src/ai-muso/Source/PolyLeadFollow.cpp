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
  
}
void PolyLeadFollow::generateTick()
{
  currentPoly->generateTick();
}
void PolyLeadFollow::updateTick()
{
  currentPoly->updateTick();
}

void PolyLeadFollow::addMidiMessage(const MidiMessage& msg, bool trainFromInput)
{
  //DBG("PolyLeadFollow::addMidiMessage " << noteCounter);
  // only want notes for now 
  if (msg.isNoteOn() || msg.isNoteOff()){
    currentPoly->addMidiMessage(msg, trainFromInput);
    noteCounter ++;
    if (noteCounter > 64)
    {
      DBG("PolyLeadFollow::addMidiMessage resetting short term model");
    
      shortTermMarkov.reset();
      noteCounter = 0;
    }
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

void PolyLeadFollow::setQuantisationMs(double ms)
{
  shortTermMarkov.setQuantisationMs(ms);
  longTermMarkov.setQuantisationMs(ms);
}
