/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bitDepth", "Bit Depth", 1.0f, 32.0f, 32.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("rateReduction", "Rate Reduce", 1.0f, 50.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));
    
    return { params.begin(), params.end() };
}


//==============================================================================
MyFirstVstAudioProcessor::MyFirstVstAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters (*this, nullptr, juce::Identifier ("BitCrusherParams"), createParameterLayout())
{
    bitDepthParameter = parameters.getRawParameterValue("bitDepth");
    rateReductionParameter = parameters.getRawParameterValue("rateReduction");
    mixParameter = parameters.getRawParameterValue("mix");
}

MyFirstVstAudioProcessor::~MyFirstVstAudioProcessor()
{
}

//==============================================================================
const juce::String MyFirstVstAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyFirstVstAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyFirstVstAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyFirstVstAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyFirstVstAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyFirstVstAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyFirstVstAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyFirstVstAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyFirstVstAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyFirstVstAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}




//==============================================================================
void MyFirstVstAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MyFirstVstAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyFirstVstAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MyFirstVstAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    const float bitDepth = bitDepthParameter->load();
    const float rateReduction = rateReductionParameter->load();
    const float mix = mixParameter->load();
    
    const float quantizeStep = 1.0f /(pow(2.0f, bitDepth) - 1.0f);
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float drySample = channelData[sample];
            float wetSample = drySample;
            
            // sample rate reduction
            if (downsampleCounter >= rateReduction)
            {
                downsampleCounter = 1;
                wetSample = drySample; // capture new sample
            }
            else
            {
                // use last captureed sample based on channel
                if (channel == 0) wetSample = lastSampleLeft;
                else wetSample = lastSampleLeft;
                
                downsampleCounter++;
            }
            
            // store last captured sample for next iteration
            if (channel == 0) lastSampleLeft = wetSample;
            else lastSampleRight = wetSample;
            
            // bit depth reduction
            if (bitDepth < 32.0f)
            {
                wetSample = quantizeStep * floor(wetSample / quantizeStep);
            }
            
            // mix
            channelData[sample] = (wetSample * mix) + (drySample * (1.0f - mix));
        }
    }
}

//==============================================================================
bool MyFirstVstAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyFirstVstAudioProcessor::createEditor()
{
    return new MyFirstVstAudioProcessorEditor (*this);
}

//==============================================================================
void MyFirstVstAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MyFirstVstAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyFirstVstAudioProcessor();
}
