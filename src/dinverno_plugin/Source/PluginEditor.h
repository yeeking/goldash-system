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
class Dinverno_pluginAudioProcessorEditor  : public AudioProcessorEditor,
                                                    Button::Listener,
                                                    ComboBox::Listener,
                                                    ChangeListener
{
public:
    Dinverno_pluginAudioProcessorEditor (Dinverno_pluginAudioProcessor&);
    ~Dinverno_pluginAudioProcessorEditor() override;
     
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // Listener interface for buttons
    void buttonClicked (Button* button) override;
    
    // Listner interface for combo box
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Dinverno_pluginAudioProcessor& audioProcessor;

    //==============================================================================
    // Your private member variables go here...
    CustomLookAndFeel lookAndFeel;
    void initialiseGUI();
    void resetButtonColours();
    
    // Perform View
    TextButton resetButton;
    
    // Configuration View
    bool configView = false;
    Label posNegFeedbackLabel;
    ComboBox posNegFeedbackCCSelector;
    Label leadFollowFeedbackLabel;
    ComboBox leadFollowFeedbackCCSelector;
    Label feedbackValueRangeLabel;
    ComboBox feedbackValueRangeSelector;
    Label feedbackModeLabel;
    ComboBox feedbackModeSelector;
    Label trainingModeLabel;
    ComboBox trainingModeCCSelector;
    Label resetModelLabel;
    ComboBox resetModelCCSelector;
    int feedbackModeSelectorOffset = 10; // Map feedbackModeSelectorID to programNumber (as you can't add a comboBox item with id "0")
    TextButton returnToPerformViewButton;
    void configureCCSelector(ComboBox* selector);
    void configureFBRangeSelector(ComboBox* selector);
    void configureFBModeSelector(ComboBox* selector);
    void changeListenerCallback(ChangeBroadcaster* source);

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dinverno_pluginAudioProcessorEditor)
};
