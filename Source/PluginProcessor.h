#pragma once
#include <JuceHeader.h>

class DJR2ROpticalTubeCompressorAudioProcessor : public juce::AudioProcessor
{
public:
    DJR2ROpticalTubeCompressorAudioProcessor();
    ~DJR2ROpticalTubeCompressorAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    float getGainReductionDb() const { return gainReductionDb.load(); }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    double sr = 44100.0;
    float envelope = 0.0f;
    float gainReductionDbValue = 0.0f;
    std::atomic<float> gainReductionDb { 0.0f };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DJR2ROpticalTubeCompressorAudioProcessor)
};
