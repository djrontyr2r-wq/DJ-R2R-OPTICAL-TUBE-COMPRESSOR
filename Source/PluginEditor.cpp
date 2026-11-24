#include "PluginEditor.h"

DJR2ROpticalTubeCompressorAudioProcessorEditor::DJR2ROpticalTubeCompressorAudioProcessorEditor(DJR2ROpticalTubeCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(760, 440);
    auto setupSlider = [this](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 24);
        s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff8a24));
        s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff393b43));
        s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffc36a));
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff17191f));
    };
    setupSlider(peak); setupSlider(gain); setupSlider(warmth);
    addAndMakeVisible(peak); addAndMakeVisible(gain); addAndMakeVisible(warmth);
    mode.addItem("COMPRESS", 1); mode.addItem("LIMIT", 2);
    mode.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff17191f));
    mode.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    addAndMakeVisible(mode);

    auto setupLabel = [this](juce::Label& l, const juce::String& t)
    {
        l.setText(t, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffd8d9df));
        l.setFont(juce::Font(13.0f, juce::Font::bold));
        addAndMakeVisible(l);
    };
    setupLabel(peakLabel, "PEAK REDUCTION");
    setupLabel(gainLabel, "OUTPUT GAIN");
    setupLabel(warmthLabel, "TUBE WARMTH");
    setupLabel(modeLabel, "OPERATING MODE");

    auto& state = audioProcessor.parameters;
    peakAttachment = std::make_unique<SliderAttachment>(state, "peak", peak);
    gainAttachment = std::make_unique<SliderAttachment>(state, "gain", gain);
    warmthAttachment = std::make_unique<SliderAttachment>(state, "warmth", warmth);
    modeAttachment = std::make_unique<ComboAttachment>(state, "mode", mode);
    startTimerHz(24);
}

void DJR2ROpticalTubeCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff090a0e));
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff171a22));
    g.fillRoundedRectangle(bounds.reduced(12.0f), 18.0f);
    g.setColour(juce::Colour(0xffff8a24));
    g.fillRoundedRectangle(juce::Rectangle<float>(28, 25, 5, 58), 2.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(27.0f, juce::Font::bold));
    g.drawText("DJ R2R", 48, 24, 190, 36, juce::Justification::left);
    g.setColour(juce::Colour(0xffffa044));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText("OPTICAL TUBE COMPRESSOR", 50, 59, 300, 20, juce::Justification::left);

    // VU-style gain reduction meter
    auto meter = juce::Rectangle<float>(480, 34, 230, 104);
    g.setColour(juce::Colour(0xff0b0c10));
    g.fillRoundedRectangle(meter, 10.0f);
    g.setColour(juce::Colour(0xff555965));
    g.drawRoundedRectangle(meter, 10.0f, 1.0f);
    g.setColour(juce::Colour(0xffc9cbd1));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("GAIN REDUCTION  /  dB", 495, 42, 200, 16, juce::Justification::centred);
    const float gr = juce::jlimit(0.0f, 20.0f, audioProcessor.getGainReductionDb());
    auto bar = juce::Rectangle<float>(500, 77, 190.0f * gr / 20.0f, 25.0f);
    g.setColour(juce::Colour(0xffff8a24));
    g.fillRoundedRectangle(bar, 5.0f);
    g.setColour(juce::Colour(0xff777b86));
    g.drawText(juce::String(gr, 1) + " dB", 495, 106, 200, 18, juce::Justification::centred);

    g.setColour(juce::Colour(0xff343741));
    g.drawLine(28.0f, 155.0f, 732.0f, 155.0f, 1.0f);
    g.setColour(juce::Colour(0xff8d929e));
    g.setFont(juce::Font(10.0f));
    g.drawText("SMOOTH OPTO RESPONSE  •  STEREO LINK  •  ANALOG-INSPIRED TONE", 30, 397, 700, 18, juce::Justification::centred);
}

void DJR2ROpticalTubeCompressorAudioProcessorEditor::resized()
{
    peakLabel.setBounds(55, 170, 180, 24);
    peak.setBounds(55, 195, 180, 165);
    gainLabel.setBounds(285, 170, 180, 24);
    gain.setBounds(285, 195, 180, 165);
    warmthLabel.setBounds(515, 170, 180, 24);
    warmth.setBounds(515, 195, 180, 165);
    modeLabel.setBounds(270, 365, 210, 22);
    mode.setBounds(485, 363, 150, 28);
}
