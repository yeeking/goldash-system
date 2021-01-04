/*
  ==============================================================================

    DinvernoCCMarkov.cpp
    Created: 6 Jul 2020 4:37:33pm
    Author:  matthew

  ==============================================================================
*/

#include "dinvernoSystem.h"

DinvernoCCMarkov::DinvernoCCMarkov(int sampleRate) 
: polyMarkovDelegate{sampleRate}, 
  chordDetector(ChordDetector{sampleRate}),
  DinvernoImproviser{sampleRate}
{
  lastTickSamples = 0;
  accumTimeDelta = 0;
  timeBeforeNextCCMsg = 0;
  std::cout << "DinvernoCCMarkov::DinvernoCCMarkov construction complete" << std::endl;
}

DinvernoCCMarkov::~DinvernoCCMarkov()
{

}


/** clock ticks. This will generate events if necessary. */
 void DinvernoCCMarkov::tick()
 {
//  std::cout << "DinvernoCCMarkov::tick start " << std::endl;

// first off, pass the tick on to the note modeller   
   polyMarkovDelegate.tick();

//std::cout << "DinvernoCCMarkov::tick poly ticked, now for me " << std::endl;

  // time housekeeping - how long have we been waiting 
  double nowSamples = (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate);
  double tDelta = nowSamples - lastTickSamples;
  accumTimeDelta += tDelta;

//std::cout << "DinvernoCCMarkov::tick accumlated time "<< accumTimeDelta << std::endl;


  // time housekeeping - how long since they last did anything?
  double timeOfLastHumanInputSeconds = (nowSamples - startTimeSamples - lastCCAtSample) / sampleRate;
  
  // decision: is it time to generate some MIDI?
  if (accumTimeDelta > timeBeforeNextCCMsg &&  // time to play
      timeOfLastHumanInputSeconds < 15)  // it is less than 15 secs since human played
  {
//std::cout << "DinvernoCCMarkov::tick time to do someting  "<< timeBeforeNextCCMsg << std::endl;

    // if we got here, we need to generate MIDI
    // reset the accumulated time
    accumTimeDelta = 0;
    // query the IOI model for a wait time
    int waitLen = std::stoi(interOnsetIntervalModel.getEvent());
//std::cout << "DinvernoCCMarkov::tick got a wait len  "<< waitLen << std::endl;

    if (waitLen == 0) // don't allow zero wait times
    {
      waitLen = (int) sampleRate; 
    }
    timeBeforeNextCCMsg = waitLen;      

    // get the next state from the generative model
    // this might be a list of symbols
    state_single next_state = controlChangeModel.getEvent();
    //std::cout << "DinvernoCCMarkov::tick got a cc state from the markov   "<< next_state << std::endl;
    
    // convert states into cc number - cc value pairs
    std::vector<std::pair<int,int>> ccs = markovStateToControlChanges(next_state);

    // we have the ccs we need to play
    // to generate midi cc messgaes and store them

    auto whenToSendMessage =  getElapsedTimeSamples() + sampleRate; // 1 second late

    for (std::pair<int, int>& cc : ccs)
    {
      std::cout << "DinvernoCCMarkov::tick firing cc " << cc.first << ":" << cc.second << " options are "<< next_state << std::endl;
    
      MidiMessage ccMsg = MidiMessage::controllerEvent(1, cc.first, cc.second);
      if (pendingMessages.getNumEvents() < MAX_PENDING_MESSAGES){
          pendingMessages.addEvent(ccMsg, whenToSendMessage);
      }
    }    
  }
  lastTickSamples = nowSamples;
 }


 void DinvernoCCMarkov::addMidiMessage(const MidiMessage& msg)
{
   // pass it onto the poly 
   polyMarkovDelegate.addMidiMessage(msg);
   std::cout << "DinvernoCCMarkov :: addMidiMessage received midi " << std::endl;
   // now do our own processing of CC messages
   if (msg.isController())
   {
    std::cout << "DinvernoCCMarkov :: addMidiMessage cc " << msg.getControllerNumber() << ":" << msg.getControllerValue()<< std::endl;

    // manage the inter onset interval model
    double elapsedSamples  = (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate) - startTimeSamples;   
    double interOnsetInterval = elapsedSamples - lastCCAtSample;
    // only add it if its not too long
    if (interOnsetInterval < sampleRate * 3) // 3 seconds or less
    { 
      interOnsetIntervalModel.putEvent(std::to_string(interOnsetInterval));
    }     
  // manage the ccs
    std::vector<std::pair<int, int>> ccs;
    ccs.push_back(std::pair<int, int> {msg.getControllerNumber(), msg.getControllerValue()});
    addCCsToModel(ccs);
    lastCCAtSample = elapsedSamples;
  
   }// end if it is midi CC data
}



