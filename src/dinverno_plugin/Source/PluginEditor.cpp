/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Dinverno_pluginAudioProcessorEditor::Dinverno_pluginAudioProcessorEditor (Dinverno_pluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 100);
    
    // Reset Button
    resetButton.addListener(this);
    returnToPerformModeButton.addListener(this);
    posNegFeedbackCCSelector.addListener(this);
    leadFollowFeedbackCCSelector.addListener(this);
    feedbackValueRangeSelector.addListener(this);
    
    // Add Buttons to GUI
    addAndMakeVisible(resetButton);
    addAndMakeVisible(posNegFeedbackLabel);
    addAndMakeVisible(posNegFeedbackCCSelector);
    addAndMakeVisible(leadFollowFeedbackLabel);
    addAndMakeVisible(leadFollowFeedbackCCSelector);
    addAndMakeVisible(feedbackValueRangeLabel);
    addAndMakeVisible(feedbackValueRangeSelector);
    addAndMakeVisible(returnToPerformModeButton);
    
    resetButtonColours();
    resetButton.setButtonText("Reset Model");
    
    posNegFeedbackLabel.setText("Pos/Neg CC:", NotificationType::dontSendNotification);
    posNegFeedbackCCSelector.addItem("PitchBend", -1);
    posNegFeedbackCCSelector.addItem("1", 1);
    posNegFeedbackCCSelector.setSelectedId(1);
    
    leadFollowFeedbackLabel.setText("Lead/Follow CC:", NotificationType::dontSendNotification);
    leadFollowFeedbackCCSelector.addItem("PB", -1);
    leadFollowFeedbackCCSelector.addItem("1", 1);
    leadFollowFeedbackCCSelector.setSelectedId(-1);
    
    feedbackValueRangeLabel.setText("Feedback Range:", NotificationType::dontSendNotification);
    feedbackValueRangeSelector.addItem("1%",1);
    feedbackValueRangeSelector.addItem("25%",25);
    feedbackValueRangeSelector.addItem("50%",50);
    feedbackValueRangeSelector.setSelectedId(25);
    
    returnToPerformModeButton.setButtonText("Return To Perform Mode");
}

Dinverno_pluginAudioProcessorEditor::~Dinverno_pluginAudioProcessorEditor()
{
    // this is needed to properly release the lookandfeel object.
    // VST: setLookAndFeel (nullptr);
    
    // This shuts down the audio device and clears the audio source.
    // VST: shutdownAudio();
    //stopTimer();
    //audioProcessor->stopTimer();
}

/** receive an event from the LogginManager. This is a lightweight events API
 * to notify the user.
 */
void Dinverno_pluginAudioProcessorEditor::musicCircleEvent(MusicCircleEvent event)
{
    /* VST:
    // lock the message thread
    // as this func is called from another thread
    // and we get assertion errors otherwise
    const MessageManagerLock mmLock;
    juce::String msg = mcEventMonitor.getText();
    switch(event){
        case MusicCircleEvent::login_succeeded:
            msg  << "\nLogged in. user id " << loggin.getUserId();
            break;
        case MusicCircleEvent::login_failed:
            msg << "\nFailed to login with user " << usernameField.getText();
            break;
        case MusicCircleEvent::media_upload_succeeded:
            msg << "\nMedia upload succeeded. ";// + (usernameField.getText());
            break;
        case MusicCircleEvent::logout_succeeded:
            msg << "\nLogged out user " << usernameField.getText();
            break;
        case MusicCircleEvent::logout_failed:
            msg << "\nFailed to logout user id " << loggin.getUserId();
            break;
            
    }
    mcEventMonitor.setText(msg);
    //    mcEventMonitor.repaint();
     */
}

