#pragma once

#include <JuceHeader.h>
#include "VstControlComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public AudioAppComponent, //public juce::Component,
                       private juce::Timer,
                       Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void updateGraph();
    void timerCallback() override { updateGraph(); }
    
    // Listener interface for buttons
    void buttonClicked (Button* button) override;
    
private:
    //==============================================================================    
    // VST Audio/Midi Routing Graph
    std::unique_ptr<AudioProcessorGraph>            m_pMainGraph;
    
    // I/O Plugins
    std::unique_ptr<AudioPluginInstance>            m_ioProcOut;
    std::unique_ptr<AudioPluginInstance>            m_ioProcMidiIn;
    std::unique_ptr<AudioPluginInstance>            m_ioProcMidiOut;
    
    // I/O Plugin Nodes
    AudioProcessorGraph::Node::Ptr                  m_ioProcOutNode;
    AudioProcessorGraph::Node::Ptr                  m_ioProcMidiInNode;
    AudioProcessorGraph::Node::Ptr                  m_ioProcMidiOutNode;

    // Dinverno System Plugin, Node and Editor
    std::unique_ptr<AudioPluginInstance>            m_dinvernoSystemPluginInstance;
    AudioProcessorGraph::Node::Ptr                  m_dinvernoSystemPluginInstanceNode;
    AudioProcessorEditor*                           m_dinvernoSystemPluginEditor;
    
    // Dinverno Audio/Midi Recorder Plugin, Node and Editor
    std::unique_ptr<AudioPluginInstance>            m_dinvernoRecorderPluginInstance;
    AudioProcessorGraph::Node::Ptr                  m_dinvernoRecorderPluginInstanceNode;
    AudioProcessorEditor*                           m_dinvernoRecorderPluginEditor;
    
    // Helm Synth for Machine Audio Plugin and Node
    std::unique_ptr<AudioPluginInstance>            m_helmMachinePresetPluginInstance;
    AudioProcessorGraph::Node::Ptr                  m_helmMachinePresetPluginInstanceNode;
    //AudioProcessorEditor*                           m_helmMachinePresetPluginEditor;
    
    // Helm Synth for Human Audio Plugin and Node
    std::unique_ptr<AudioPluginInstance>            m_helmHumanPresetPluginInstance;
    AudioProcessorGraph::Node::Ptr                  m_helmHumanPresetPluginInstanceNode;
    //AudioProcessorEditor*                           m_helmHumanPresetPluginEditor;
    
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;
    
    VstControlComponent m_machineControlComponent;
    VstControlComponent m_humanControlComponent;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
