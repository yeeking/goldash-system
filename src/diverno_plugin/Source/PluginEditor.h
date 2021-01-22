/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MidiReceiver.h"
#include "MidiSetupComponent.h"
#include "dinvernoSystem.h"
#include "RecordWidget.h"
#include "MusicCircleClient.h"

//==============================================================================
struct CustomLookAndFeel    : public LookAndFeel_V4 {
    Font getTextButtonFont (TextButton&, int buttonHeight) override {
        return Font (32.0f, Font::bold);
    }
};

//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
class Diverno_pluginAudioProcessorEditor  : public AudioProcessorEditor,
                                                    Timer,
                                                    MidiReceiver,
                                                    Button::Listener,
                                                    RecordingReceiver,
                                                    LogginListener
{
public:
    Diverno_pluginAudioProcessorEditor (Diverno_pluginAudioProcessor&);
    ~Diverno_pluginAudioProcessorEditor() override;

    /** Timer callback*/
    void timerCallback() override;
    
    /** LogginListener callback */
    void musicCircleEvent(MusicCircleEvent event) override;
    
    //==============================================================================
    /* VST:
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    */
     
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // from the midiInputHandler
    void receiveMidi(const MidiMessage& message) override;
    
    // Listener interface for buttons
    void buttonClicked (Button* button) override;
    
    // record listener
    void recordingStarted() override;
    void recordingComplete(File audioFile) override;
    
    void sendMidi(MidiMessage& message);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Diverno_pluginAudioProcessor& audioProcessor;

    //==============================================================================
    // Your private member variables go here...
    void resetButtonColours();
    void sendAllNotesOff();
    MidiSetupComponent midiSetupComponent;
    
    //MusicCircleClient mcClient{"csys2", "test123"};
    LogginManager loggin;
    
    // GUI compoenents for login details
    //Label loginStatusLabel{};
    TextEditor mcEventMonitor  { "MC Event Monitor" };
    TextEditor usernameField{"username"};
    TextEditor passwordField{"password", '*'};
    TextButton loginButton{"login"};
    
    // markov models
    DinvernoImproviser* currentImproviser;
    DinvernoMidiParrot dinvernoParrot{44100};
    DinvernoRandomMidi dinvernoRandomMidi{44100};
    DinvernoRandomEnergy dinvernoRandomEnergy{44100};
    DinvernoCCMarkov dinvernoPolyMarkov{44100};
    
    //DinvernoPolyMarkov dinvernoPolyMarkov{44100};
    
    TextButton parrotButton;
    TextButton randomButton;
    TextButton randomEnergyButton;
    TextButton polyButton;
    TextButton resetButton;
    CustomLookAndFeel lookAndFeel;
    RecordWidget recordWidget;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Diverno_pluginAudioProcessorEditor)
};
