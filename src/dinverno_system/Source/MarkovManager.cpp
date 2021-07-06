/*
  ==============================================================================

    MarkovManager.cpp
    Created: 30 Oct 2019 3:28:02pm
    Author:  matthew

  ==============================================================================
*/

#include "MarkovManager.h"
#include <iostream>

MarkovManager::MarkovManager()
{
  inputMemory.assign(250, "0");
  outputMemory.assign(250, "0");
  
}
MarkovManager::~MarkovManager()
{
  
}
void MarkovManager::reset()
{
    inputMemory.assign(250, "0");
    outputMemory.assign(250, "0");
    chain.reset();
}
void MarkovManager::putEvent(state_single event)
{
  // add the observation to the markov 
  chain.addObservationAllOrders(inputMemory, event);
  // update the input memory
  addStateToStateSequence(inputMemory, event);
}
state_single MarkovManager::getEvent()
{
  // get an observation
  state_single event = chain.generateObservation(outputMemory, 100);
  // check the output
  // update the outputMemory
  addStateToStateSequence(outputMemory, event);
  return event; 
}

void MarkovManager::addStateToStateSequence(state_sequence& seq, state_single new_state){
  // shift everything across
  for (auto i=1;i<seq.size();i++)
  {
    seq[i-1] = seq[i];
  }
  // replace the final state with the new one
  seq[seq.size()-1] = new_state;
}

int MarkovManager::getOrderOfLastEvent()
{
  return chain.getOrderOfLastMatch();
}
