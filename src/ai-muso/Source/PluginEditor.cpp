/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AimusoAudioProcessorEditor::AimusoAudioProcessorEditor (AimusoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    setResizable(true, true);
    
    setupUI();
    DBG("PluginEditor::cons done.");
    
}
void AimusoAudioProcessorEditor::setupUI()
{
    // model load and save controls
    addAndMakeVisible(loadModelBtn);
    loadModelBtn.setButtonText("Load model");
    addAndMakeVisible(saveModelBtn);
    saveModelBtn.setButtonText("Save model");
    addAndMakeVisible(currentModelLabel);

    addAndMakeVisible(trainToggle);
    trainToggle.setButtonText("AI is learning");
    trainToggle.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::green);
    trainToggle.addListener(this);

   // addAndMakeVisible(trainModeLabel);

    // midi channel select controls
    addAndMakeVisible(midiInSelector);
    addAndMakeVisible(midiInLabel);
    midiInLabel.setText("MIDI IN: ", juce::NotificationType::dontSendNotification);
    midiInSelector.setRange(0, 16, 1);
    midiInSelector.addListener(this);
    
    addAndMakeVisible(midiOutSelector);
    addAndMakeVisible(midiOutLabel);
    midiOutLabel.setText("MIDI OUT: ", juce::NotificationType::dontSendNotification);
    midiOutSelector.setRange(1, 16, 1);
    midiOutSelector.addListener(this);
                            

    // quantise
    addAndMakeVisible(quantiseLabel);
    quantiseLabel.setText("QUANT", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(quantiseSelector);
    quantiseSelector.setRange(0, 250, 10);
    quantiseSelector.setTextValueSuffix("ms");
    quantiseSelector.addListener(this);
    quantiseSelector.setValue(50);// start with a 25ms quant.
    
    // group for mode buttons
    //addAndMakeVisible(modeBox);
    addAndMakeVisible(leadModeBtn);
    leadModeBtn.setButtonText("Algo lead");
    leadModeBtn.addListener(this);
    leadModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::green);
    // make sure the mode matches the highlighted button
    audioProcessor.leadMode(); 

    // addAndMakeVisible(interactModeBtn);
    // interactModeBtn.setButtonText("Algo interact");
    // interactModeBtn.addListener(this);

    addAndMakeVisible(followModeBtn);
    followModeBtn.setButtonText("Algo follow");
    followModeBtn.addListener(this);

    addAndMakeVisible(resetModelBtn);
    resetModelBtn.setButtonText("Reset model");
    resetModelBtn.addListener(this);
}

AimusoAudioProcessorEditor::~AimusoAudioProcessorEditor()
{
}

//==============================================================================
void AimusoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

}

void AimusoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int colCount = 4;
    int colWidth = getWidth() / colCount;
    int rowCount = 5;
    int rowHeight = getHeight() / rowCount;
    int xPos = 0;
    int yPos = 0;
    
    // model load and save controls
    // [load][save][current model]
    trainToggle.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    loadModelBtn.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    saveModelBtn.setBounds(xPos, yPos, colWidth, rowHeight);
    // midi channel select controls
    // [midi in 15 [-----]]
    xPos = 0;
    yPos += rowHeight;
    midiInLabel.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    midiInSelector.setBounds(xPos, yPos, colWidth*3, rowHeight);
    // [midi out 1 [----]]
    xPos = 0;
    yPos += rowHeight;
    midiOutLabel.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    midiOutSelector.setBounds(xPos, yPos, colWidth*3, rowHeight);
   

    // quantise
    xPos = 0;
    yPos += rowHeight;
    quantiseLabel.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    quantiseSelector.setBounds(xPos, yPos, colWidth*3, rowHeight);
       
    // group for mode buttons
    //modeBox
    xPos = 0;
    yPos += rowHeight;
    leadModeBtn.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    followModeBtn.setBounds(xPos, yPos, colWidth, rowHeight);
    xPos += colWidth;
    interactModeBtn.setBounds(xPos, yPos, colWidth, rowHeight);
    
    xPos += colWidth;
    resetModelBtn.setBounds(xPos, yPos, colWidth, rowHeight);
    
}


void AimusoAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &this->quantiseSelector)
        audioProcessor.setQuantisationMs(slider->getValue());
    if (slider == &this->midiInSelector)
        audioProcessor.setMidiInChannel(slider->getValue());
    if (slider == &this->midiOutSelector)
        audioProcessor.setMidiOutChannel(slider->getValue());
            
    
}


void AimusoAudioProcessorEditor::buttonClicked(Button* btn)
{
    //juce::Colour bg = getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId);
    //interactModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::darkgrey);
    

    if (btn == &this->followModeBtn){
        // reset mode buttons
        leadModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::darkgrey);   
        // highlight this button
        followModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::green);
        audioProcessor.followMode();
    }
    if (btn == &this->leadModeBtn){
        // reset mode buttons
        followModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::darkgrey);
        // highlight correct button
        leadModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::green);
        audioProcessor.leadMode(); 
    } 
    if (btn == &this->resetModelBtn){
        audioProcessor.resetModels();
        followModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::darkgrey);
        leadModeBtn.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::green);
        audioProcessor.leadMode(); 
    }
    if (btn == &this->trainToggle){
        if (audioProcessor.isTraining()){
            trainToggle.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::darkgrey);
            trainToggle.setButtonText("AI is not learning");
            audioProcessor.disableTraining();
        }
        else {
            trainToggle.setColour(juce::TextButton::ColourIds::buttonColourId, Colours::green);
            trainToggle.setButtonText("AI is learning");

            audioProcessor.enableTraining();
        }     
    }
}