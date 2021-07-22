/*
  ==============================================================================

    DinvernoPolyMarkov.cpp
    Created: 4 Nov 2019 3:52:54pm
    Author:  matthew

  ==============================================================================
*/

#include "dinvernoSystem.h"

DinvernoPolyMarkov::DinvernoPolyMarkov(int sampleRate) : 
              chordDetector(ChordDetector{sampleRate}), 
              inLeadMode{true}, 
              DinvernoImproviser(sampleRate)
{
  lastTickSamples = 0;
  accumTimeDelta = 0;
  timeBeforeNextNote = 0;
  lastNoteOnAtSample = 0;
  
  gotoLeadMode();

}
DinvernoPolyMarkov::~DinvernoPolyMarkov()
{

}

void DinvernoPolyMarkov::reset()
{

    std::cout << "DinvernoPolyMarkov::reset" << std::endl;
    pendingMessages.clear();
    // send all notes off
    pendingMessages.addEvent(MidiMessage::allNotesOff(1), 0);
    // reset leaders and followers: 
    followPitchModel.reset(); // pitches of notes
    followLengthModel.reset();  // length of notes
    followVelocityModel.reset(); // loudness of notes
    followInterOnsetIntervalModel.reset(); // time between note onts

    leadPitchModel.reset(); // pitches of notes
    leadLengthModel.reset();  // length of notes
    leadVelocityModel.reset(); // loudness of notes
    leadInterOnsetIntervalModel.reset(); // time between note onts
    
    chordDetector.reset();

    if (isReadyToLog())
      loggin->logData("PolyMarkov", "Reset applied.");
}


// this will be called periodically
// this is where do our generation
// which results in midi notes being added
// to the pendingevents list
void DinvernoPolyMarkov::tick()
{
  // for now, just play a note every second
  // with pitch and length from the length model.
  double nowSamples = (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate);
  double tDelta = nowSamples - lastTickSamples;
  accumTimeDelta += tDelta;
  double timeSinceLastNoteSeconds = (nowSamples - startTimeSamples - lastNoteOnAtSample) / sampleRate;
  if (accumTimeDelta > timeBeforeNextNote &&  // time to play
      timeSinceLastNoteSeconds < 15)  // it is less than 15 secs since human played
  {
    //std::cout << "time since last note " << timeSinceLastNoteSeconds << " : "<< lastNoteOnAtSample << std::endl;
    
    // reset the wait time
    accumTimeDelta = 0;
    // same length for all notes
    int noteLen = std::stoi(lengthModel->getEvent());
    int waitLen = std::stoi(interOnsetIntervalModel->getEvent());
    if (waitLen == 0)
    {
      waitLen = (int) sampleRate; 
    }
    if (noteLen > 0){
      state_single event = pitchModel->getEvent();
      std::vector<int> notes = markovStateToNotes(event);
      for (int& note : notes){
        if (note > 0)
        {
          uint8 velocity = std::stoi(velocityModel->getEvent());
          MidiMessage noteOn = MidiMessage::noteOn(1, note, velocity);
          MidiMessage noteOff = MidiMessage::noteOff(1, note, velocity);
          auto sampleNumber =  getElapsedTimeSamples() + sampleRate; // 1 second late
          if (pendingMessages.getNumEvents() < MAX_PENDING_MESSAGES){
            pendingMessages.addEvent(noteOn, sampleNumber);
          // length should be dictated by the length 
            pendingMessages.addEvent(noteOff, sampleNumber + noteLen);
          }
          //std::cout << "note len " << noteLen << " wait len " << waitLen << std::endl;
          timeBeforeNextNote = waitLen; // we'll use this as a wait time          
        }
      }
    }
  }
  lastTickSamples = nowSamples;
}

