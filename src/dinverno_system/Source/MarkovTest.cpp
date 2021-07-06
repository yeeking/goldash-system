
#include "MarkovChain.h"
#include "MarkovManager.h"
//#include "dinvernoSystem.h"
//#include "../JuceLibraryCode/JuceHeader.h"

#include <iostream>
#include <string>
#include <random>

/**
 * helper function to print result of a test
 */
void log(std::string test, bool result)
{
    std::cout << test << " : " << result << std::endl;
}


// 1
bool emptyChainReturnsNull()
{
    MarkovChain chain{};
    chain.reset();
    state_single res = chain.generateObservation(state_sequence{}, 1);
    if (res == "0") return true;
    else return false; 
}
// 2
bool addOneTokenReturnOneToken()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence prevState = {"a"};
    chain.addObservation(prevState, "b");
    std::string res = chain.generateObservation(prevState, 1);
    if (res == "b") return true;
    else return false; 
}    
// 3
bool addSameTwoObsReturnOneObs()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence prevState = {"a"};

    chain.addObservation(prevState, "b");
    chain.addObservation(prevState, "c");
    
    std::string res = chain.generateObservation(std::vector<std::string>{"a"}, 1);
    if (res == "b" || res == "c") return true;
    else return false; 
}
// 4
bool addSameTwoObsReturnDiffObs()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence prevState = {"a"};

    chain.addObservation(prevState, "b");
    chain.addObservation(prevState, "c");
    
    bool seenA = false;
    bool seenB = false;
    bool seenC = false;
    for (auto i=0;i<100;i++)
    {
        std::string res = chain.generateObservation(prevState, 1);
        if (res == "a") 
        {
            seenA = true; 
            // instant fail
            break;
        }
        if (res == "b") seenB = true;
        if (res == "c") seenC = true;       
    }
    if (!seenA && seenB && seenC) return true;
    else return false; 
}
// 5
bool returnsHighestOrder()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence prevState1 = {"a"};
    state_sequence prevState2 = {"a", "b"};
    chain.addObservation(prevState1, "c");
    chain.addObservation(prevState2, "d");
    std::string res = chain.generateObservation(prevState2, 2);
    if (res == "d" && chain.getOrderOfLastMatch() == 2) return true;
    else return false; 
}
// 6
bool respectsMaxOrderLow()
{
    MarkovChain chain{};
    chain.reset();
    
    state_sequence prevState1 = {"a"};
    state_sequence prevState2 = {"a", "b"};
    chain.addObservationAllOrders(prevState1, "c"); // a->c
    chain.addObservationAllOrders(prevState2, "d"); // a-b -->d so should do ab->d and b->d

    std::string res = chain.generateObservation(prevState2, 1); // query on a,b but limit to 'b'

    // max order is 1, so it should search on b not ab
    if (res == "d" && chain.getOrderOfLastMatch() == 1) return true;

    else return false; 
}

// 7
bool respectsMaxOrderHigh()
{
    MarkovChain chain{};
    chain.reset();

    state_sequence prevState1 = {"a"};
    state_sequence prevState2 = {"a", "b"};
    state_sequence prevState3 = {"a", "b", "c"};

    chain.addObservation(prevState1, "e");
    chain.addObservation(prevState2, "f");
    chain.addObservation(prevState3, "g");
    
    std::string res = chain.generateObservation(prevState3, 3);
    // max order is 3, so it should search for cba, not ba or a
//    if (res == "g") return true;
    if (res == "g" && chain.getOrderOfLastMatch() == 3) return true;

    else return false; 
}
// 8
bool addAllOrders()
{
  
    MarkovChain chain{};
    chain.reset();

    state_sequence prevState1 = {"c"};
    state_sequence prevState2 = {"b", "c"};
    state_sequence prevState3 = {"a", "b", "c"};
    // this should add
    // c->e
    // b,c -> e
    // a.b.c -> e
    chain.addObservationAllOrders(prevState3, "e");
    // get c-e
    std::string res1 = chain.generateObservation(prevState3, 3); // a,b,c 
    // get b,c-e
    std::string res2 = chain.generateObservation(prevState2, 2); // b,c
    // get a,b,c-e
    std::string res3 = chain.generateObservation(prevState1, 1); // c

    // max order is 3, so it should search for cba, not ba or a
    if (res1 == "e" && res2 == "e" && res3 == "e") return true;
    else return false; 
}

// 9
bool addAllOrdersWorksWithOrder1()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence prevState = {"a"};
    chain.addObservationAllOrders(prevState, "b");
    std::string res = chain.generateObservation(prevState, 1);
    if (res == "b") return true;
    else return false; 
}
// 9a
bool addAllOrdersWorksWithOrder2()
{
  MarkovChain chain{};
    chain.reset();

    state_sequence prevState1 = {"c"};
    state_sequence prevState2 = {"b", "c"};
    // this should add
    // c->e
    // b,c -> e
    // a.b.c -> e
    chain.addObservationAllOrders(prevState2, "e");
    // get c-e
    std::string res1 = chain.generateObservation(prevState2, 1);
    // get b,c-e
    std::string res2 = chain.generateObservation(prevState2, 2);
    // get a,b,c-e
    
    // max order is 3, so it should search for cba, not ba or a
    if (res1 == "e" && res2 == "e") return true;
    else return false; 
}