void Dinverno_pluginAudioProcessorEditor::buttonClicked (Button* button)
{
    // let's use a pointer technique to despatch the button click
    // to the correct function
    /*
    if (button == &loginButton)
    {
        std::cout << "Login button clicked " << std::endl;
        std::string username = usernameField.getText().toStdString();
        std::string password = passwordField.getText().toStdString();
        loggin.setUsername(username);
        loggin.setPassword(password);
        loggin.loginToMC(username, password);
    }
    
    if (button == &parrotButton)
    {
        resetButtonColours();
        
        sendAllNotesOff();
        audioProcessor.setCurrentImproviser(String("PARROT"));   //currentImproviser = &dinvernoParrot;
        parrotButton.setColour(TextButton::buttonColourId,
                               Colours::red);
    }
    if (button == &randomButton)
    {
        resetButtonColours();
        
        sendAllNotesOff();
        audioProcessor.setCurrentImproviser(String("RANDOM MIDI"));   //currentImproviser = &dinvernoRandomMidi;
        randomButton.setColour(TextButton::buttonColourId,
                               Colours::red);
    }
    if (button == &randomEnergyButton)
    {
        resetButtonColours();
        
        sendAllNotesOff();
        audioProcessor.setCurrentImproviser(String("RANDOM ENERGY"));   //currentImproviser = &dinvernoRandomEnergy;
        randomEnergyButton.setColour(TextButton::buttonColourId,
                                     Colours::red);
    }
    if (button == &polyButton)
    {
        resetButtonColours();
        
        sendAllNotesOff();
        audioProcessor.setCurrentImproviser(String("POLY"));   //currentImproviser = &dinvernoPolyMarkov;
        polyButton.setColour(TextButton::buttonColourId,
                             Colours::red);
    }
     */
    
    if (button == &resetButton)
    {
        auto modifiers = ModifierKeys::getCurrentModifiers();
        if (modifiers.isCtrlDown()){
            configView = true;
            resized();
        }else{
            audioProcessor.resetCurrentImproviser();                     //currentImproviser->reset();
            //sendAllNotesOff();
        }
    }
    
    if (button == &returnToPerformModeButton){
        configView = false;
        resized();
    }
}

void Dinverno_pluginAudioProcessorEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &posNegFeedbackCCSelector){
        int test = 1;
    }
    if (comboBoxThatHasChanged == &leadFollowFeedbackCCSelector){
    
    }
    if (comboBoxThatHasChanged == &feedbackValueRangeSelector){
    
    }
}

void Dinverno_pluginAudioProcessorEditor::sendAllNotesOff()
{
    //MidiMessage msg = MidiMessage::allNotesOff(1);
    //sendMidi(msg);
}

void Dinverno_pluginAudioProcessorEditor::resetButtonColours()
{
    // set default colours
    /*
    parrotButton.setColour(TextButton::buttonColourId,
                           Colours::lightsteelblue);
    randomButton.setColour(TextButton::buttonColourId,
                           Colours::lightsteelblue);
    randomEnergyButton.setColour(TextButton::buttonColourId,
                                 Colours::lightsteelblue);
    polyButton.setColour(TextButton::buttonColourId,
                         Colours::lightsteelblue);
     */
    resetButton.setColour(TextButton::buttonColourId,
                          Colours::darkblue);
    
    returnToPerformModeButton.setColour(TextButton::buttonColourId,
                                        Colours::darkgreen);

}

/* VST:
void Dinverno_pluginAudioProcessorEditor::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.
    
    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.
    
    // For more details, see the help for AudioProcessor::prepareToPlay()
}


void Dinverno_pluginAudioProcessorEditor::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    
    // For more details, see the help for AudioProcessor::getNextAudioBlock()
    
    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    bufferToFill.clearActiveBufferRegion();
}

void Dinverno_pluginAudioProcessorEditor::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    
    // For more details, see the help for AudioProcessor::releaseResources()
}
*/

