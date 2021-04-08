/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainsyAudioProcessor::GainsyAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    params(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    if (!pipe.createNewPipe("mypipe"))
        DBG("failed to create mypipe: already exists");
    else
        DBG("pipe created!");

    juce::NamedPipe pipe2;
    if (!pipe2.openExisting("mypipe"))
        DBG("failed to create mypipe2: already exists");
    else
        DBG("pipe2 created!");

    auto data = "is this C++";
    auto ret = pipe.write(data, strlen(data) + 1, 200);
    DBG("aaa write returned: " + std::to_string(ret));

    char datum[256] = { 0 };
    ret = pipe2.read(datum, strlen(data) + 1, 200);
    DBG("read returned: " + std::to_string(ret));

    datum[strlen(data) + 1] = 0;
    DBG(std::string("Got back datum: '") + datum + "' of size: " + std::to_string(strlen(datum)));
    pipe2.close();
}

GainsyAudioProcessor::~GainsyAudioProcessor()
{
    DBG("Closing pipe");
    pipe.close();
}

//==============================================================================
const juce::String GainsyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainsyAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GainsyAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GainsyAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GainsyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainsyAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int GainsyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainsyAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GainsyAudioProcessor::getProgramName(int index)
{
    return {};
}

void GainsyAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void GainsyAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void GainsyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainsyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void GainsyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool GainsyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainsyAudioProcessor::createEditor()
{
    return new GainsyAudioProcessorEditor(*this);
}

//==============================================================================
void GainsyAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ValueTree state = params.copyState();
    std::unique_ptr<juce::XmlElement> xml = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void GainsyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    if (xml.get() != nullptr && xml->hasTagName(params.state.getType())) {
        juce::ValueTree state = juce::ValueTree::fromXml(*xml);
        params.replaceState(state);
    }
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout GainsyAudioProcessor::createParameterLayout()
{
    auto modeParamUniq = std::make_unique<juce::AudioParameterChoice>(
        "MODE", "Mode", juce::StringArray { "Before", "After" }, 0);

    auto channelParamUniq = std::make_unique<juce::AudioParameterInt>(
        "CHANNEL", "Channel", 1, 64, 1);
    
    modeParam = modeParamUniq.get();
    channelParam = channelParamUniq.get();

    return { std::move(modeParamUniq), std::move(channelParamUniq) };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainsyAudioProcessor();
}
