#include "ChordDetector.h"
#include <iostream>
#include <string>
#include <vector>

void log(std::string test, bool result)
{
    std::cout << test << " : " << result << std::endl;
}

//1 
bool createChord()
{
    ChordDetector det(0); // start time = 0
    det.notePlayed(64, 10); // note played 10 samples later
    return true;
}
//2
bool singleNoteNowtToPlay()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    std::vector<int> notes = det.getReadyNotes();
    if (notes.size() == 0) return true;
    else return false;
}
//3
bool twoNotesFarApartPlayOne()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    det.notePlayed(48, 44100); // note played 1 second later
    
    std::vector<int> notes = det.getReadyNotes();
    if (notes.size() == 1) return true;
    else return false;
}
//4
bool twoNotesFarApartPlayFirst()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    det.notePlayed(48, 44100); // note played 1 second later
    
    std::vector<int> notes = det.getReadyNotes();
    if (notes[0] == 64) return true;
    else return false;
}
//5
bool threeNotesFarApartPlaySecond()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    det.notePlayed(48, 44100); // note played 1 second later
    det.notePlayed(96, 88200); // note played 2 second later
    
    std::vector<int> notes = det.getReadyNotes();
    if (notes[0] == 48) return true; // this is correct as it'll ignore the first one
    else return false;
}
//6
bool twoCloseNotesReturnNil()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    det.notePlayed(48, 1); // note played 1 sample later    
    std::vector<int> notes = det.getReadyNotes();
    if (notes.size() == 0) return true; // as it is still collecting the chord.
    else return false;
}
//7
bool twoCloseNotesOneFarReturnTwo()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    det.notePlayed(48, 1); // note played 1 sample later  
    det.notePlayed(96, 44100); // end the chord
      
    std::vector<int> notes = det.getReadyNotes();
    if (notes.size() == 2 && 
        notes[0] == 64 && 
        notes[1] == 48) return true; 
    else return false;
}
//8
bool twoCloseNotesTwoFarReturnThird()
{
    ChordDetector det(44100); // sample rate
    det.notePlayed(64, 0); // note played at 0
    det.notePlayed(48, 1); // note played 1 sample later  
    det.notePlayed(96, 44100); // end the chord
    det.notePlayed(102, 88200); // end the chord
      
    std::vector<int> notes = det.getReadyNotes();
    if (notes.size() == 1 && 
        notes[0] == 96) return true; 
    else return false;
}




int main()
{
    int total_tests, passed_tests;
    total_tests = 0;
    passed_tests = 0; 
    // run some tests
    //1  
    bool res = createChord();
    log("createChord", res);
    total_tests ++;
    if (res) passed_tests ++;
    //2
    res = singleNoteNowtToPlay();
    log("singleNoteNowtToPlay", res);
    total_tests ++;
    if (res) passed_tests ++;
    //3
    res = twoNotesFarApartPlayOne();
    log("twoNotesFarApartPlayOne", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 4
    res = twoNotesFarApartPlayFirst();
    log("twoNotesFarApartPlayFirst", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 5
    res = threeNotesFarApartPlaySecond();
    log("threeNotesFarApartPlaySecond", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 6
    res = twoCloseNotesReturnNil();
    log("twoCloseNotesReturnNil", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 7
     res = twoCloseNotesOneFarReturnTwo();
    log("twoCloseNotesOneFarReturnTwo", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 8
     res = twoCloseNotesTwoFarReturnThird();
    log("twoCloseNotesTwoFarReturnThird", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    

    std::cout << "Passed " << passed_tests << " of " << total_tests << std::endl;

}