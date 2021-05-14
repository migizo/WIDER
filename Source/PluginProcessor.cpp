/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WIDERAudioProcessor::WIDERAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                       
#else
    :
#endif
parameters(*this, nullptr, juce::Identifier(JucePlugin_Name),
    {
        std::make_unique<juce::AudioParameterFloat>(TRANS("WIDE"), TRANS("WIDE"), juce::NormalisableRange<float>(0.f, 100.f, 1.f), 50.0f),
    })
{
    wideParameter = parameters.getRawParameterValue("WIDE");
}

WIDERAudioProcessor::~WIDERAudioProcessor()
{
}

//==============================================================================
const juce::String WIDERAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WIDERAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WIDERAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WIDERAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WIDERAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WIDERAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WIDERAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WIDERAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WIDERAudioProcessor::getProgramName (int index)
{
    return {};
}

void WIDERAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WIDERAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // DSP moduleクラスオブジェクトの初期化に必要な情報を保持する構造体
    juce::dsp::ProcessSpec spec;
    
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    spec.maximumBlockSize = samplesPerBlock;
    
    delayL.prepare(spec);
    delayR.prepare(spec);
}

void WIDERAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WIDERAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WIDERAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    double wide = *wideParameter / 100.0;
    double mix = jmin(1.0, jmax(0.0, jmap(wide, 0.0, 1.0, 0.0, 1.0)));
    int delayTimeSampleNum = getSampleRate() * 0.02;
    
    auto* inData = buffer.getReadPointer (0);
    
    auto getPan = [](bool isL, double pan/* -1.0 ~ 1.0 */) {
        if (isL) return jmap(pan, -1.0, 1.0, 1.0, 0.0);
        else return jmap(pan, -1.0, 1.0, 0.0, 1.0);
    };
    
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* outData = buffer.getWritePointer (channel);
        auto& delay = channel == 0 ? delayL : delayR;

        for (int i = 0; i < numSamples; i++) {
            delay.pushSample(0, inData[i]);
            float delayData = mix * delay.popSample(0, delayTimeSampleNum) + (1.0 - mix) * inData[i];
            outData[i] = getPan(channel == 0, -wide) * inData[i];
            outData[i] += getPan(channel == 0, wide) * delayData;
        }
    }
}

//==============================================================================
bool WIDERAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WIDERAudioProcessor::createEditor()
{
    return new WIDERAudioProcessorEditor (*this);
}

//==============================================================================
void WIDERAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void WIDERAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WIDERAudioProcessor();
}
