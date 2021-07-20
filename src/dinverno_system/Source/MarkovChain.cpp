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

MarkovChain::MarkovChain() : maxOrder{1000}, orderOfLastMatch{0}
{
  srand((int)time(NULL));
}

MarkovChain::~MarkovChain()
{

}

void MarkovChain::addObservation(const state_sequence& prevState, state_single currentState)
{
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

std::vector<state_sequence>  MarkovChain::breakStateIntoAllOrders(const state_sequence& prevState)
{
  std::vector<state_sequence> allPrevs;
  // start is in the range 0-prevState.size() - 1
  long unsigned int end = prevState.size();
  allPrevs.push_back(prevState);
  for (long unsigned int start = 1; start < end; start ++)
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
    if (s != "0")// ignore blank states
    {
      str.append(s);
      str.append(",");  
    } 
    // else {
    //   std::cout << "stateSequenceToString blank state ignored " << str << std::endl;
    // }
  } 
  return str;
}
std::string MarkovChain::stateSequenceToString(const state_sequence& sequence, long unsigned int maxOrder)
{
  if (maxOrder >= sequence.size()){ 
    // max order is higher pr == than the order we have
    // simply ignore it and return the highest order we can
    return stateSequenceToString(sequence);
  }
  else {// requested maxOrdder is lower than the available order in the incoming state seq.
    std::string str = std::to_string(maxOrder); // write the order first
    str.append(",");
    long unsigned int want_to_skip = sequence.size() - maxOrder;
    long unsigned int skipped = 0;
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
  // check for empty model
  if (model.size() == 0)
  {
    //std::cout << "warning - requested obs from empty model " << std::endl;
    return "0";
  }
  // don't allow orders beyond our own maxOrder
  if (maxOrder > this->maxOrder) maxOrder = this->maxOrder;
  // attempt to find a key in the chain that matches the incoming prevState
  state_single key = stateSequenceToString(prevState, maxOrder);
  state_sequence poss_next_states{};
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
    state_single obs = pickRandomObservation(poss_next_states);
    // remember what we did
    this->orderOfLastMatch = maxOrder; 
    this->lastMatch = state_and_observation{key, obs};
    return obs; 
  }
  else {
    //std::cout << "MarkovChain::generateObservation no match for that key " << key << "  at order " << maxOrder << std::endl;
    if (maxOrder > 1) 
    {
      //std::cout << "no match, reducing order to " << maxOrder - 1 << std::endl;
      // recurse with lower max order
      return generateObservation(prevState, maxOrder-1);
    }
    else {
      // worst case - nothing at higher than zero order
      this->orderOfLastMatch = 0;
      state_single obs = zeroOrderSample();
      this->lastMatch = state_and_observation{"0", obs};
      return obs; 
    }
  }
}

state_single MarkovChain::zeroOrderSample()
{

  state_sequence poss_next_states{};
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
  return state;
}


state_single MarkovChain::pickRandomObservation(const state_sequence& seq)
{
  if (seq.size() == 0) // they key existed but there';s nothing there.
  {
    return "0";
  } 
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

int MarkovChain::getOrderOfLastMatch()
{
  return this->orderOfLastMatch;
}

state_and_observation MarkovChain::getLastMatch()
{
  return this->lastMatch;
}

void  MarkovChain::removeMapping(state_single state_key, state_single unwanted_option)
{
  if (model.size() ==0 ) return; 
  state_sequence current_options{};
  bool have_key = true;
  try
  {
    current_options = model.at(state_key);
  }
  catch (const std::out_of_range& oor)
  {
    have_key = false; // nothing to do as we don't even have the state_key 
  }
  if (have_key) // we have seen this state_key
  {
    // create a new set of possible 
    state_sequence updated_options{};
    for (const state_single& obs : current_options)
    {
      if (obs != unwanted_option)
      {
        updated_options.push_back(obs);
      }
    }
    this->model[state_key] = updated_options;
  }
}

void MarkovChain::amplifyMapping(state_single state_key, state_single wanted_option)
{
  if (model.size() ==0 ) return; 
  state_sequence options = getOptionsForSequenceKey(state_key);
  if (options.size() == 0) // nothing mapped to this key... easy! 
  {
    options.push_back(wanted_option);
    this->model[state_key] = options; 
    return; 
  }
  // how many of the wanted option are there, relative to the total?
  float wanted = 0;
  float othermappings = 0;
  for (const state_single& s : options) {
    if (s == wanted_option) wanted ++;
    else othermappings ++;
  }
  // basically match the number of othermappings
  // to make this mapping as likely as any other
  for (auto i=0;i<othermappings;i++) model[state_key].push_back(wanted_option);
}


state_sequence MarkovChain::getOptionsForSequenceKey(state_single seqAsKey)
{
  state_sequence options{};
  try
  {
    options = model.at(seqAsKey);
  }
  catch (const std::out_of_range& oor)
  {
    // that's ok...we caught it :) 
  }
  return options; 
}