// chord detect version....

// /** MIDI incoming - this 
//  * will update the models if necessary. 
//  */
//  void DinvernoCCMarkov::addMidiMessage(const MidiMessage& msg)
//  {
//    // pass it onto the poly 
//    polyMarkovDelegate.addMidiMessage(msg);
//    std::cout << "DinvernoCCMarkov :: addMidiMessage received midi " << std::endl;
//    // now do our own processing of CC messages
//    if (msg.isController())
//    {
//     std::cout << "DinvernoCCMarkov :: addMidiMessage cc " << msg.getControllerNumber() << ":" << msg.getControllerValue()<< std::endl;
    
//     // manage the fact that it might be a 'cc chord' i.e.
//     // more than one cc going at the same time
//     // - tell the chord detector about the new CC
//     chordDetector.ccPlayed(msg.getControllerNumber(), msg.getControllerValue(), getElapsedTimeSamples());
//     // - see if the chord detector is ready 
//     // to yield some events (i.e. the window has passed)
//     std::vector<std::pair<int, int>> ccs = chordDetector.getReadyCCs();
//     if (ccs.size() > 0) // ready to yield, so add events to model
//     {
//       std::cout << "DinvernoCCMarkov :: addMidiMessage got data from chord detector " << ccs.size() << std::endl;

//      // manage the inter onset interval model
//       double elapsedSamples  = (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate) - startTimeSamples;   
//       double interOnsetInterval = elapsedSamples - lastCCAtSample;
//       // only add it if its not too long
//       if (interOnsetInterval < sampleRate * 3) // 3 seconds or less
//       { 
//         interOnsetIntervalModel.putEvent(std::to_string(interOnsetInterval));
//       }     
//     // manage the ccs
//       addCCsToModel(ccs);
//       lastCCAtSample = elapsedSamples;
//     }
//    }// end if it is midi CC data
//  }
 
void DinvernoCCMarkov::addCCsToModel(std::vector<std::pair<int, int>> ccs)
{
  // convert the pairs 
  state_single cc_state = controlChangesToMarkovState(ccs);
  std::cout << "DinvernoCCMarkov::addCCsToModel adding state to markov " << cc_state << std::endl;
  // remember when this note started so we can measure length later
  controlChangeModel.putEvent(cc_state);
}


 void DinvernoCCMarkov::reset()
 {
   std::cout << "DinvernoCCMarkov::reset " << std::endl;
   polyMarkovDelegate.reset();
    pendingMessages.clear();
    controlChangeModel.reset();
if (isReadyToLog())
    loggin->logData("DinvernoCCMarkov", "Reset applied.");
 }

/** 
     * Counterpart to controlChangesToMarkovState
     * converts from a single state symbol
     * which is a string in the format ccnum:ccval-ccnum:ccval
     * the control data it represents, which is a vector of pairs of ints
     * representing cc numbers abd cc values
    */
    
std::vector<std::pair<int,int>> DinvernoCCMarkov::markovStateToControlChanges(state_single state)
{
  
  std::vector<std::pair<int,int>> ccs{};
  if (state == "0") return ccs; // cope with empty model

  // split into number:value on '-'
  for (std::string& num_colon_val : ImproviserUtils::tokenise(state, '-'))
  {
    std::vector<std::string> num_val = ImproviserUtils::tokenise(num_colon_val, ':');
    ccs.push_back(std::pair<int, int> {std::stoi(num_val[0]), std::stoi(num_val[1])});
  }
  return ccs;
}

/**
     * Counterpart to markovStateToControlChanges
     * converts from a set of cc ints and cc values
     * which is a vector of pairs of ints
     * into a single state symbol
     * which is a string in the format ccnum:ccval-ccnum:ccval
     */

state_single DinvernoCCMarkov::controlChangesToMarkovState(std::vector<std::pair<int,int>> ccs)
{
  state_single state{""};
  for (std::pair<int,int>& cc: ccs)
  {
    state += std::to_string(cc.first) + ":" + std::to_string(cc.second) + "-";
  }
  return state; 
}
