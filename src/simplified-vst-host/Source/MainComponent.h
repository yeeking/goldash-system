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
    /** Load the two sent presets into the human and ai synths
     does not work for Surge XT unfortunately */
    void loadFXPFiles(const juce::File humanFile, const juce::File aiFile);
    /** set the preset on the sent node and update the gui as appropriate*/
    void setPresetAndUpdateGUI(AudioProcessorGraph::Node::Ptr& node, unsigned int index, VstControlComponent& gui);
    /** configures GUI layout, listeners etc. */
    void setupGUI();
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
    AudioProcessorEditor*                           dinvernoSystemGUI;
    
    // Dinverno Audio/Midi Recorder Plugin, Node and Editor
    std::unique_ptr<AudioPluginInstance>            m_dinvernoRecorderPluginInstance;
    AudioProcessorGraph::Node::Ptr                  m_dinvernoRecorderPluginInstanceNode;
    AudioProcessorEditor*                           recorderPluginGUI;
    
    juce::String synthName = "/Dexed.vst3";
    //juce::String synthName = "/Surge XT.vst3";
    
    // Helm Synth for Machine Audio Plugin and Node
    std::unique_ptr<AudioPluginInstance>            aiPlugin;
    AudioProcessorGraph::Node::Ptr                  aiPluginNode;
    //AudioProcessorEditor*                           m_helmMachinePresetPluginEditor;

    // Helm Synth for Human Audio Plugin and Node
    std::unique_ptr<AudioPluginInstance>            humanPlugin;
    AudioProcessorGraph::Node::Ptr                  humanPluginNode;
    //AudioProcessorEditor*                           m_helmHumanPresetPluginEditor;
    
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer player;
    
    VstControlComponent aiSynthGUI;
    VstControlComponent humanSynthGUI;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};


// how to load an fxp file directly into a vst plugin
//https://forum.juce.com/t/managing-plugin-parameters-in-audioplugininstance/30013
// To use this, create a derived implementation of ExtensionsVisitor and pass it to AudioPluginInstance::getExtensions. The plugin instance will call visitVST3Client on your extensions instance, and you can then call setPreset on the provided VST3Client argument.
class VSTVisitor : public juce::ExtensionsVisitor {

    void visitVST3Client (const VST3Client& plug) override
    {
    File exePath = File::getSpecialLocation (File:: currentExecutableFile);
    File macOSPath = exePath.getParentDirectory();
    File contentsPath = macOSPath.getParentDirectory();
    File resourcesPath = contentsPath.getChildFile("Resources");
    juce::File f(resourcesPath.getFullPathName() + "/test.fxp");

    juce::MemoryBlock mb;
    mb = plug.getPreset();
    DBG("visitVST3Client plugin data size from getPreset " << mb.getSize());
    f.loadFileAsData(mb);
    DBG("visitVST3Client plugin data size loaded from disk " << mb.getSize());

    plug.setPreset(mb);
    mb = plug.getPreset();
    DBG("visitVST3Client plugin data size from getPreset after set " << mb.getSize());


    }
};
