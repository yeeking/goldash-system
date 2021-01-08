/*
  ==============================================================================

    dinvernoSystem.h
    This file contains all the  a bridge between the JUCE layer, i.e. midi events etc.
    and the modelling layer, i.e. the markov chains
    Created: 30 Oct 2019 4:45:11pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MarkovManager.h"
#include "ChordDetector.h"
#include "LogginManager.h"
#include "MusicCircleClient.h"

#pragma once


/**
 * This is an abstract class that specifies an interface for
 * the DinvernoImproviser type of improviser
 * The various versions of these improvisers will implement this interface
 * The outer JUCE program can therefore interact with this interface.
 */
class DinvernoImproviser {
public:
/**
 * Constructor sets up sampleRate and starttime for convenience 
 */
   DinvernoImproviser(int sampleRate) : sampleRate(sampleRate), 
                                        startTimeSamples (Time::getMillisecondCounterHiRes() * 0.001 * sampleRate), 
                                        readyToLog{false} {}
    virtual ~DinvernoImproviser() {}
    virtual void tick() = 0;
    virtual void addMidiMessage(const MidiMessage& msg) = 0;
    virtual void reset() = 0;
    virtual MidiBuffer getPendingMidiMessages();
    void setLogginManager(LogginManager* loggin);
    LogginManager* loggin;
    bool isReadyToLog();
    //MusicCircleClient mcClient{"teresa", "mjlcdm07"};;
   
    
protected:
  double getElapsedTimeSamples();   
  double sampleRate;
  double startTimeSamples;
  MidiBuffer pendingMessages;
  const int MAX_PENDING_MESSAGES = 32;
private:
   bool readyToLog;
};

/**
 * Super simple improviser that takes in midi notes
 * and sends back those notes a little while later
 * Made this to test out the buffered midi note construction.
 */
class DinvernoMidiParrot : public DinvernoImproviser {
public:
   DinvernoMidiParrot(int sampleRate);
   ~DinvernoMidiParrot();
   virtual void tick() override;
   virtual void addMidiMessage(const MidiMessage& msg) override;
    virtual void reset() override;
private:
};

/**
 * Super simple improviser that takes in midi notes
 * and sends back those notes a little while later
 * Made this to test out the buffered midi note construction.
 */
class DinvernoRandomMidi : public DinvernoImproviser {
public:
   DinvernoRandomMidi(int sampleRate);
   ~DinvernoRandomMidi();
   virtual void tick() override;
   virtual void addMidiMessage(const MidiMessage& msg) override;
    virtual void reset() override;
    /** prepare a note seq starting at sent time, returns the length.*/
    virtual double prepareRandomNoteSequence(double startTime);

protected:
    juce::Random random{Time::currentTimeMillis()};
    double maxWaitBeteeenRiffs;
    double waitTimeSamples;
    double timeSinceLastPlayed;
    double lastTick;
    
};

class DinvernoRandomEnergy : public DinvernoRandomMidi {
  public:
    DinvernoRandomEnergy(int sampleRate);
    ~DinvernoRandomEnergy();
    virtual void tick() override;
    double prepareRandomNoteSequence(double startTime, double energy);

  private:
    double energy; 
    void addMidiMessage(const MidiMessage& msg) override;
};

class DinvernoMonoMarkov : public DinvernoImproviser {
public:
   DinvernoMonoMarkov(int sampleRate);
   ~DinvernoMonoMarkov();
   virtual void tick() override;
   virtual void addMidiMessage(const MidiMessage& msg) override;
    virtual void reset() override;

private:
  void addNoteOnToModel(int note, int velocity);
  void addNoteOffToModel(int note);
  double getNoteOnTimeSamples(int note);
  double lastTickSamples;
  double accumTimeDelta;
  double timeSinceLastNote;
  std::map<int,double> noteOnTimesSamples;
  MarkovManager pitchModel;
  MarkovManager lengthModel; 
};