//==============================================================================
void Dinverno_pluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void Dinverno_pluginAudioProcessorEditor::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    //int rowCount = 6;
    //int col = getWidth()/2;
    //int row = getHeight()/rowCount;
    
    //midiSetupComponent.setBounds(0, 0, getWidth()/2, row * 3);
    
    //mcEventMonitor.setBounds(0, row*3 , getWidth()/2, row);
    //int loginCol = getWidth() / 2 / 3;
    //usernameField.setBounds(0, row*4 + (row/2), loginCol, row/2);
    //passwordField.setBounds(loginCol, row*4 + (row/2), loginCol, row/2);
    //loginButton.setBounds(loginCol*2, row*4 + (row/2), loginCol, row/2);
    
    //parrotButton.setBounds(col, 0, col, row);
    //randomButton.setBounds(col, row, col, row);
    //randomEnergyButton.setBounds(col, row*2, col, row);
    //polyButton.setBounds(col, row*3, col, row);
    //resetButton.setBounds(col, row*4, col, row);
    
    //recordWidget.setBounds(0, row*5, getWidth(), row);
    if (!configView){
        // Perform View
        resetButton.setVisible(true);
        posNegFeedbackLabel.setVisible(false);
        posNegFeedbackCCSelector.setVisible(false);
        leadFollowFeedbackLabel.setVisible(false);
        leadFollowFeedbackCCSelector.setVisible(false);
        feedbackValueRangeLabel.setVisible(false);
        feedbackValueRangeSelector.setVisible(false);
        returnToPerformModeButton.setVisible(false);
        
        resetButton.setBounds(0, 0, getWidth(),getHeight());
    }else{
        // Config View
        resetButton.setVisible(false);
        posNegFeedbackLabel.setVisible(true);
        posNegFeedbackCCSelector.setVisible(true);
        leadFollowFeedbackLabel.setVisible(true);
        leadFollowFeedbackCCSelector.setVisible(true);
        feedbackValueRangeLabel.setVisible(true);
        feedbackValueRangeSelector.setVisible(true);
        returnToPerformModeButton.setVisible(true);

        int selectorWidth = getWidth()*4/10;
        int selectorHeight = getHeight()/4;
        int labelWidth = getWidth()-selectorWidth;
        int labelHeight = selectorHeight;
        
        posNegFeedbackLabel.setBounds(0,0*selectorHeight, labelWidth, labelHeight);
        posNegFeedbackCCSelector.setBounds(getWidth()-selectorWidth,0*selectorHeight,selectorWidth,selectorHeight);
        
        leadFollowFeedbackLabel.setBounds(0,1*selectorHeight, labelWidth, labelHeight);
        leadFollowFeedbackCCSelector.setBounds(getWidth()-selectorWidth,1*selectorHeight,selectorWidth,selectorHeight);
        
        feedbackValueRangeLabel.setBounds(0,2*selectorHeight, labelWidth, labelHeight);
        feedbackValueRangeSelector.setBounds(getWidth()-selectorWidth,2*selectorHeight,selectorWidth,selectorHeight);
        
        returnToPerformModeButton.setBounds(0,3*selectorHeight,getWidth(),selectorHeight);
    }
}

void Dinverno_pluginAudioProcessorEditor::receiveMidi(const MidiMessage& message)
{
    // parse it and add it to the model
    // if we are in the right mode!
    //std::cout << "MainCom: receive MIDI " << message.getDescription() << std::endl;
    //currentImproviser->addMidiMessage(message);
}

void Dinverno_pluginAudioProcessorEditor::sendMidi(MidiMessage& message)
{
    //std::cout << "MainCom: sendMidi " << message.getDescription() << std::endl;
    //message.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
    //midiSetupComponent.sendToOutputs (message);
}

void Dinverno_pluginAudioProcessorEditor::timerCallback()
{
    /*
    //audioProcessor.tickCurrentImproviser();
    
    //std::cout << "maincompo:: timer sending some midi " << std::endl;
    int sampleNumber;
    
    audioProcessor.currentImproviser->tick();
    
    MidiBuffer toSend = audioProcessor.currentImproviser->getPendingMidiMessages();
    if (toSend.getNumEvents() > 0){
        //std::cout << "timerCallback sending " << toSend.getNumEvents() << std::endl;
        MidiBuffer::Iterator iterator (toSend);
        MidiMessage message;
        while (iterator.getNextEvent (message, sampleNumber))
        {
            //sendMidi(message);
        }
    }
    */
}

void Dinverno_pluginAudioProcessorEditor::recordingStarted()
{
    //loggin.resetClockAndAnnotationQueue();
}


void Dinverno_pluginAudioProcessorEditor::recordingComplete(File audioFile)
{
    /*
    DBG("MainComponent::recordingComplete about to upload " << audioFile.getFullPathName());
    loggin.postMedia(audioFile.getFullPathName().toStdString(), [this](int result){
        std::cout << "MainComponent::recordingComplete postMedia callback result " << result << std::endl;
        //loggin.sendQueuedAnnotations();
    });
     */
}




