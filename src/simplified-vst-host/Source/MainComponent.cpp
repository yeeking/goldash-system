#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(): m_pMainGraph (new AudioProcessorGraph())
{
    // Initialise Graph and Player
    m_pMainGraph->enableAllBuses();
    deviceManager.initialiseWithDefaultDevices(2, 2);
    deviceManager.addAudioCallback(&player);
    
    // Midi Input: Listen on all available inputs
    auto midiList = juce::MidiInput::getAvailableDevices();
    for(int i = 0; i < midiList.size(); i++){
        auto inputDevice  = midiList[i];    //juce::MidiInput::getDefaultDevice();
        deviceManager.setMidiInputDeviceEnabled(inputDevice.identifier, true);
        deviceManager.addMidiInputDeviceCallback(inputDevice.identifier, &player);
    }
    
    // Midi Outputs
    auto outputDevice = juce::MidiOutput::getDefaultDevice();
    deviceManager.setDefaultMidiOutputDevice(outputDevice.identifier);
    
    // Graph IO Processors
    m_ioProcOut         = std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    m_ioProcMidiIn      = std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    m_ioProcMidiOut     = std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);
    
    //AudioPluginInstance * instance = new AudioProcessorGraph::AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    
    //std::unique_ptr<AudioProcessor> ap = new AudioProcessorGraph::AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    //std::unique_ptr<AudioPluginInstance> instance = std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor> (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
    
    m_ioProcOutNode     = m_pMainGraph->addNode (std::move (m_ioProcOut) );
    m_ioProcMidiInNode  = m_pMainGraph->addNode (std::move (m_ioProcMidiIn) );
    m_ioProcMidiOutNode = m_pMainGraph->addNode (std::move (m_ioProcMidiOut) );
    m_ioProcOutNode->getProcessor()->enableAllBuses();
    
    // Add VST3 Plugins
    OwnedArray<PluginDescription> pluginDescriptions;
    KnownPluginList plist;
    AudioPluginFormatManager pluginFormatManager;
    //pluginFormatManager.addDefaultFormats();
    VST3PluginFormat* vst3 = new VST3PluginFormat();
    pluginFormatManager.addFormat(vst3);
    
    
    
    // Add Plugins from Resources folder to KnownPluginList
    //File curDir = File::getCurrentWorkingDirectory();
    //String dir = curDir.getFullPathName();
    File exePath = File::getSpecialLocation (File:: currentExecutableFile);
    File macOSPath = exePath.getParentDirectory();
    File contentsPath = macOSPath.getParentDirectory();
    File resourcesPath = contentsPath.getChildFile("Resources");
