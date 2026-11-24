#include "PluginEditor.h"
#include <cmath>

DJR2ROpticalTubeCompressorAudioProcessorEditor::DJR2ROpticalTubeCompressorAudioProcessorEditor(DJR2ROpticalTubeCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(900, 540);
    auto setup = [](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 22);
        s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb66b2b));
        s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff77716a));
        s.setColour(juce::Slider::thumbColourId, juce::Colour(0xff292a2b));
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff282522));
        s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xffe8dfcc));
        s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xffaaa08d));
    };
    setup(peak); setup(gain); setup(warmth);
    addAndMakeVisible(peak); addAndMakeVisible(gain); addAndMakeVisible(warmth);
    mode.addItem("COMPRESS", 1); mode.addItem("LIMIT", 2);
    mode.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff292724));
    mode.setColour(juce::ComboBox::textColourId, juce::Colour(0xffffe8c2));
    mode.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffa76a36));
    addAndMakeVisible(mode);
    auto label = [](juce::Label& l, const juce::String& t)
    {
        l.setText(t, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, juce::Colour(0xff302b25));
        l.setFont(juce::Font(13.0f, juce::Font::bold));
    };
    label(peakLabel, "PEAK REDUCTION"); label(gainLabel, "GAIN");
    label(warmthLabel, "TUBE WARMTH"); label(modeLabel, "MODE");
    addAndMakeVisible(peakLabel); addAndMakeVisible(gainLabel);
    addAndMakeVisible(warmthLabel); addAndMakeVisible(modeLabel);
    auto& state = audioProcessor.parameters;
    peakAttachment = std::make_unique<SliderAttachment>(state, "peak", peak);
    gainAttachment = std::make_unique<SliderAttachment>(state, "gain", gain);
    warmthAttachment = std::make_unique<SliderAttachment>(state, "warmth", warmth);
    modeAttachment = std::make_unique<ComboAttachment>(state, "mode", mode);
    startTimerHz(24);
}

void DJR2ROpticalTubeCompressorAudioProcessorEditor::paint(juce::Graphics& g)
{
    using namespace juce;
    g.fillAll(Colour(0xff11100e));
    auto panel = getLocalBounds().toFloat().reduced(16.0f);
    g.setColour(Colour(0xff28231e)); g.fillRoundedRectangle(panel, 18.0f);
    g.setColour(Colour(0xffbd7a3d)); g.drawRoundedRectangle(panel, 18.0f, 2.0f);
    auto face = panel.reduced(12.0f);
    g.setColour(Colour(0xffd9cfb9)); g.fillRoundedRectangle(face, 12.0f);
    g.setColour(Colour(0xff8e806b)); g.drawRoundedRectangle(face, 12.0f, 1.2f);

    g.setColour(Colour(0xff2c2924)); g.setFont(Font(28.0f, Font::bold));
    g.drawText("DJ R2R", 42, 29, 220, 38, Justification::left);
    g.setColour(Colour(0xffa75e2b)); g.setFont(Font(12.0f, Font::bold));
    g.drawText("OPTICAL TUBE COMPRESSOR", 44, 66, 260, 20, Justification::left);
    g.setColour(Colour(0xff8c806e)); g.drawLine(40.0f, 94.0f, 860.0f, 94.0f, 1.0f);

    Rectangle<float> meter(318.0f, 112.0f, 264.0f, 174.0f);
    g.setColour(Colour(0xfff3ead5)); g.fillRoundedRectangle(meter, 9.0f);
    g.setColour(Colour(0xff4a4034)); g.drawRoundedRectangle(meter, 9.0f, 2.0f);
    g.setColour(Colour(0xff302a23)); g.setFont(Font(12.0f, Font::bold));
    g.drawText("VU  /  GAIN REDUCTION", meter.toNearestInt().withHeight(22).translated(0, 7), Justification::centred);

    const Point<float> centre(450.0f, 254.0f);
    const float radius = 91.0f;
    const float a0 = MathConstants<float>::pi * 1.18f;
    const float a1 = MathConstants<float>::pi * 1.82f;
    Path arc; arc.addArc(centre.x-radius, centre.y-radius, radius*2, radius*2, a0, a1, true);
    g.setColour(Colour(0xff3d352b)); g.strokePath(arc, PathStrokeType(2.0f));
    for (int i=0; i<=10; ++i)
    {
        const float a=jmap(float(i)/10.0f,a0,a1);
        const float inner=radius-(i%2==0?13.0f:8.0f);
        Point<float> p1(centre.x+std::cos(a)*inner,centre.y+std::sin(a)*inner);
        Point<float> p2(centre.x+std::cos(a)*radius,centre.y+std::sin(a)*radius);
        g.drawLine(Line<float>(p1,p2),i%2==0?1.8f:1.0f);
        if(i%2==0)
        {
            Point<float> lp(centre.x+std::cos(a)*(radius-27.0f),centre.y+std::sin(a)*(radius-27.0f));
            g.setFont(Font(10.0f,Font::bold));
            g.drawText(String(i*2),Rectangle<float>(lp.x-12,lp.y-7,24,14),Justification::centred);
        }
    }
    const float gr=jlimit(0.0f,20.0f,audioProcessor.getGainReductionDb());
    const float na=jmap(gr,0.0f,20.0f,a0,a1);
    Point<float> tip(centre.x+std::cos(na)*(radius-18),centre.y+std::sin(na)*(radius-18));
    g.setColour(Colour(0xffb33b25)); g.drawLine(Line<float>(centre,tip),2.5f);
    g.fillEllipse(centre.x-5,centre.y-5,10,10);
    g.setColour(Colour(0xff302a23)); g.setFont(Font(10.0f,Font::bold));
    g.drawText(String(gr,1)+" dB GR",meter.toNearestInt().withY(262).withHeight(18),Justification::centred);

    g.setColour(Colour(0xffa75e2b)); g.fillRoundedRectangle(Rectangle<float>(365,302,170,24),5);
    g.setColour(Colours::white); g.setFont(Font(10.0f,Font::bold));
    g.drawText("R2R  •  CLASS A OPTICAL",365,304,170,20,Justification::centred);
    g.setColour(Colour(0xff8b806f)); g.drawLine(40,462,860,462,1);
    g.setColour(Colour(0xff655b4e)); g.setFont(Font(10.0f,Font::bold));
    g.drawText("ANALOG-INSPIRED OPTO RESPONSE   •   STEREO LINK   •   DJ R2R MUSIC",40,474,820,18,Justification::centred);
    for(auto pt:{Point<float>(34,34),Point<float>(866,34),Point<float>(34,506),Point<float>(866,506)})
    {
        g.setColour(Colour(0xff665b4d)); g.fillEllipse(pt.x-4,pt.y-4,8,8);
        g.setColour(Colour(0xffd7c9ae)); g.drawEllipse(pt.x-2,pt.y-2,4,4,1);
    }
}

void DJR2ROpticalTubeCompressorAudioProcessorEditor::resized()
{
    gainLabel.setBounds(72,142,190,24); gain.setBounds(75,165,190,190);
    peakLabel.setBounds(638,142,190,24); peak.setBounds(638,165,190,190);
    warmthLabel.setBounds(335,348,230,22); warmth.setBounds(350,365,200,92);
    modeLabel.setBounds(640,390,90,24); mode.setBounds(725,388,120,28);
}
