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
    
    //mcClient.login();
    //std::string default_username{"csys2"};
    //std::string default_password{"test123"};
    
    // loggin = new LogginManager(default_username, default_password);
    //loggin.addEventListener(this);
    
    // settings stuff:
    
    // MIDI
    //midiSetupComponent.setMidiInputReceiver(this);
    //addAndMakeVisible(midiSetupComponent);
    
    // MC login etc.
    /*
    addAndMakeVisible(mcEventMonitor);
    mcEventMonitor.setMultiLine(true);
    mcEventMonitor.setText("MusicCircle events appear here");
    usernameField.setText(default_username);
    passwordField.setText(default_password);
    addAndMakeVisible(usernameField);
    addAndMakeVisible(passwordField);
    addAndMakeVisible(loginButton);
    loginButton.addListener(this);
    */
    
    // Record Widget
    //recordWidget.addRecordingReceiver(this);
    
    // add the login to the improvisers:
    /*
    dinvernoParrot.setLogginManager(&loggin);
    dinvernoRandomMidi.setLogginManager(&loggin);
    dinvernoRandomEnergy.setLogginManager(&loggin);
    dinvernoPolyMarkov.setLogginManager(&loggin);
    */
    //audioProcessor.setImprovisersLoginManager(&loggin);
     
    // add the listner to the improvisors:
    //parrotButton.addListener(this);
    //randomButton.addListener(this);
    //randomEnergyButton.addListener(this);
    //polyButton.addListener(this);
    resetButton.addListener(this);
    
    // Add Buttons to GUI
    addAndMakeVisible(resetButton);
    //addAndMakeVisible(recordWidget);
    // disabling these for the Monash June trial
    //addAndMakeVisible(parrotButton);
    //addAndMakeVisible(randomButton);
    //addAndMakeVisible(randomEnergyButton);
    //addAndMakeVisible(polyButton);
    // end of disabled buttons
    
    resetButtonColours();
    //VST: setLookAndFeel(&lookAndFeel);
    
    //parrotButton.setButtonText("PARROT");
    //randomButton.setButtonText("RANDOM");
    //randomEnergyButton.setButtonText("RANDOM ENERGY");
    //polyButton.setButtonText("POLY MARKOV");
    resetButton.setButtonText("Reset Model");
    
    //parrotButton.setColour(TextButton::buttonColourId, Colours::red);
    
    //audioProcessor.setCurrentImproviser(&dinvernoPolyMarkov);  //currentImproviser = &dinvernoPolyMarkov;
    startTimer(1000);
    
    //loggin.loginToMC(default_username, default_password);
    
    // OSC
    addAndMakeVisible(oscAddressLabel);
    addAndMakeVisible(oscPortLabel);
    oscAddressLabel.setText("Server",dontSendNotification);
    oscPortLabel.setText("Port",dontSendNotification);
    addAndMakeVisible(oscAddressField);
    addAndMakeVisible(oscPortField);
    oscAddressField.setText("localhost");
    oscPortField.setText("9001");
    oscButton.addListener(this);
    addAndMakeVisible(oscButton);
    oscButton.setButtonText("OSC");
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

bool Dinverno_pluginAudioProcessorEditor::validateOSCAddress(String oscAddressStr)
{
    // Validate Server Address
    bool validAddress = false;
    
    // Split Address into '.' seperated components
    StringArray oscAddressSplit = StringArray();
    oscAddressSplit.addTokens(oscAddressStr, ".", "");
    
    // Check 4 part address is entereed
    if (oscAddressSplit.size() == 4){
        validAddress = true;
        // Check each part is a number
        for (int i = 0; i < 4; i++){
            int addressComponent = oscAddressSplit[i].getIntValue();
            if (!(0 <= addressComponent && addressComponent <= 255)){
                validAddress = false;
            }
        }
    }
    
    return validAddress;
}

