/*
  ==============================================================================

    MarkovChain.h
    Created: 25 Oct 2019 6:47:13am
    Author:  matthew

  ==============================================================================
*/
#include <string>
#include <map>
#include <vector>
#include <random>

#pragma once

typedef std::vector<std::string> state_sequence;
typedef std::string state_single;

class MarkovChain {
  public:
    MarkovChain();
    ~MarkovChain();
    /** 
     * addObservation
     * add a single observation to the chain
     * @param prevState - the state preceeding the observation
     * @param currentState - the state observed
     * 
    */
    void addObservation(state_sequence prevState, state_single currentState);
    /**
     *  addObservationAllOrders
     * Add all orders of the sent observation to the chain
     * It breaks prevState into multiple orders of observation, 1-prevState.length 
     * then calls this->addObservation on each one 
     * @param prevState - the state preceeding the observation
     * @param currentState - the state observed
     */
    void addObservationAllOrders(state_sequence prevState, state_single currentState);

  // should be private once testing is complete... 
  // note to self - how to enable testing of private methods? 
  /**
   * breakStateIntoAllOrders
   * Convers the sent state_sequence (actually a vector of length n)
   * Into n new vectors, each one the size 1-n respectively
   * and containing elements indexed [n to 1] down to [n]
   * as we assume that prevState[n] was the most recent state
   * e.g. [a,b,c] -> [a,b,c], [b,c], [c]
   * @param state_sequence: the incoming vector that needs to be broken down
   * @return a vector of several_statess a
   */
    std::vector<state_sequence> breakStateIntoAllOrders(state_sequence prevState);
  
  /**
   * stateSequenceToString 
   * Converts a state_sequence into a string that can be used as a key
   * in the model. E.g.
   * ["a", "b", "thepther", "126"] -> "4,a,b,theother,127" (note it is prefixed with the length)
   * @param state_sequence: a vector of strings 
   */

    std::string stateSequenceToString(state_sequence sequence);
  /**
   * stateSequenceToString 
   * Converts a state_sequence into a string that can be used as a key
   * in the model. E.g.
   * sequence=["a", "b", "thepther", "126"], maxOrder=2 -> "2,theother,127" (note it is prefixed with the length)
   * Note that it takes the items from the end of the array, assuming they are the most recent states
   * @param state_sequence: a vector of strings 
   * @param max_order:an int representing which limits how much of the sequence we use 
   */

    std::string stateSequenceToString(state_sequence sequence, int maxOrder);
    /**
     * generateObservation: generate a new observation from the chain
     * @param prevState - the lookup state used to query the model
     * @param maxOrder - how much of the previous state to consider. 
     * It will try to query at this ordder, but if nothing is there, it'll
     * reduce the order until the query returns something.
     * @return a state sampled from the model
     */
    state_single generateObservation(const state_sequence prevState, int maxOrder);
  /**
   * Picks a random observation from the sent sequence. 
   */
    state_single pickRandomObservation(state_sequence& seq);
     /**
     * toString: convert the current model into a string for saving etc.
     * @return a string that can be sent to 'fromString' to recreate the model later
     */
    std::string toString();
    /**
     * fromString: recreate the model from the sent string
     * @param savedModel: the model we want
     */
    void fromString(std::string savedModel);
    /** Yanik the chain, as it were. 
     */
    void reset();
    
private:
/**
 * Maps from string keys to list of possible next states
 * 
 */
    std::map<std::string,state_sequence> model;
    int maxOrder; 
};
