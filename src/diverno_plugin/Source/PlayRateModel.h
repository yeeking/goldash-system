/*
  ==============================================================================

    PlayRateModel.h
    Created: 31 Oct 2019 5:40:14pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

/**
 * Class used to model play rate. It counts number of notes played
 * in windows of time, then builds a markov model from tihs
 * It can then tell a note playing model if it is allowed to play notes
 * in a given window, based on how many its played already and how many 
 * notes the person we are modelling plays per window.
 */
class PlayRateModel {
 public:
 /** Set up a model for play rate, observing a window size of timePeriodSamples*/
   PlayRateModel(double timePeriodInSamples);
   ~PlayRateModel();
   /** Tell the model to tick, sending it the current time, which means it does not need to check current time itself. */
   void tick(double timeNowInSamples);
   /** Tell the model that we played a note. This will influence if it lets us play any more notes this window*/
   void modelPlayed();
   /** Ask the model if we are allowed to play, based on the notes played already this window */
   bool canModelPlay();
   /** Tell the model a note was played by whoever we are modelling. Used to build the notes per 10 second model*/
   void modeleePlayed();
};