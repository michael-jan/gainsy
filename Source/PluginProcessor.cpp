/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

std::atomic<double> loudness[128] = {0.0};

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
    loudnessState = nullptr;
}

GainsyAudioProcessor::~GainsyAudioProcessor()
{
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
    if (!loudnessState) {
        if (!(loudnessState = ebur128_init(2, sampleRate, EBUR128_MODE_S)))
            DBG("this is very sad");
    }
}

void GainsyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    ebur128_destroy(&loudnessState);
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
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    
    juce::AudioDataConverters::interleaveSamples(buffer.getArrayOfReadPointers(),
                                                 deinterleavedBuf,
                                                 buffer.getNumSamples(),
                                                 totalNumInputChannels);
    
    ebur128_add_frames_float(loudnessState,
                             deinterleavedBuf,
                             buffer.getNumSamples() * totalNumInputChannels);
    
    double tmp;
    ebur128_loudness_shortterm(loudnessState, &tmp);
    loudness[*channelParam] = tmp;
    
    DBG(tmp);
    
    if (modeParam->getIndex() == 0) {
        // before
    } else {
        // after
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
