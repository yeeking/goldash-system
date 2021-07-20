/*
  ==============================================================================

    MidiReceiver.h
    Created: 25 Oct 2019 12:26:53am
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#pragma once

class MidiReceiver {
public:
  virtual ~MidiReceiver() {}
  virtual void receiveMidi(const MidiMessage& message) = 0;
  
};
