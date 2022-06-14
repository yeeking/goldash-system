/*
  ==============================================================================

    PolyLeadFollow.h
    Created: 9 Mar 2021 1:23:57pm
    Author:  matthewyk

  ==============================================================================
*/

#pragma once

#include "dinvernoSystem.h"

class PolyLeadFollow :public DinvernoImproviser
{
  public: 
    PolyLeadFollow(int sampleRate);
    virtual void tick() override;
    virtual void generateTick() override;
    virtual void updateTick() override; 
    virtual void setQuantisationMs(double ms) override; 

    virtual void addMidiMessage(const MidiMessage& msg, bool trainFromInput) override;
    virtual void reset() override;

    void follow();
    void lead();

    MidiBuffer getPendingMidiMessages() override;     

  private:
    DinvernoPolyMarkov longTermMarkov{0};// setting samplerate to 0 as we'll use iniitaliser lists to do it properly
    DinvernoPolyMarkov shortTermMarkov{0};
    DinvernoPolyMarkov* currentPoly;
    int noteCounter;
};