// 10
bool breakStateIntoAllOrdersThreeOrders()
{
  
    MarkovChain chain{};
    state_sequence prevState = {"a", "b", "c"};
    std::vector<state_sequence> allPrevStates = chain.breakStateIntoAllOrders(prevState);
    // Should have [a,b,c], [b,c], [c]
    // check length
    if (allPrevStates.size() == 3) return true;
    else return false; 
}

// 11
bool breakStateIntoAllOrdersThreeOrdersSize()
{
    MarkovChain chain{};
    state_sequence prevState = {"a", "b", "c"};
    std::vector<state_sequence> allPrevStates = chain.breakStateIntoAllOrders(prevState);
    // Should have [a,b,c], [b,c], [c]
    // check sub lengths
    if (allPrevStates.size() != 3) return false;// stops the core from dumping
    if (allPrevStates[0].size() == 3 && 
        allPrevStates[1].size() == 2 && 
        allPrevStates[2].size() == 1) return true;
    else return false; 
}

// 12
bool breakStateIntoAllOrdersThreeOrdersContents()
{
    MarkovChain chain{};
    state_sequence prevState = {"a", "b", "c"};
    std::vector<state_sequence> allPrevStates = chain.breakStateIntoAllOrders(prevState);
    // Should have [a,b,c], [b,c], [c]
    // check contents
    if (allPrevStates.size() != 3) return false;// stops the core from dumping
    bool con1, con2, con3;
    con1 = false;
    con2 = false;
    con3 = false;
    // item 1
    if (allPrevStates[2].size() == 1 && 
        allPrevStates[2][0] == "c"){
            con1 = true;
        }
    // item 2
    if (allPrevStates[1].size() == 2 && 
        allPrevStates[1][0] == "b" && 
        allPrevStates[1][1] == "c" ){
            con2 = true;
        }
    // item 3
    if (allPrevStates[0].size() == 3 && 
        allPrevStates[0][0] == "a" && 
        allPrevStates[0][1] == "b" && 
        allPrevStates[0][2] == "c"){
        con3 = true;
    }
    if(con1 && con2 && con3) return true;
    else return false; 
}
// 13
bool sequenceToStringOne()
{
  
    MarkovChain chain{};
    state_sequence seq = {"a"};
    std::string s = chain.stateSequenceToString(seq);
    if (s == "1,a,") return true;
    else return false; 
}
// 14
bool sequenceToStringThree()
{
  
    MarkovChain chain{};
    state_sequence seq = {"a", "128", "anotherone"};
    std::string s = chain.stateSequenceToString(seq);
    if (s == "3,a,128,anotherone,") return true;
    else return false; 
}

// 15
bool unkownKeyUseDistribution()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence seenState = {"a"};
    state_sequence unseenState = {"b"};
    
    chain.addObservation(seenState, "b");
    chain.addObservation(seenState, "c");
    // zero order...
    std::string res = chain.generateObservation(unseenState, 1);
    if (res == "b" || res == "c") return true;
    else return false; 
}

// 15a
bool unkownKeyUseDistributionMoreThanOne()
{
    MarkovChain chain{};
    chain.reset();
    state_sequence seenState1 = {"a"};
    state_sequence seenState2 = {"a", "c"};
    state_sequence seenState3 = {"d", "x", "y"};
    
    state_sequence unseenState = {"b"};
    
    chain.addObservation(seenState1, "b");
    chain.addObservation(seenState1, "q");
    chain.addObservation(seenState2, "z");
    chain.addObservation(seenState3, "p");

    bool seenB, seenQ, seenZ, seenP;
    seenB = false;
    seenQ = false;
    seenZ = false;
    seenP = false;
    // look for all the possible observations
    for (auto i=0;i<100;i++)
    {
        std::string res = chain.generateObservation(unseenState, 1);
        if (res == "b") seenB = true;
        if (res == "q") seenQ = true;
        if (res == "z") seenZ = true;
        if (res == "p") seenP = true;
        
    }
    // zero order...
    if (seenB && seenQ && seenZ && seenP) return true;
    else return false; 
}

// 16
bool sequenceToStringMaxLength()
{
    MarkovChain chain{};
    state_sequence seq = {"a", "128", "anotherone"};
    std::string s = chain.stateSequenceToString(seq, 2);
    if (s == "2,128,anotherone,") return true;
    else return false; 
}
// 17
bool sequenceToStringMaxLengthTooLong()
{
    MarkovChain chain{};
    state_sequence seq = {"a", "128", "anotherone"};
    std::string s = chain.stateSequenceToString(seq, 5);
    if (s == "3,a,128,anotherone,") return true;
    else return false; 
}
// 18
bool sequenceToStringMaxLengthTheSame()
{
    MarkovChain chain{};
    state_sequence seq = {"a", "128", "anotherone", "c"};
    std::string s = chain.stateSequenceToString(seq, 4);
    if (s == "4,a,128,anotherone,c,") return true;
    else return false; 
}
// 19
bool addStateToStateSequence()
{
    MarkovManager manager{};
    state_sequence seq = {"a", "b", "c", "d"};
    manager.addStateToStateSequence(seq, "e");
    if (
        seq[0] == "b" && 
        seq[1] == "c" && 
        seq[2] == "d" && 
        seq[3] == "e" 
    ){return true;}
    else return false; 
}

