#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

DJR2ROpticalTubeCompressorAudioProcessor::DJR2ROpticalTubeCompressorAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

juce::AudioProcessorValueTreeState::ParameterLayout DJR2ROpticalTubeCompressorAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    p.push_back(std::make_unique<AudioParameterFloat>("peak", "Peak Reduction", NormalisableRange<float>(0.0f, 100.0f, 0.1f), 35.0f));
    p.push_back(std::make_unique<AudioParameterFloat>("gain", "Gain", NormalisableRange<float>(-12.0f, 24.0f, 0.1f), 0.0f));
    p.push_back(std::make_unique<AudioParameterChoice>("mode", "Mode", StringArray { "Compress", "Limit" }, 0));
    p.push_back(std::make_unique<AudioParameterFloat>("warmth", "Tube Warmth", NormalisableRange<float>(0.0f, 100.0f, 0.1f), 18.0f));
    return { p.begin(), p.end() };
}

bool DJR2ROpticalTubeCompressorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void DJR2ROpticalTubeCompressorAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sr = sampleRate;
    envelope = 0.0f;
    gainReductionDb = 0.0f;
}

void DJR2ROpticalTubeCompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = getTotalNumOutputChannels();
    const int samples = buffer.getNumSamples();
    for (int ch = getTotalNumInputChannels(); ch < channels; ++ch)
        buffer.clear(ch, 0, samples);

    const float peak = parameters.getRawParameterValue("peak")->load();
    const float makeup = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("gain")->load());
    const float mode = parameters.getRawParameterValue("mode")->load();
    const float warmth = parameters.getRawParameterValue("warmth")->load() * 0.01f;
    const float threshold = juce::jmap(peak, 0.0f, 100.0f, 0.92f, 0.10f);
    const float ratio = mode >= 0.5f ? 12.0f : 3.2f;
    const float attackMs = mode >= 0.5f ? 2.5f : 9.0f;
    const float releaseMs = 180.0f;
    const float attack = std::exp(-1.0f / (float(sr) * attackMs * 0.001f));
    const float release = std::exp(-1.0f / (float(sr) * releaseMs * 0.001f));
    float maxGr = 0.0f;

    for (int i = 0; i < samples; ++i)
    {
        float detector = 0.0f;
        for (int ch = 0; ch < getTotalNumInputChannels(); ++ch)
            detector = juce::jmax(detector, std::abs(buffer.getSample(ch, i)));

        const float coeff = detector > envelope ? attack : release;
        envelope = coeff * envelope + (1.0f - coeff) * detector;

        float reduction = 1.0f;
        if (envelope > threshold)
        {
            const float overDb = juce::Decibels::gainToDecibels(envelope / threshold);
            const float reducedDb = overDb - overDb / ratio;
            reduction = juce::Decibels::decibelsToGain(-reducedDb);
            maxGr = juce::jmax(maxGr, reducedDb);
        }

        for (int ch = 0; ch < getTotalNumInputChannels(); ++ch)
        {
            float x = buffer.getSample(ch, i) * reduction;
            // Gentle, level-dependent soft clipping as an original tube-colour stage.
            const float drive = 1.0f + warmth * 2.0f;
            x = std::tanh(x * drive) / std::tanh(drive);
            buffer.setSample(ch, i, x * makeup);
        }
    }
    gainReductionDbValue += 0.15f * (maxGr - gainReductionDbValue);
    gainReductionDb.store(gainReductionDbValue);
}

juce::AudioProcessorEditor* DJR2ROpticalTubeCompressorAudioProcessor::createEditor()
{
    return new DJR2ROpticalTubeCompressorAudioProcessorEditor(*this);
}

void DJR2ROpticalTubeCompressorAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}

void DJR2ROpticalTubeCompressorAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DJR2ROpticalTubeCompressorAudioProcessor();
}
