/*
  ==============================================================================

    VstControlComponent.h
    Created: 5 Mar 2021 7:08:38pm
    Author:  Sam Trolland

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class VstControlComponent : public juce::Component
{
public:
    //==============================================================================
    VstControlComponent()
    {
        addLabelAndSetStyle(title);
        addLabelAndSetStyle(programNumber);
        addLabelAndSetStyle(programName);
        
        prevButton.setEnabled(true);
        nextButton.setEnabled(true);
        addAndMakeVisible(nextButton);
        addAndMakeVisible(prevButton);
        
    };
    
    ~VstControlComponent(){};
    
    void resized()
    {
        int cols = 8;
        int col = getWidth()/cols;
        int rows = 4;
        int row = getHeight()/rows;
        
        prevButton.setBounds(0,row,col,rows*row-2*row);
        nextButton.setBounds(cols*col - col,row,col,rows*row-2*row);
        
        title.setBounds(col,0,cols*col-2*col,row);
        programName.setBounds(col,row,cols*col-2*col,row);
        programNumber.setBounds(col,2*row,cols*col-2*col,row);
        
    }
    
    void setTitle(String newTitle)
    {
        title.setText(newTitle,NotificationType::dontSendNotification);
    }
    
    void setProgramName(String newProgramName)
    {
        programName.setText(newProgramName,NotificationType::dontSendNotification);
    }
    
    void setProgramNumber(int newProgramNumber)
    {
        programNumber.setText(String(newProgramNumber),NotificationType::dontSendNotification);
    }
    
    void addButtonListner(Button::Listener* listner)
    {
        prevButton.addListener(listner);
        nextButton.addListener(listner);
    }
    
    //==============================================================================
    TextButton nextButton {">"};
    TextButton prevButton {"<"};
    
    Label title         {"title","Control Title"};
    Label programNumber {"programNumber", "Program Number"};
    Label programName   {"programName", "Program Name"};
    
    private:
    //==============================================================================
    void addLabelAndSetStyle (Label& label)
    {
        label.setFont (Font (15.00f, Font::plain));
        label.setJustificationType (Justification::centred);
        label.setEditable (false, false, false);
        label.setColour (TextEditor::textColourId, Colours::black);
        label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));
        
        addAndMakeVisible (label);
    }
};