// 20
bool getOutputNoData()
{
    MarkovManager manager{};
    state_single bad = manager.getEvent();
    if (bad == "0") return true;
    return false; 
}


// 22
bool variableOrderGenerate()
{
    MarkovManager manager{};
    state_sequence seq = {"a", "b", "c", "d"};
    int highestOrder = 0;
    // feed them in
    for (state_single& s : seq){
        manager.putEvent(s);
    }
    for (auto i=0;i<100;i++)
    {
        manager.getEvent();
        //std::cout << manager.getOrderOfLastEvent() << std::endl;
        if (manager.getOrderOfLastEvent() > highestOrder) highestOrder = manager.getOrderOfLastEvent();
    }
    if (highestOrder > 0) return true;
     return false; 
}

bool zeroOrderSample()
{
    MarkovChain chain{};
    state_single s = chain.zeroOrderSample(); // pick an observation at random from the chain
    return true; 
}

void runMarkovTests()
{
    int total_tests, passed_tests;
    total_tests = 0;
    passed_tests = 0; 
    bool res = false; 
    
    res = emptyChainReturnsNull();
    log("emptyChainReturnsNull", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    res = addOneTokenReturnOneToken();
    log("addOneTokenReturnOneToken", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    res = addSameTwoObsReturnOneObs();
    log("addSameTwoObsReturnOneObs", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    res = addSameTwoObsReturnDiffObs();
    log("addSameTwoObsReturnDiffObs", res);
    total_tests ++;
    if (res) passed_tests ++;

    res = returnsHighestOrder();
    log("returnsHighestOrder", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    res = respectsMaxOrderLow();
    log("respectsMaxOrderLow", res);
    total_tests ++;
    if (res) passed_tests ++;

    res = respectsMaxOrderHigh();
    log("respectsMaxOrderHigh", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    res = addAllOrders();
    log("addAllOrders", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    res = addAllOrdersWorksWithOrder1();
    log("addAllOrdersWorksWithOrder1", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 9a
    res = addAllOrdersWorksWithOrder2();
    log("addAllOrdersWorksWithOrder2", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    // 10
    res = breakStateIntoAllOrdersThreeOrders();
    log("breakStateIntoAllOrdersThreeOrders", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 11
    res = breakStateIntoAllOrdersThreeOrdersSize();
    log("breakStateIntoAllOrdersThreeOrdersSize", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 12
    res = breakStateIntoAllOrdersThreeOrdersContents();
    log("breakStateIntoAllOrdersThreeOrdersContents", res);
    total_tests ++;
    if (res) passed_tests ++;

    //13
    res = sequenceToStringOne();
    log("sequenceToStringOne", res);
    total_tests ++;
    if (res) passed_tests ++;

    //14
    res = sequenceToStringThree();
    log("sequenceToStringThree", res);
    total_tests ++;
    if (res) passed_tests ++;

    // 15
    res = unkownKeyUseDistribution();
    log("unkownKeyUseDistribution", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 15a
    res = unkownKeyUseDistributionMoreThanOne();
    log("unkownKeyUseDistributionMoreThanOne", res);
    total_tests ++;
    if (res) passed_tests ++;
    
    
    // 16
    res = sequenceToStringMaxLength();
    log("sequenceToStringMaxLength", res);
    total_tests ++;
    if (res) passed_tests ++;

    // 17
    res = sequenceToStringMaxLengthTooLong();
    log("sequenceToStringMaxLengthTooLong", res);
    total_tests ++;
    if (res) passed_tests ++;
    //  18
    res = sequenceToStringMaxLengthTheSame();
    log("sequenceToStringMaxLengthTheSame", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 19
    res = addStateToStateSequence();
    log("addStateToStateSequence", res);
    total_tests ++;
    if (res) passed_tests ++;
    // 20
    res = getOutputNoData();
    log("getOutputNoData", res);
    total_tests ++;
    if (res) passed_tests ++;

    // 22
    res = variableOrderGenerate();
    log("variableOrderGenerate", res);
    total_tests ++;
    if (res) passed_tests ++;

    // 23
    res = zeroOrderSample();
    log("zeroOrderSample", res);
    total_tests ++;
    if (res) passed_tests ++;



    
    res = respectsMaxOrderLow();
    log("respectsMaxOrderLow", res);
    total_tests ++;
    if (res) passed_tests ++;


    std::cout << "Passed " << passed_tests << " of " << total_tests << std::endl;
  
}

int main(){
    runMarkovTests();
    return 0;
}