class DinvernoPolyMarkov : public DinvernoImproviser {
public:
   DinvernoPolyMarkov(int sampleRate);
   ~DinvernoPolyMarkov();
   virtual void tick() override;
   virtual void addMidiMessage(const MidiMessage& msg) override;
    virtual void reset() override;

private:
/**add a vector of notes to the model. If it is a chord, there will be > 1 note*/
  void addNotesToModel(std::vector<int> notes);
  /** add note offs, used to build the note length model */
  void addNoteOffToModel(int note);
  /** helper function to convert a vector of notes into a state string */
  state_single notesToMarkovState(std::vector<int> notes);
  /** does the opposite of notesToMarkovState - converts a state back to a vector */
  std::vector<int> markovStateToNotes(state_single n_state);
  /** query the noteOnTimesSamples map with error checking */
  double getNoteOnTimeSamples(int note);

  double lastTickSamples;
  double accumTimeDelta;
  double timeBeforeNextNote;
  double lastNoteOnAtSample;
  std::map<int,double> noteOnTimesSamples;
  MarkovManager pitchModel; // pitches of notes
  MarkovManager lengthModel;  // length of notes
  MarkovManager velocityModel; // loudness of notes
  MarkovManager interOnsetIntervalModel; // time between note onts
  ChordDetector chordDetector;
};

/**
 * DinvernoCCMarkov : improviser that works with
 * midi note data and midi CC data. 
 * It actually contains a DinvernoPolyMarkov which handles the notes
 * and its own implementation for the midi CC processing
 */
class DinvernoCCMarkov : public DinvernoImproviser
{
public:
    DinvernoCCMarkov(int sampleRate);
    ~DinvernoCCMarkov();
    virtual void tick() override; 
    virtual void addMidiMessage(const MidiMessage& msg) override;
    virtual void reset() override;
    virtual MidiBuffer getPendingMidiMessages() override;

private:
    /** we use this to model the midi notes */
    DinvernoPolyMarkov polyMarkovDelegate;
    /** models the times between control change messages */
    MarkovManager interOnsetIntervalModel; 
    /** models the control change messages */
    MarkovManager controlChangeModel; 
    /** We use this to detect when there are multiple 
     * ccs coming in at roughly the same time
     */
    ChordDetector chordDetector;

    double lastTickSamples;
    double accumTimeDelta;
    double timeBeforeNextCCMsg;
    double lastCCAtSample;

    /** 
     * Counterpart to controlChangesToMarkovState
     * converts from a single state symbol
     * which is a string in the format ccnum:ccval-ccnum:ccval
     * the control data it represents, which is a vector of pairs of ints
     * representing cc numbers abd cc values
    */
    std::vector<std::pair<int,int>> markovStateToControlChanges(state_single state);
    /**
     * Counterpart to markovStateToControlChanges
     * converts from a set of cc ints and cc values
     * which is a vector of pairs of ints
     * into a single state symbol
     * which is a string in the format ccnum:ccval-ccnum:ccval
     */
    state_single controlChangesToMarkovState(std::vector<std::pair<int,int>> ccs);

    void addCCsToModel(std::vector<std::pair<int, int>> ccs);

};


class ImproviserUtils {
  public:
static std::vector<std::string> tokenise(std::string input, char sep)
{
  std::vector<std::string> vec;
  int end;
  auto start = input.find_first_not_of(sep, 0);
  std::string token;
  // find index of sep
  do{
    end = input.find_first_of(sep, start);
    //std::cout << start << " - " << end << std::endl;
    if (start == input.length() || start == end){// whitespace at the end
      break;
    }
    if (end >= 0){// we found it - use end for substr
      token = input.substr(start, end - start);
    }
    else { // we did not find it, use the remaining length of the string
      token = input.substr(start, input.length() - start);
    }
    //std::cout << "got token " << token << std::endl;
    vec.push_back(token);
    // did we find it?
    start = end + 1;
  }while (end > 0);  // yes? continue
  return vec;
}

};
