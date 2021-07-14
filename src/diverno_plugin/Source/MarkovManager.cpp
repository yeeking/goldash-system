/*
  ==============================================================================

    MarkovManager.cpp
    Created: 30 Oct 2019 3:28:02pm
    Author:  matthew

  ==============================================================================
*/

#include "MarkovManager.h"
#include <iostream>

MarkovManager::MarkovManager(unsigned long chainEventMemoryLength) : maxChainEventMemory{chainEventMemoryLength}, chainEventIndex{0}
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
  // store the event in case we want to provide negative or positive feedback to the chain
  // later
  rememberChainEvent(chain.getLastMatch());
  return event; 
}

void MarkovManager::addStateToStateSequence(state_sequence& seq, state_single new_state){
  // shift everything across
  for (long unsigned int i=1;i<seq.size();i++)
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


void MarkovManager::rememberChainEvent(state_and_observation sObs)
{
  // the memory of chain events is not full yet
  if (chainEvents.size() < maxChainEventMemory)
  {
    chainEvents.push_back(sObs);
  }
  else 
  {
    // the memory of chain events is full - do FIFO
    chainEvents[chainEventIndex] = sObs;
    chainEventIndex = (chainEventIndex + 1) % maxChainEventMemory;
  }
}

void MarkovManager::giveNegativeFeedback()
{
  // remove all recently used mappings
  for (state_and_observation& so : chainEvents)
  {
    chain.removeMapping(so.first, so.second);
  }
}


void MarkovManager::givePositiveFeedback()
{
  // amplify all recently used mappings
  for (state_and_observation& so : chainEvents)
  {
    chain.amplifyMapping(so.first, so.second);
  }
}