bool Dinverno_pluginAudioProcessorEditor::validateOSCPort(String oscPortStr)
{
    bool validPort = false;
 
    // Check if OSC Port is Valid
    int oscPort = oscPortStr.getIntValue();
    if (oscPort >= 0 && oscPort < 65536){
        validPort = true;
    }
    
    return validPort;
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
        audioProcessor.resetCurrentImproviser();                     //currentImproviser->reset();
        //sendAllNotesOff();
        
    }
    
    if (button == &oscButton)
    {
        if (!oscConnected){
            // Validate OSC Server Address
            String oscAddressStr = oscAddressField.getText();
            if (oscAddressStr == "localhost")   oscAddressStr = "127.0.0.1";    // Handle "localhost"
            bool validAddress = validateOSCAddress(oscAddressStr);
            
            // Validate OSC Server Port
            String oscPortStr = oscPortField.getText();
            bool validPort = validateOSCPort(oscPortStr);
            
            // Check Inputs are Valid
            if (validPort && validAddress){
                // Update Input Fields
                oscAddressField.setText(oscAddressStr);
                oscPortField.setText(oscPortStr);
                oscAddressField.applyColourToAllText(Colours::white, true);
                oscPortField.applyColourToAllText(Colours::white, true);
                
                // Attempt to Connect to OSC Server
                if (osc.connect (oscAddressStr, oscPortStr.getIntValue() ))
                {
                    // OSC Connected successfully
                    oscConnected = true;
                    oscButton.setColour(TextButton::buttonColourId,
                                        Colours::green);
                }else{
                    // Handle connect error
                    //log ("Error: could not connect to UDP port 9001.");
                    oscConnected = false;
                    oscButton.setColour(TextButton::buttonColourId,
                                        Colours::grey);
                }
            }else{
                // Invalid OSC Address or Port
                if (!validAddress) oscAddressField.applyColourToAllText(Colours::red, true);
                if (!validPort) oscPortField.applyColourToAllText(Colours::red, true);
            }
        }else{
            // Attempt to Disconnect from OSC Server
            if (osc.disconnect()){
                // OSC Disconnected successfully
                oscConnected = false;
                oscButton.setColour(TextButton::buttonColourId,
                                    Colours::grey);
            }else{
                // Handle disconnect error
                oscConnected = false;
                oscButton.setColour(TextButton::buttonColourId,
                                    Colours::grey);
            }
        }
        
        // Can't edit OSC Text Fields when connected (must disconnect first)
        oscAddressField.setReadOnly(oscConnected);
        oscPortField.setReadOnly(oscConnected);
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
    
    oscButton.setColour(TextButton::buttonColourId,
                        Colours::grey);
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
    int oscLabelBuffer = 3;
    int oscButtonHeight = 50;
    int oscButtonWidth = 50;
    resetButton.setBounds(0, 0, getWidth(),getHeight()-oscButtonHeight);
    
    oscAddressLabel.setBounds(0,1.0*oscButtonHeight,oscButtonWidth,0.5*oscButtonHeight);
    oscAddressField.setBounds(oscButtonWidth,1.0*oscButtonHeight+oscLabelBuffer,getWidth()-2*oscButtonWidth-oscLabelBuffer,0.5*oscButtonHeight-2*oscLabelBuffer);
    oscPortLabel.setBounds(0,1.5*oscButtonHeight,oscButtonWidth,0.5*oscButtonHeight);
    oscPortField.setBounds(oscButtonWidth,1.5*oscButtonHeight+oscLabelBuffer,getWidth()-2*oscButtonWidth-oscLabelBuffer,0.5*oscButtonHeight-2*oscLabelBuffer);
    oscButton.setBounds(getWidth() - oscButtonWidth,oscButtonHeight,oscButtonWidth,oscButtonHeight);
    
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
    
    // Check if connected to OSC Server
    if (oscConnected){
        // Send Markov Models via OSC
        
        // Get InputMemory and put into string object for sending
        state_sequence inputMemory = audioProcessor.getImproviserInputMemory();
        juce::String inputMemoryStr = juce::String();
        for (std::string noteStr : inputMemory){
            inputMemoryStr += String(noteStr);
        }
        
        // Get OutputMemory and put into string object for sending
        state_sequence outputMemory = audioProcessor.getImproviserOutputMemory();
        juce::String outputMemoryStr = juce::String();
        for (std::string noteStr : outputMemory){
            outputMemoryStr += String(noteStr);
        }
        
        // Create OSC Messages
        OSCMessage inputMsg = OSCMessage("/InputMemory", inputMemoryStr);
        OSCMessage outputMsg = OSCMessage("/OutputMemory", outputMemoryStr);
        
        // Create OSC Bundle from Input and Output Memory Messages
        OSCBundle oscBundle = OSCBundle();
        oscBundle.addElement(inputMsg);
        oscBundle.addElement(outputMsg);
        
        // Send OSC Bundle
        if(! osc.send (oscBundle))
        {
            // Handle OSC Send Error
            //showConnectionErrorMessage ("Error: could not send OSC message.");
        }
    }
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




