/*
  ==============================================================================
  This code is taken from the JUCE MIDIDemo example. Here is the license:

   This file is part of the JUCE examples.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiReceiver.h"

#pragma once

//==============================================================================
struct MidiDeviceListEntry : ReferenceCountedObject
{
    MidiDeviceListEntry (MidiDeviceInfo info) : deviceInfo (info) {}

    MidiDeviceInfo deviceInfo;
    std::unique_ptr<MidiInput> inDevice;
    std::unique_ptr<MidiOutput> outDevice;

    using Ptr = ReferenceCountedObjectPtr<MidiDeviceListEntry>;
};


//==============================================================================
class MidiSetupComponent  : public Component,
                  private Timer,
                  private MidiKeyboardStateListener,
                  private MidiInputCallback,
                  private AsyncUpdater
{
public:
    //==============================================================================
    MidiSetupComponent()
        : midiKeyboard       (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
          midiInputSelector  (new MidiDeviceListBox ("Midi Input Selector",  *this, true)),
          midiOutputSelector (new MidiDeviceListBox ("Midi Output Selector", *this, false))
    {
        addLabelAndSetStyle (midiInputLabel);
        addLabelAndSetStyle (midiOutputLabel);
        addLabelAndSetStyle (incomingMidiLabel);
        addLabelAndSetStyle (outgoingMidiLabel);

        midiKeyboard.setName ("MIDI Keyboard");
        addAndMakeVisible (midiKeyboard);

        midiMonitor.setMultiLine (true);
        midiMonitor.setReturnKeyStartsNewLine (false);
        midiMonitor.setReadOnly (true);
        midiMonitor.setScrollbarsShown (true);
        midiMonitor.setCaretVisible (false);
        midiMonitor.setPopupMenuEnabled (false);
        midiMonitor.setText ({});
        addAndMakeVisible (midiMonitor);

        if (! BluetoothMidiDevicePairingDialogue::isAvailable())
            pairButton.setEnabled (false);

        addAndMakeVisible (pairButton);
        pairButton.onClick = []
        {
            RuntimePermissions::request (RuntimePermissions::bluetoothMidi,
                                         [] (bool wasGranted)
                                         {
                                             if (wasGranted)
                                                 BluetoothMidiDevicePairingDialogue::open();
                                         });
        };
        keyboardState.addListener (this);

        addAndMakeVisible (midiInputSelector .get());
        addAndMakeVisible (midiOutputSelector.get());

        setSize (732, 520);

        startTimer (500);
    }

    ~MidiSetupComponent() override
    {
        stopTimer();
        midiInputs .clear();
        midiOutputs.clear();
        keyboardState.removeListener (this);

        midiInputSelector .reset();
        midiOutputSelector.reset();
    }

    //==============================================================================
    void timerCallback() override
    {
        updateDeviceList (true);
        updateDeviceList (false);
    }

    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, 0.25f)); // 0.25 so its not too loud
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs (m);
    }

    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber, 0.25f)); // 0.25 so its not too loud
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs (m);
    }

    void paint (Graphics&) override {}

    void resized() override
    {
        auto margin = 10;
        auto col = getWidth() / 2;
        auto row = getHeight() / 14;
        auto x = 0;
        auto y = 0;
        midiInputLabel.setBounds(0, 0, col, row);
    
        midiOutputLabel.setBounds (0, col, col, row);
        y = row;

        midiInputSelector->setBounds (0, y, col, row * 4);
        midiOutputSelector->setBounds (col, y, col, row * 4);
        y += row * 4;
   
        pairButton.setBounds (0, y, col*2, row);
        y += row;
        outgoingMidiLabel.setBounds (0, y, col*2, row);
        y += row;
        midiKeyboard.setBounds (0, y, col*2, row*2);
        y += row * 2;
        incomingMidiLabel.setBounds (0, y, col*2, row);
        y += row;
        midiMonitor.setBounds (0, y, col*2, row * 4);
        
    //     auto y = (getHeight() / 3) + ((2 * 24) + (3 * margin) + 64);
    //     midiMonitor.setBounds (margin, y,
    //                            getWidth() - (2 * margin), getHeight() - y - margin);
   

    //     midiInputLabel.setBounds (margin, margin,
    //                               (getWidth() / 2) - (2 * margin), 24);

    //     midiOutputLabel.setBounds ((getWidth() / 2) + margin, margin,
    //                                (getWidth() / 2) - (2 * margin), 24);

    //     midiInputSelector->setBounds (margin, (2 * margin) + 24,
    //                                   (getWidth() / 2) - (2 * margin),
    //                                   (getHeight() / 3) - ((4 * margin) + 24 + 24));

    //     midiOutputSelector->setBounds ((getWidth() / 2) + margin, (2 * margin) + 24,
    //                                    (getWidth() / 2) - (2 * margin),
    //                                    (getHeight() / 3) - ((4 * margin) + 24 + 24));

    //     pairButton.setBounds (margin, (getHeight() / 3) - (margin + 24),
    //                           getWidth() - (2 * margin), 24);

    //     outgoingMidiLabel.setBounds (margin, getHeight() / 3, getWidth() - (2 * margin), 24);
    //     midiKeyboard.setBounds (margin, (getHeight() / 3) + (24 + margin), getWidth() - (2 * margin), 64);

    //     incomingMidiLabel.setBounds (margin, (getHeight() / 3) + (24 + (2 * margin) + 64),
    //                                  getWidth() - (2 * margin), 24);

    //     auto y = (getHeight() / 3) + ((2 * 24) + (3 * margin) + 64);
    //     midiMonitor.setBounds (margin, y,
    //                            getWidth() - (2 * margin), getHeight() - y - margin);
    // 
    }

    void openDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice.get() == nullptr);
            midiInputs[index]->inDevice = MidiInput::openDevice (midiInputs[index]->deviceInfo.identifier, this);

            if (midiInputs[index]->inDevice.get() == nullptr)
            {
                DBG ("MidiSetupComponentmo::openDevice: open input device for index = " << index << " failed!");
                return;
            }

            midiInputs[index]->inDevice->start();
        }
        else
        {
            jassert (midiOutputs[index]->outDevice.get() == nullptr);
            midiOutputs[index]->outDevice = MidiOutput::openDevice (midiOutputs[index]->deviceInfo.identifier);

            if (midiOutputs[index]->outDevice.get() == nullptr)
            {
                DBG ("MidiSetupComponent::openDevice: open output device for index = " << index << " failed!");
            }
        }
    }

    void closeDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice.get() != nullptr);
            midiInputs[index]->inDevice->stop();
            midiInputs[index]->inDevice.reset();
        }
        else
        {
            jassert (midiOutputs[index]->outDevice.get() != nullptr);
            midiOutputs[index]->outDevice.reset();
        }
    }

    int getNumMidiInputs() const noexcept
    {
        return midiInputs.size();
    }

    int getNumMidiOutputs() const noexcept
    {
        return midiOutputs.size();
    }

    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }

private:
    //==============================================================================
    struct MidiDeviceListBox : public ListBox,
                               private ListBoxModel
    {
        MidiDeviceListBox (const String& name,
                           MidiSetupComponent& contentComponent,
                           bool isInputDeviceList)
            : ListBox (name, this),
              parent (contentComponent),
              isInput (isInputDeviceList)
        {
            setOutlineThickness (1);
            setMultipleSelectionEnabled (true);
            setClickingTogglesRowSelection (true);
        }

        //==============================================================================
        int getNumRows() override
        {
            return isInput ? parent.getNumMidiInputs()
                           : parent.getNumMidiOutputs();
        }

        void paintListBoxItem (int rowNumber, Graphics& g,
                               int width, int height, bool rowIsSelected) override
        {
            auto textColour = getLookAndFeel().findColour (ListBox::textColourId);

            if (rowIsSelected)
                g.fillAll (textColour.interpolatedWith (getLookAndFeel().findColour (ListBox::backgroundColourId), 0.5));


            g.setColour (textColour);
            g.setFont (height * 0.7f);

            if (isInput)
            {
                if (rowNumber < parent.getNumMidiInputs())
                    g.drawText (parent.getMidiDevice (rowNumber, true)->deviceInfo.name,
                                5, 0, width, height,
                                Justification::centredLeft, true);
            }
            else
            {
                if (rowNumber < parent.getNumMidiOutputs())
                    g.drawText (parent.getMidiDevice (rowNumber, false)->deviceInfo.name,
                                5, 0, width, height,
                                Justification::centredLeft, true);
            }
        }

        //==============================================================================
        void selectedRowsChanged (int) override
        {
            auto newSelectedItems = getSelectedRows();
            if (newSelectedItems != lastSelectedItems)
            {
                for (auto i = 0; i < lastSelectedItems.size(); ++i)
                {
                    if (! newSelectedItems.contains (lastSelectedItems[i]))
                        parent.closeDevice (isInput, lastSelectedItems[i]);
                }

                for (auto i = 0; i < newSelectedItems.size(); ++i)
                {
                    if (! lastSelectedItems.contains (newSelectedItems[i]))
                        parent.openDevice (isInput, newSelectedItems[i]);
                }

                lastSelectedItems = newSelectedItems;
            }
        }

        //==============================================================================
        void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
        {
            SparseSet<int> selectedRows;
            for (auto i = 0; i < midiDevices.size(); ++i)
                if (midiDevices[i]->inDevice.get() != nullptr || midiDevices[i]->outDevice.get() != nullptr)
                    selectedRows.addRange (Range<int> (i, i + 1));

            lastSelectedItems = selectedRows;
            updateContent();
            setSelectedRows (selectedRows, dontSendNotification);
        }

    private:
        //==============================================================================
        MidiSetupComponent& parent;
        bool isInput;
        SparseSet<int> lastSelectedItems;
    };
    
    public:

    void setMidiInputReceiver(MidiReceiver* midiInputReceiver)
    {
        this->midiInputReceiver = midiInputReceiver;
    }

    //==============================================================================
    void handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage& message) override
    {
        // This is called on the MIDI thread
        const ScopedLock sl (midiMonitorLock);
        incomingMessages.add (message);
        triggerAsyncUpdate();
        
    }

    void handleAsyncUpdate() override
    {
        // This is called on the message loop
        Array<MidiMessage> messages;

        {
            const ScopedLock sl (midiMonitorLock);
            messages.swapWith (incomingMessages);
        }

        String messageText;

        for (auto& m : messages)
        {
            midiInputReceiver->receiveMidi(m);
            // display it
//            messageText = m.getDescription() + "\n";

            messageText << m.getDescription() << "\n";
        }
        midiMonitor.setText(messageText);
        //midiMonitor.insertTextAtCaret (messageText);
    }
public:
    void sendToOutputs (const MidiMessage& msg)
    {
        for (auto midiOutput : midiOutputs)
            if (midiOutput->outDevice.get() != nullptr)
                midiOutput->outDevice->sendMessageNow (msg);
    }
private:
    //==============================================================================
    bool hasDeviceListChanged (const Array<MidiDeviceInfo>& availableDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                : midiOutputs;

        if (availableDevices.size() != midiDevices.size())
            return true;

        for (auto i = 0; i < availableDevices.size(); ++i)
            if (availableDevices[i] != midiDevices[i]->deviceInfo)
                return true;

        return false;
    }

    ReferenceCountedObjectPtr<MidiDeviceListEntry> findDevice (MidiDeviceInfo device, bool isInputDevice) const
    {
        const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                      : midiOutputs;

        for (auto& d : midiDevices)
            if (d->deviceInfo == device)
                return d;

        return nullptr;
    }

    void closeUnpluggedDevices (const Array<MidiDeviceInfo>& currentlyPluggedInDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                : midiOutputs;

        for (auto i = midiDevices.size(); --i >= 0;)
        {
            auto& d = *midiDevices[i];

            if (! currentlyPluggedInDevices.contains (d.deviceInfo))
            {
                if (isInputDevice ? d.inDevice .get() != nullptr
                                  : d.outDevice.get() != nullptr)
                    closeDevice (isInputDevice, i);

                midiDevices.remove (i);
            }
        }
    }

    void updateDeviceList (bool isInputDeviceList)
    {
        auto availableDevices = isInputDeviceList ? MidiInput::getAvailableDevices()
                                                  : MidiOutput::getAvailableDevices();

        if (hasDeviceListChanged (availableDevices, isInputDeviceList))
        {

            ReferenceCountedArray<MidiDeviceListEntry>& midiDevices
                = isInputDeviceList ? midiInputs : midiOutputs;

            closeUnpluggedDevices (availableDevices, isInputDeviceList);

            ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;

            // add all currently plugged-in devices to the device list
            for (auto& newDevice : availableDevices)
            {
                MidiDeviceListEntry::Ptr entry = findDevice (newDevice, isInputDeviceList);

                if (entry == nullptr)
                    entry = new MidiDeviceListEntry (newDevice);

                newDeviceList.add (entry);
            }

            // actually update the device list
            midiDevices = newDeviceList;

            // update the selection status of the combo-box
            if (auto* midiSelector = isInputDeviceList ? midiInputSelector.get() : midiOutputSelector.get())
                midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
        }
    }

    //==============================================================================
    void addLabelAndSetStyle (Label& label)
    {
        label.setFont (Font (15.00f, Font::plain));
        label.setJustificationType (Justification::centredLeft);
        label.setEditable (false, false, false);
        label.setColour (TextEditor::textColourId, Colours::black);
        label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

        addAndMakeVisible (label);
    }

    //==============================================================================
    Label midiInputLabel    { "Midi Input Label",  "MIDI Input:" };
    Label midiOutputLabel   { "Midi Output Label", "MIDI Output:" };
    Label incomingMidiLabel { "Incoming Midi Label", "Received MIDI messages:" };
    Label outgoingMidiLabel { "Outgoing Midi Label", "Play the keyboard to send MIDI messages..." };
    MidiKeyboardState keyboardState;
  //public:
    MidiKeyboardComponent midiKeyboard;
  private: 
  // simple interface back to the maincomponent
  // so we can pass midi messages
    MidiReceiver* midiInputReceiver;
    TextEditor midiMonitor  { "MIDI Monitor" };
    TextButton pairButton   { "MIDI Bluetooth devices..." };

    std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;
    ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;

    CriticalSection midiMonitorLock;
    Array<MidiMessage> incomingMessages;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiSetupComponent)
};

