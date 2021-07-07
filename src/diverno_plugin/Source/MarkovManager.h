/*
  ==============================================================================

    MarkovManager.h
    Created: 30 Oct 2019 3:28:02pm
    Author:  matthew

  ==============================================================================
*/

#pragma once
#include "MarkovChain.h"

class MarkovManager {
  public:
      MarkovManager();
      ~MarkovManager();
      /** add an event to the chain. The manager manages previous events to ensure 
       * that variable orders are passed to the underlying markov model
      */
      void putEvent(state_single symbol);
      /**
      * retrieve an event from the underlying markov model. 
      */
      state_single getEvent();
    /**
     * wipe the underlying model and reset short term input and output memory. 
     */
        void reset();

      /**
     * Rotates the sent seq and pops the sent item on the end
     * [1,2,3], 4 -> [2,3,4]
     */
      void addStateToStateSequence(state_sequence& seq, state_single new_state);

    /**
   * Gets the current inputMemory state
   */
    state_sequence getInputMemory();
    
    /**
   * Gets the current outputMemory state
   */
    state_sequence getOutputMemory();
    
    
  private:
      state_sequence inputMemory;
      state_sequence outputMemory;
      MarkovChain chain;
};
