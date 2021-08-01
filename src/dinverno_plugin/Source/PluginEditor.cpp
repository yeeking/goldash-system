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
    
    // Initialise the GUI objects
    initialiseGUI();
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

void Dinverno_pluginAudioProcessorEditor::initialiseGUI(){
    // Reset Button
    resetButton.addListener(this);
    returnToPerformViewButton.addListener(this);
    posNegFeedbackCCSelector.addListener(this);
    leadFollowFeedbackCCSelector.addListener(this);
    feedbackValueRangeSelector.addListener(this);
    feedbackModeSelector.addListener(this);
    
    // Add Buttons to GUI
    addAndMakeVisible(resetButton);
    addAndMakeVisible(posNegFeedbackLabel);
    addAndMakeVisible(posNegFeedbackCCSelector);
    addAndMakeVisible(leadFollowFeedbackLabel);
    addAndMakeVisible(leadFollowFeedbackCCSelector);
    addAndMakeVisible(feedbackValueRangeLabel);
    addAndMakeVisible(feedbackValueRangeSelector);
    addAndMakeVisible(feedbackModeLabel);
    addAndMakeVisible(feedbackModeSelector);
    addAndMakeVisible(returnToPerformViewButton);
    
    resetButtonColours();
    resetButton.setButtonText("Reset Model");
    
    posNegFeedbackLabel.setText("Pos/Neg CC:", NotificationType::dontSendNotification);
    configureCCSelector(&posNegFeedbackCCSelector);
    posNegFeedbackCCSelector.setSelectedId(1);
    
    leadFollowFeedbackLabel.setText("Lead/Follow CC:", NotificationType::dontSendNotification);
    configureCCSelector(&leadFollowFeedbackCCSelector);
    leadFollowFeedbackCCSelector.setSelectedId(-1);
    
    feedbackValueRangeLabel.setText("Feedback Range:", NotificationType::dontSendNotification);
    configureFBRangeSelector(&feedbackValueRangeSelector);
    feedbackValueRangeSelector.setSelectedId(25);
    
    feedbackModeLabel.setText("Feedback Mode:", NotificationType::dontSendNotification);
    configureFBModeSelector(&feedbackModeSelector);
    feedbackModeSelector.setSelectedId(audioProcessor.getCurrentProgram()+feedbackModeSelectorOffset);
    
    returnToPerformViewButton.setButtonText("Return To Perform Mode");
}

void Dinverno_pluginAudioProcessorEditor::configureCCSelector(ComboBox* selector)
{
    // Populate combo box with all CC channels
    selector->addItem("PB", -1);
    for (int i = 1; i <= 127; i++){
        selector->addItem(String(i), i);
    }
}

void Dinverno_pluginAudioProcessorEditor::configureFBRangeSelector(ComboBox* selector)
{
    // Populate combo bo with percentages in range [1..50]
    for (int i = 1; i <= 50; i++){
        selector->addItem(String(i)+"%",i);
    }
}

void Dinverno_pluginAudioProcessorEditor::configureFBModeSelector(ComboBox* selector)
{
    selector->addItem("Practice",0+feedbackModeSelectorOffset);
    selector->addItem("Forward",1+feedbackModeSelectorOffset);
    selector->addItem("Reverse",2+feedbackModeSelectorOffset);
    selector->addItem("No Feedback",3+feedbackModeSelectorOffset);
}

void Dinverno_pluginAudioProcessorEditor::buttonClicked (Button* button)
{
    
    if (button == &resetButton){
        // Reset Button Clicked: Check if CTRL modifier
        auto modifiers = ModifierKeys::getCurrentModifiers();
        if (modifiers.isCtrlDown()){
            // CTRL Modifier: Open Config View
            configView = true;
            resized();
        }else{
            // No CTRL Modifier: Reset Model
            audioProcessor.resetCurrentImproviser();
        }
    }
    
    if (button == &returnToPerformViewButton){
        // Return to Perform View Button Clicked: Return to perform view
        configView = false;
        resized();
    }
}

void Dinverno_pluginAudioProcessorEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &posNegFeedbackCCSelector){
        audioProcessor.setPosNegFeedbackCC(posNegFeedbackCCSelector.getSelectedId());
    }
    if (comboBoxThatHasChanged == &leadFollowFeedbackCCSelector){
        audioProcessor.setLeadFollowFeedbackCC(leadFollowFeedbackCCSelector.getSelectedId());
    }
    if (comboBoxThatHasChanged == &feedbackValueRangeSelector){
        audioProcessor.setFeedbackBandwidth(feedbackValueRangeSelector.getSelectedId());
    }
    if (comboBoxThatHasChanged == &feedbackModeSelector){
        int newProgramNumber = feedbackModeSelector.getSelectedId() - feedbackModeSelectorOffset;
        if (audioProcessor.getCurrentProgram() != newProgramNumber){
            audioProcessor.setCurrentProgram(newProgramNumber);
        }
    }
}

void Dinverno_pluginAudioProcessorEditor::resetButtonColours()
{
    resetButton.setColour(TextButton::buttonColourId,
                          Colours::darkblue);
    
    returnToPerformViewButton.setColour(TextButton::buttonColourId,
                                        Colours::darkgreen);
}

//==============================================================================
void Dinverno_pluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void Dinverno_pluginAudioProcessorEditor::resized()
{
    if (!configView){
        // Perform View
        resetButton.setVisible(true);
        posNegFeedbackLabel.setVisible(false);
        posNegFeedbackCCSelector.setVisible(false);
        leadFollowFeedbackLabel.setVisible(false);
        leadFollowFeedbackCCSelector.setVisible(false);
        feedbackValueRangeLabel.setVisible(false);
        feedbackValueRangeSelector.setVisible(false);
        feedbackModeLabel.setVisible(false);
        feedbackModeSelector.setVisible(false);
        returnToPerformViewButton.setVisible(false);
        
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
        feedbackModeLabel.setVisible(true);
        feedbackModeSelector.setVisible(true);
        returnToPerformViewButton.setVisible(true);

        int selectorWidth = getWidth()*45/100;
        int selectorHeight = getHeight()/5;
        int labelWidth = getWidth()-selectorWidth;
        int labelHeight = selectorHeight;
        
        posNegFeedbackLabel.setBounds(0,0*selectorHeight, labelWidth, labelHeight);
        posNegFeedbackCCSelector.setBounds(getWidth()-selectorWidth,0*selectorHeight,selectorWidth,selectorHeight);
        
        leadFollowFeedbackLabel.setBounds(0,1*selectorHeight, labelWidth, labelHeight);
        leadFollowFeedbackCCSelector.setBounds(getWidth()-selectorWidth,1*selectorHeight,selectorWidth,selectorHeight);
        
        feedbackValueRangeLabel.setBounds(0,2*selectorHeight, labelWidth, labelHeight);
        feedbackValueRangeSelector.setBounds(getWidth()-selectorWidth,2*selectorHeight,selectorWidth,selectorHeight);
        
        feedbackModeLabel.setBounds(0,3*selectorHeight, labelWidth, labelHeight);
        feedbackModeSelector.setBounds(getWidth()-selectorWidth,3*selectorHeight,selectorWidth,selectorHeight);
        
        returnToPerformViewButton.setBounds(0,4*selectorHeight,getWidth(),selectorHeight);
    }
}