//    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/dinverno_plugin.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));
    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/ai-muso.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));
    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/DinvernoAudioMidiRecorder.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));
    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/Dexed.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));
    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/Dexed.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));


    jassert (pluginDescriptions.size() > 0);
    String msg ("Error Loading Plugin: ");
    
    // Load Plugins
    m_dinvernoSystemPluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[0], 44100.0, 512, msg);
    m_dinvernoRecorderPluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[1], 44100.0, 512, msg);
    m_helmHumanPresetPluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[2], 44100.0, 512, msg);
    m_helmMachinePresetPluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[3], 44100.0, 512, msg);

    m_helmHumanPresetPluginInstance->setCurrentProgram(103);
    int numPrograms = m_helmHumanPresetPluginInstance->getNumPrograms();
    int curProgram_human = m_helmHumanPresetPluginInstance->getCurrentProgram();
    String programName_human = m_helmHumanPresetPluginInstance->getProgramName(curProgram_human);
    
    m_helmMachinePresetPluginInstance->setCurrentProgram(108);
    int curProgram_machine = m_helmMachinePresetPluginInstance->getCurrentProgram();
    String programName_machine = m_helmMachinePresetPluginInstance->getProgramName(curProgram_machine);
    
    // Create Plugin Nodes
    m_dinvernoSystemPluginInstanceNode = m_pMainGraph->addNode (std::move (m_dinvernoSystemPluginInstance) );
    m_dinvernoSystemPluginInstanceNode->getProcessor()->enableAllBuses();
    
    m_dinvernoRecorderPluginInstanceNode = m_pMainGraph->addNode (std::move (m_dinvernoRecorderPluginInstance) );
    m_dinvernoRecorderPluginInstanceNode->getProcessor()->enableAllBuses();
    
    m_helmMachinePresetPluginInstanceNode = m_pMainGraph->addNode (std::move (m_helmMachinePresetPluginInstance) );
    m_helmMachinePresetPluginInstanceNode->getProcessor()->enableAllBuses();
    
    m_helmHumanPresetPluginInstanceNode = m_pMainGraph->addNode (std::move (m_helmHumanPresetPluginInstance) );
    m_helmHumanPresetPluginInstanceNode->getProcessor()->enableAllBuses();
    //addAndMakeVisible(&graphHolder);
    
    // Connect Plugin Nodes
    // Midi: Input -> helmHuman
    m_pMainGraph->addConnection({ {m_ioProcMidiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                  {m_helmHumanPresetPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
                                });
    
    // Midi: Input -> dinvernoSystem
    m_pMainGraph->addConnection({ {m_ioProcMidiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                  {m_dinvernoSystemPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
    });
    
    
    // Midi: Input -> dinvernoRecorder
    m_pMainGraph->addConnection({ {m_ioProcMidiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
    });
    
    // Midi: dinvernoSystem -> dinvernoRecorder
    m_pMainGraph->addConnection({ {m_dinvernoSystemPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
    });
    
    // Midi: dinvernoSystem -> helmMachine
    m_pMainGraph->addConnection({ {m_dinvernoSystemPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                  {m_helmMachinePresetPluginInstanceNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
    });
    
    // Audio: helmMachine -> dinvernoRecorder (x2)
    m_pMainGraph->addConnection({ {m_helmMachinePresetPluginInstanceNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 0}
    });
    
    m_pMainGraph->addConnection({ {m_helmMachinePresetPluginInstanceNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 1}
    });
    
    // Audio: helmHuman -> dinvernoRecorder (x2)
    m_pMainGraph->addConnection({ {m_helmHumanPresetPluginInstanceNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 0}
    });
    
    m_pMainGraph->addConnection({ {m_helmHumanPresetPluginInstanceNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 1}
    });
    
    // Audio: dinvernoRecorder -> Audio Output (x2)
    m_pMainGraph->addConnection({ {m_dinvernoRecorderPluginInstanceNode->nodeID, 0},
                                  {m_ioProcOutNode->nodeID, 0}
                                });
    
    m_pMainGraph->addConnection({ {m_dinvernoRecorderPluginInstanceNode->nodeID, 1},
                                  {m_ioProcOutNode->nodeID, 1}
                                });
    
    player.setProcessor (m_pMainGraph.get());
    
    // GUI Setup
    // Window Size
    int buf = 10;
    int compWidth = 200;
    int compHeight = 100;
    setSize (2*compWidth+3*buf, 2*compHeight+3*buf);
    //int buffer = getWidth()/80;
    
    // Open Plugin GUI Editor: Human
    m_dinvernoSystemPluginEditor = m_dinvernoSystemPluginInstanceNode->getProcessor()->createEditor();
    auto dinvernoSystemContainer = m_dinvernoSystemPluginEditor->getConstrainer();
    m_dinvernoSystemPluginEditor->setBounds(buf, compHeight+2*buf, compWidth,compHeight);      //dinvernoSystemContainer->getMinimumWidth(), dinvernoSystemContainer->getMinimumHeight());
    addAndMakeVisible (m_dinvernoSystemPluginEditor);
    
    // Open Plugin GUI Editor: Machine
    m_dinvernoRecorderPluginEditor = m_dinvernoRecorderPluginInstanceNode->getProcessor()->createEditor();
    auto dinvernoRecorderContainer = m_dinvernoRecorderPluginEditor->getConstrainer();
    m_dinvernoRecorderPluginEditor->setBounds(compWidth+2*buf, compHeight+2*buf, compWidth,compHeight); // dinvernoRecorderContainer->getMinimumWidth(), dinvernoRecorderContainer->getMinimumHeight());
    addAndMakeVisible (m_dinvernoRecorderPluginEditor);
    
    // VST Control Components: GUI Positioning
    m_humanControlComponent.setBounds(buf,buf,compWidth,compHeight);
    m_machineControlComponent.setBounds(compWidth+2*buf,buf,compWidth,compHeight);
    
    // VST Control Components: Human Setup
    m_humanControlComponent.setTitle("Human Player");
    m_humanControlComponent.setProgramName(programName_human);
    m_humanControlComponent.setProgramNumber(curProgram_human);
    m_humanControlComponent.addButtonListner(this);
    
    // VST Control Components: Machine setup
    m_machineControlComponent.setTitle("Machine Player");
    m_machineControlComponent.setProgramName(programName_machine);
    m_machineControlComponent.setProgramNumber(curProgram_machine);
    m_machineControlComponent.addButtonListner(this);
    
    addAndMakeVisible(m_humanControlComponent);
    addAndMakeVisible(m_machineControlComponent);

    startTimer (100);
}

MainComponent::~MainComponent()
{
    auto device = juce::MidiInput::getDefaultDevice();
    
    deviceManager.removeAudioCallback (&player);
    deviceManager.setMidiInputDeviceEnabled (device.identifier, false);
    deviceManager.removeMidiInputDeviceCallback (device.identifier, &player);
    
}
//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.
    
    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.
    
    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!
    
    // For more details, see the help for AudioProcessor::getNextAudioBlock()
    
    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    
    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    
    
}

// Button Listener
void MainComponent::buttonClicked (Button* button)
{

    int curProgram_human = m_helmHumanPresetPluginInstanceNode->getProcessor()->getCurrentProgram();
    //int curProgram_human = m_helmHumanPresetPluginInstance->getCurrentProgram();
    String programName_human = m_helmHumanPresetPluginInstanceNode->getProcessor()->getProgramName(curProgram_human);
    
    int curProgram_machine = m_helmMachinePresetPluginInstanceNode->getProcessor()->getCurrentProgram();
    //int curProgram_machine = m_helmMachinePresetPluginInstance->getCurrentProgram();
    String programName_machine = m_helmMachinePresetPluginInstanceNode->getProcessor()->getProgramName(curProgram_machine);
    
    if (button == &m_humanControlComponent.prevButton){
        // Human Prev Preset
        int nextProgram_human = curProgram_human - 1;
        m_helmHumanPresetPluginInstanceNode->getProcessor()->setCurrentProgram(nextProgram_human);
    }else if (button == &m_humanControlComponent.nextButton){
        // Human Next Preset
        int nextProgram_human = curProgram_human + 1;
        m_helmHumanPresetPluginInstanceNode->getProcessor()->setCurrentProgram(nextProgram_human);
    }else if (button == &m_machineControlComponent.prevButton){
        // Machine Next Preset
        int nextProgram_machine = curProgram_machine - 1;
        m_helmMachinePresetPluginInstanceNode->getProcessor()->setCurrentProgram(nextProgram_machine);
    }else if (button == &m_machineControlComponent.nextButton){
         // Machine Next Preset
        int nextProgram_machine = curProgram_machine + 1;
        m_helmMachinePresetPluginInstanceNode->getProcessor()->setCurrentProgram(nextProgram_machine);
    }
    
    // Update GUI
    curProgram_human = m_helmHumanPresetPluginInstanceNode->getProcessor()->getCurrentProgram();
    programName_human = m_helmHumanPresetPluginInstanceNode->getProcessor()->getProgramName(curProgram_human);
    m_humanControlComponent.setProgramName(programName_human);
    m_humanControlComponent.setProgramNumber(curProgram_human);
    
    // Update GUI
    curProgram_machine = m_helmMachinePresetPluginInstanceNode->getProcessor()->getCurrentProgram();
    programName_machine = m_helmMachinePresetPluginInstanceNode->getProcessor()->getProgramName(curProgram_machine);
    m_machineControlComponent.setProgramName(programName_machine);
    m_machineControlComponent.setProgramNumber(curProgram_machine);
}

void MainComponent::updateGraph()
{
    /*for (auto slot : slots)
    {
        if (slot != nullptr)
        {
            activeSlots.add (slot);
            
            slot->getProcessor()->setPlayConfigDetails (mainProcessor->getMainBusNumInputChannels(),
                                                        mainProcessor->getMainBusNumOutputChannels(),
                                                        mainProcessor->getSampleRate(),
                                                        mainProcessor->getBlockSize());
        }
    }*/
}
