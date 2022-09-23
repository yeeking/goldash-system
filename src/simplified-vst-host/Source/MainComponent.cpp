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

    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/ai-muso.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));
    plist.scanAndAddFile(resourcesPath.getFullPathName()+"/DinvernoAudioMidiRecorder.vst3", true, pluginDescriptions, *pluginFormatManager.getFormat(0));


    plist.scanAndAddFile(resourcesPath.getFullPathName()+synthName, true, pluginDescriptions, *pluginFormatManager.getFormat(0));
    plist.scanAndAddFile(resourcesPath.getFullPathName()+synthName, true, pluginDescriptions, *pluginFormatManager.getFormat(0));

    
    jassert (pluginDescriptions.size() > 0);
    String msg ("Error Loading Plugin: ");
    
    // Load Plugins
    m_dinvernoSystemPluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[0], 44100.0, 512, msg);
    m_dinvernoRecorderPluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[1], 44100.0, 512, msg);
    humanPlugin = pluginFormatManager.createPluginInstance(*pluginDescriptions[2], 44100.0, 512, msg);
    aiPlugin = pluginFormatManager.createPluginInstance(*pluginDescriptions[3], 44100.0, 512, msg);
    
   

    
    // Create Plugin Nodes
    m_dinvernoSystemPluginInstanceNode = m_pMainGraph->addNode (std::move (m_dinvernoSystemPluginInstance) );
    m_dinvernoSystemPluginInstanceNode->getProcessor()->enableAllBuses();
    
    m_dinvernoRecorderPluginInstanceNode = m_pMainGraph->addNode (std::move (m_dinvernoRecorderPluginInstance) );
    m_dinvernoRecorderPluginInstanceNode->getProcessor()->enableAllBuses();
    
    aiPluginNode = m_pMainGraph->addNode (std::move (aiPlugin) );
    aiPluginNode->getProcessor()->enableAllBuses();
    
    humanPluginNode = m_pMainGraph->addNode (std::move (humanPlugin) );
    humanPluginNode->getProcessor()->enableAllBuses();
    //addAndMakeVisible(&graphHolder);
    
    // Connect Plugin Nodes
    // Midi: Input -> helmHuman
    m_pMainGraph->addConnection({ {m_ioProcMidiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex},
                                  {humanPluginNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
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
                                  {aiPluginNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}
    });
    
    // Audio: helmMachine -> dinvernoRecorder (x2)
    m_pMainGraph->addConnection({ {aiPluginNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 0}
    });
    
    m_pMainGraph->addConnection({ {aiPluginNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 1}
    });
    
    // Audio: helmHuman -> dinvernoRecorder (x2)
    m_pMainGraph->addConnection({ {humanPluginNode->nodeID, 0},
                                  {m_dinvernoRecorderPluginInstanceNode->nodeID, 0}
    });
    
    m_pMainGraph->addConnection({ {humanPluginNode->nodeID, 0},
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
    
    
    setupGUI();
    
    startTimer (100);
    
    // now we have done our work, sort out the programs
    setPresetAndUpdateGUI(humanPluginNode, 5, humanSynthGUI);
    setPresetAndUpdateGUI(aiPluginNode, 3, aiSynthGUI);
    
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
    
    // For more etails, see the help for AudioProcessor::prepareToPlay()


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

// GUI lifecycyle
//setupGUI is called once from the constructor
//paint is called every time the gui is redrawn and it does any custom painting
// on top of inner components
//resize is called when the window opens and whenever it is resized

void MainComponent::setupGUI()
{
    DBG("MainComponent::setupGUI");

    //int numPrograms = humanPlugin->getNumPrograms();
    int curProgram_human = humanPluginNode->getProcessor()->getCurrentProgram();
    String programName_human = humanPluginNode->getProcessor()->getProgramName(curProgram_human);
    
    //aiPlugin->setCurrentProgram(2);
    int curProgram_machine = aiPluginNode->getProcessor()->getCurrentProgram();
    String programName_machine = aiPluginNode->getProcessor()->getProgramName(curProgram_machine);
    
    //DBG("MainComponent:: presets " << numPrograms << " h:" << programName_human << ", m: " << programName_machine);

    
    
    // plugin editor for the algorithmic musician
    dinvernoSystemGUI = m_dinvernoSystemPluginInstanceNode->getProcessor()->createEditor();
    
    // Open Plugin GUI Editor: recorder
    recorderPluginGUI = m_dinvernoRecorderPluginInstanceNode->getProcessor()->createEditor();
    
    // VST Control Components: Human Setup
    humanSynthGUI.setTitle("Human Player");
    humanSynthGUI.setProgramName(programName_human);
    humanSynthGUI.setProgramNumber(curProgram_human);
    humanSynthGUI.addButtonListner(this);
    
    // VST Control Components: Machine setup
    aiSynthGUI.setTitle("Machine Player");
    aiSynthGUI.setProgramName(programName_machine);
    aiSynthGUI.setProgramNumber(curProgram_machine);
    aiSynthGUI.addButtonListner(this);
    
    addAndMakeVisible(humanSynthGUI);
    addAndMakeVisible(aiSynthGUI);
    addAndMakeVisible (recorderPluginGUI);
    addAndMakeVisible (dinvernoSystemGUI);

    
    
    // this triggers a call to resized, so doing it here to make
    // sure all the gui bits are ready
    setSize (800, 600);
    DBG("MainComponent::setupGUI: size set!");


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
    DBG("MainComponent::resized");
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    int colWidth = getWidth() / 3;
    int rowHeight = getHeight() / 4;
    int col = 0;
    int row = 0;
    
    humanSynthGUI.setBounds(col*colWidth, row*rowHeight, colWidth, rowHeight);
    col++;
    aiSynthGUI.setBounds(col*colWidth, row*rowHeight, colWidth, rowHeight);
    col++;
    recorderPluginGUI->setBounds(col*colWidth + colWidth/4, row*rowHeight, colWidth*0.74, rowHeight);
    row++;
    col = 0;
    // seems to ignore size changes
    dinvernoSystemGUI->setBounds(0, row*rowHeight, getWidth()*1.5, rowHeight*3);
    dinvernoSystemGUI->repaint(0, row*rowHeight, getWidth(), rowHeight*3);
}

// Button Listener
void MainComponent::buttonClicked (Button* button)
{
    int curProgram_human = humanPluginNode->getProcessor()->getCurrentProgram();
    int curProgram_machine = aiPluginNode->getProcessor()->getCurrentProgram();

    if (button == &humanSynthGUI.prevButton){
        // Human Prev Preset
        int nextProgram_human = curProgram_human - 1;
        humanPluginNode->getProcessor()->setCurrentProgram(nextProgram_human);
    }else if (button == &humanSynthGUI.nextButton){
        // Human Next Preset
        int nextProgram_human = curProgram_human + 1;
        humanPluginNode->getProcessor()->setCurrentProgram(nextProgram_human);
    }else if (button == &aiSynthGUI.prevButton){
        // Machine Next Preset
        int nextProgram_machine = curProgram_machine - 1;
        aiPluginNode->getProcessor()->setCurrentProgram(nextProgram_machine);
    }else if (button == &aiSynthGUI.nextButton){
         // Machine Next Preset
        int nextProgram_machine = curProgram_machine + 1;
        aiPluginNode->getProcessor()->setCurrentProgram(nextProgram_machine);
    }
    setPresetAndUpdateGUI(humanPluginNode, humanPluginNode->getProcessor()->getCurrentProgram(), humanSynthGUI);
    setPresetAndUpdateGUI(aiPluginNode, aiPluginNode->getProcessor()->getCurrentProgram(), aiSynthGUI);
    
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

void MainComponent::loadFXPFiles(const juce::File humanFile, const juce::File aiFile)
{
// this does not work for the serge xt unfortunately
    // it just ignores the fxp passed in setPreset in the visitor
    VSTVisitor visitor{};
    ((juce::AudioPluginInstance*) humanPluginNode->getProcessor())->getExtensions(visitor);
    
}

void MainComponent::setPresetAndUpdateGUI(AudioProcessorGraph::Node::Ptr& node, unsigned int index, VstControlComponent& gui)
{
    node->getProcessor()->setCurrentProgram(index);
    int index_ = node->getProcessor()->getCurrentProgram();
    
    gui.setProgramNumber(index_);
    gui.setProgramName(node->getProcessor()->getProgramName(index_));
}
