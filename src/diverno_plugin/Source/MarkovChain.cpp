/*
  ==============================================================================

    MarkovChain.cpp
    Created: 25 Oct 2019 6:47:13am
    Author:  matthew

  ==============================================================================
*/

#include "MarkovChain.h"
#include <iostream>
#include <ctime>

MarkovChain::MarkovChain()
{
  srand((int)time(NULL));
  maxOrder = 1000;
}

MarkovChain::~MarkovChain()
{

}

void MarkovChain::addObservation(const state_sequence& prevState, state_single currentState)
{
  std::cout << "MarkovChain::addObservation adding observation " << currentState << std::endl; 
  if (currentState == "0")
  {
    std::cout << "MarkovChain::addObservation received invalid state. Ignoring it " << currentState << std::endl;
    //throw "MarkovChain::addObservation observation 0 is reserved";
  }
  // convert the previous state to a CSV style key
  state_single key = stateSequenceToString(prevState);
  
  // do we have this key already
  bool have_key = true;
  try
  {
    model.at(key);
  }
  catch (const std::out_of_range& oor)
  {
    have_key = false;
  }
  // if we have this key, we are adding the new obs on that key
  if (have_key)
  {
    model[key].push_back(currentState);
  }
  else{// new key - create a new next state sequence
    state_sequence seq = {currentState};
    model[key] = seq;
  }
}

void MarkovChain::addObservationAllOrders(const state_sequence& prevState, state_single currentState)
{
  std::vector<state_sequence> allPrevs = breakStateIntoAllOrders(prevState);
  for (state_sequence& seq : allPrevs)
  {
    addObservation(seq, currentState);
  } 
}

std::vector<state_sequence>  MarkovChain::breakStateIntoAllOrders(state_sequence prevState)
{
  std::vector<state_sequence> allPrevs;
  // start is in the range 0-prevState.size() - 1
  auto end = prevState.size();
  allPrevs.push_back(prevState);
  for (int start = 1; start < end; start ++)
  {
    state_sequence::const_iterator first = prevState.begin() + start;
    state_sequence::const_iterator last = prevState.begin() + prevState.size();
    state_sequence prevStateShort(first, last);
    allPrevs.push_back(prevStateShort);
  }
  return allPrevs;
}


std::string MarkovChain::stateSequenceToString(const state_sequence& sequence)
{
  std::string str = std::to_string(sequence.size()); // write the order first
  str.append(",");
  // prefix it with the order
  for (const state_single& s : sequence)
  {
    str.append(s);
    str.append(",");   
  } 
  return str;
}
std::string MarkovChain::stateSequenceToString(const state_sequence& sequence, int maxOrder)
{
  if (maxOrder >= sequence.size()){ 
    // max order is higher pr == than the order we have
    // simply ignore it and return the highest order we can
    return stateSequenceToString(sequence);
  }
  else {// requested maxOrdder is lower than the available order in the incoming state seq.
    std::string str = std::to_string(maxOrder); // write the order first
    str.append(",");
    auto want_to_skip = sequence.size() - maxOrder;
    auto skipped = 0;
    // prefix it with the order
    for (const state_single& s : sequence)
    {
     if (skipped < want_to_skip) 
     {
        skipped ++;
        continue; 
     } 
      str.append(s);
      str.append(",");
    }
    return str;
  }
}


state_single MarkovChain::generateObservation(const state_sequence& prevState, int maxOrder)
{
  //std::cout << "MarkovChain::generateObservation model size " << model.size() <<std::endl;

  // check for empty model
  if (model.size() == 0)
  {
    //std::cout << "warning - requested obs from empty model " << std::endl;
    return "0";
  }
  // force the maxOrder allowed by this class
  if (maxOrder > this->maxOrder) maxOrder = this->maxOrder; 

  std::string key = stateSequenceToString(prevState);
  state_sequence poss_next_states;

  bool have_key = true;
  try
  {
    poss_next_states = model.at(key);
  }
  catch (const std::out_of_range& oor)
  {
    have_key = false;
  }
  if (have_key)
  {
    // get a random numner
    return pickRandomObservation(poss_next_states);
  }
  else {
    // no key - choose something at random from all next observed states
    int randInd = 0;
    if (model.size() > 1) randInd = rand() % model.size();
    int ind = 0;
    state_single state = "0"; // start on the default state
    // iterate the map until we teach our random index
    // have to do this as skips are not possible
    for (std::map<std::string,state_sequence>::iterator it=model.begin();it!=model.end(); ++it)
    {
      if (ind == randInd){
        poss_next_states = (it->second);
        state = pickRandomObservation(poss_next_states);
        break;// jump down to the return statement 
      }
      else {
        ind ++;
        continue;
      }
    }
    std::cout << "MarkovChain::generateObservation  " << state <<std::endl;

    return state;
  }
}

state_single MarkovChain::pickRandomObservation( state_sequence& seq)
{
  auto ind = 0;
  if (seq.size() > 1) ind = rand() % seq.size();  
  return seq.at(ind);
  //return "0";
}

std::string MarkovChain::toString()
{
  return "String representation of the model here";
}

void MarkovChain::fromString(std::string savedModel)
{

}

void MarkovChain::reset()
{
    model.clear();
}