void DinvernoPolyMarkov::addMidiMessage(const MidiMessage& message)
{

  if (message.isNoteOn()){
  if (!inLeadMode && random.nextDouble() > 0.95) reset();

    std::string mgsDesc = message.getDescription().toStdString();
    if (isReadyToLog())
      loggin->logData("PolyMarkov", "Adding midi message: " + mgsDesc);
    // compared to the mono markov, we check times of the notes
    // here not in addNotesToModel as the chord detector can 
    // delay the processing of notes
    double elapsedSamples  = (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate) - startTimeSamples;
    noteOnTimesSamples[message.getNoteNumber()] = elapsedSamples;
    // now tell the chord detector about the note
    chordDetector.notePlayed(message.getNoteNumber(), getElapsedTimeSamples());
    // see if the chord detector has anything for us
    std::vector<int> notes = chordDetector.getReadyNotes();
    if (notes.size() > 0){
      // the gap between this and the previous note was sufficient
      // to release notes into the model
      addNotesToModel(notes);
      // remember velocity
      velocityModel->putEvent(std::to_string(message.getVelocity()));
      // calculate and remember interonsetinterval
//      std::cout << "time now " << elapsedSamples << "last on at " << lastNoteOnAtSample << " ioi " << (elapsedSamples - lastNoteOnAtSample) <<  std::endl;; 
      double interOnsetInterval = elapsedSamples - lastNoteOnAtSample;
      // only add it if its not too long
      if (interOnsetInterval < sampleRate * 3) // 3 seconds or less
      {
        interOnsetIntervalModel->putEvent(std::to_string(interOnsetInterval));
      }
      
      lastNoteOnAtSample = elapsedSamples; 
    }
  }
  if (message.isNoteOff()){
    addNoteOffToModel(message.getNoteNumber());
    std::string mgsDesc = message.getDescription().toStdString();
    if (isReadyToLog())
      loggin->logData("PolyMarkov", "Adding midi message off the model: " + mgsDesc);
  }
  
  // Thresholds for top and bottom end of value ranges
  int feedbackBandwidthCC = 30;
  int feedbackBandwidthPB = feedbackBandwidthCC/127*16383;
    
  if (message.isController()){
      // CC Message Received: Check which controller
      if(message.getControllerNumber() == 1)
      {
        //Leading/Following
        if (127-feedbackBandwidthCC < message.getControllerValue() && !inLeadMode){
            // Signal to Lead
            feedback(FeedbackEventType::lead);
        }else if (message.getControllerValue() < feedbackBandwidthCC && inLeadMode){
            //Signal to Follow
            feedback(FeedbackEventType::follow);
        }
    }
  }
  
  if(message.isPitchWheel()){
    //PitchBend Message Received
    if (16383-feedbackBandwidthPB < message.getPitchWheelValue()){
      // Positive Feedback
      feedback(FeedbackEventType::positive);
    }else if (message.getPitchWheelValue() < feedbackBandwidthPB){
      // Negative Feedback
      feedback(FeedbackEventType::negative);
    }
  }
}

void DinvernoPolyMarkov::addNotesToModel(std::vector<int> notes)
{
  state_single n_state = notesToMarkovState(notes);
  // remember when this note started so we can measure length later
  pitchModel->putEvent(n_state);
}
state_single DinvernoPolyMarkov::notesToMarkovState(std::vector<int> notes)
{ 
  state_single mstate = "";
  for (int& note : notes){
    mstate += std::to_string(note) + "-";
  }
  return mstate;
}
std::vector<int> DinvernoPolyMarkov::markovStateToNotes(state_single n_state)
{
  std::vector<int> notes;
  if (n_state == "0") return notes;
  //std::cout << "DinvernoPolyMarkov::markovStateToNotes got state " << n_state << std::endl;
  for (std::string& note_s : ImproviserUtils::tokenise(n_state, '-'))
  {
    //std::cout << "DinvernoPolyMarkov::markovStateToNotes got substate " << note_s << std::endl;
    notes.push_back(std::stoi(note_s));
  }
  return notes;
}


void DinvernoPolyMarkov::addNoteOffToModel(int note)
{
    double noteStart = getNoteOnTimeSamples(note);
    int elapsedSamples  = (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate) - startTimeSamples;
    int noteLen = elapsedSamples - noteStart;
    lengthModel->putEvent(std::to_string(noteLen));
}

double DinvernoPolyMarkov::getNoteOnTimeSamples(int note)
{
  double time;
  try
  {
    time = noteOnTimesSamples.at(note);
  }
  catch (const std::out_of_range& oor)
  {
    time = getElapsedTimeSamples() - sampleRate;// set it to 1 second ago as we don't have a start time
  }
  return time; 
}


void DinvernoPolyMarkov::feedback(FeedbackEventType fbType)
{

    switch (fbType) 
    // this is considered to be bad engineering because it is 'control coupling'
    // where one module sends a control flag to another one
    // which dictates its execution path
    // what I should do is make the FeedbackListener interface
    // more complex but I don't want to do that 
    {
        case FeedbackEventType::negative:
            pitchModel->giveNegativeFeedback(); 
            lengthModel->giveNegativeFeedback();  
            velocityModel->giveNegativeFeedback(); 
            interOnsetIntervalModel->giveNegativeFeedback(); 

            break;
        case FeedbackEventType::positive:
            pitchModel->givePositiveFeedback();             
            leadLengthModel.givePositiveFeedback();
            velocityModel->givePositiveFeedback(); 
            interOnsetIntervalModel->givePositiveFeedback(); 
            break;
        case FeedbackEventType::follow:
            gotoFollowMode();
            break;
        case FeedbackEventType::lead:
            gotoLeadMode();
            break;  
    }
}

void DinvernoPolyMarkov::gotoFollowMode()
{
    pitchModel = &followPitchModel;
    lengthModel = &followLengthModel;
    velocityModel = &followVelocityModel;
    interOnsetIntervalModel = &followInterOnsetIntervalModel;
    inLeadMode = false;  
}

void DinvernoPolyMarkov::gotoLeadMode()
{
    pitchModel = &leadPitchModel;
    lengthModel = &leadLengthModel;
    velocityModel = &leadVelocityModel;
    interOnsetIntervalModel = &leadInterOnsetIntervalModel; 
    inLeadMode = true; 
}
