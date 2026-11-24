#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class R2RMetalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override;
};

class DJR2ROpticalTubeCompressorAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit DJR2ROpticalTubeCompressorAudioProcessorEditor(DJR2ROpticalTubeCompressorAudioProcessor&);
    ~DJR2ROpticalTubeCompressorAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override { repaint(); }
    DJR2ROpticalTubeCompressorAudioProcessor& audioProcessor;
    R2RMetalLookAndFeel metalLook;
    juce::Slider peak, gain, warmth;
    juce::ComboBox mode;
    juce::Label peakLabel, gainLabel, warmthLabel, modeLabel;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<SliderAttachment> peakAttachment, gainAttachment, warmthAttachment;
    std::unique_ptr<ComboAttachment> modeAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DJR2ROpticalTubeCompressorAudioProcessorEditor)
};
