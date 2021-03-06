/*
  ==============================================================================

    ChordDetector.cpp
    Created: 31 Oct 2019 6:03:02pm
    Author:  matthew

  ==============================================================================
*/

#include "ChordDetector.h"
#include <iostream>

ChordDetector::ChordDetector(int sampleRate) : sampleRate(sampleRate)
{

}

ChordDetector::~ChordDetector()
{

}

void ChordDetector::notePlayed(int note, double timeInSamples)
{
  if (timeInSamples - lastNoteTimeInSamples > (sampleRate / 8)) // 1/8 of a econd
  {
    // individual note
    // prepare the previously collected notes for returning 
    // might be no collected notes, that's fine
    notesForReturn.clear();
    for (int storedNote : storedNotes){
      notesForReturn.push_back(storedNote);
    }
    storedNotes.clear();
  }
  // store the note
  storedNotes.push_back(note);
  lastNoteTimeInSamples = timeInSamples;
}

void ChordDetector::ccPlayed(int number, int value, double timeInSamples)
{
  if (timeInSamples - lastNoteTimeInSamples > (sampleRate / 16)) // 1/16 of a second
  {
    // individual note
    // prepare the previously collected notes for returning 
    // might be no collected notes, that's fine
    ccsForReturn.clear();
    for (std::pair<int, int>& cc : storedCCs)
    {
      // only add old value if not same cc number as incoming
      // this means the new value overrides the stored
      // value.
      if (cc.first != number) ccsForReturn.push_back(cc); 
    }  
    storedCCs.clear();
  }
  storedCCs.push_back(std::pair<int, int>{number, value});
  lastNoteTimeInSamples = timeInSamples;
}



std::vector<int> ChordDetector::getReadyNotes()
{
  if (notesForReturn.size() > 0){// we hvae notes to return!
    std::vector<int> retVal{notesForReturn};
    //std::cout << "ChordDetector::getReadyNotes: notes " << retVal.size() << std::endl; 
    notesForReturn.clear(); // clear it as we've given them back now
    return retVal;
  }
  else{ // nothing ready to return yet - return empty vector
    std::vector<int> retVal{};
    return retVal;
  }
}

std::vector<std::pair<int,int>> ChordDetector::getReadyCCs()
{
  if (ccsForReturn.size() > 0){// we hvae notes to return!
    std::vector<std::pair<int,int>> retVal{ccsForReturn};
    //std::vector<int> retVal{notesForReturn};
    //std::cout << "ChordDetector::getReadyNotes: notes " << retVal.size() << std::endl; 
    ccsForReturn.clear(); // clear it as we've given them back now
    return retVal;
  }
  else{ // nothing ready to return yet - return empty vector
    std::vector<std::pair<int,int>> retVal{};
 //   std::vector<int> retVal{};
    return retVal;
  }
}


