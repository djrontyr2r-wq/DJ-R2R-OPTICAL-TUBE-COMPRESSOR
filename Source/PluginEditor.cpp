#include "PluginEditor.h"
#include <cmath>

using namespace juce;

void R2RMetalLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int w, int h,
                                           float pos, float start, float end, Slider&)
{
    const float size = jmin((float)w, (float)h) * 0.72f;
    const float cx = x + w * 0.5f, cy = y + h * 0.46f;
    const float r = size * 0.5f;
    const float angle = start + pos * (end - start);

    g.setColour(Colour(0xff5b5146));
    for (int i = 0; i <= 20; ++i)
    {
        const float a = jmap((float)i / 20.0f, start, end);
        const float r1 = r + 7.0f, r2 = r + (i % 5 == 0 ? 17.0f : 12.0f);
        g.drawLine(cx + std::sin(a) * r1, cy - std::cos(a) * r1,
                   cx + std::sin(a) * r2, cy - std::cos(a) * r2,
                   i % 5 == 0 ? 2.0f : 1.0f);
    }

    Rectangle<float> knob(cx-r, cy-r, 2*r, 2*r);
    g.setColour(Colour(0xff17191b)); g.fillEllipse(knob.expanded(3.0f));
    g.setGradientFill(ColourGradient(Colour(0xff777b7e), cx-r*0.45f, cy-r*0.8f,
                                     Colour(0xff111315), cx+r*0.7f, cy+r*0.9f, false));
    g.fillEllipse(knob);
    g.setColour(Colour(0xffaeb2b4)); g.drawEllipse(knob, 1.2f);
    g.setColour(Colour(0xff3b3e40)); g.drawEllipse(knob.reduced(r*0.17f), 1.0f);
    g.setGradientFill(ColourGradient(Colour(0xfff0f1ef), cx-r*0.25f, cy-r*0.35f,
                                     Colour(0xff777b7d), cx+r*0.45f, cy+r*0.55f, false));
    g.fillEllipse(knob.reduced(r*0.28f));
    g.setColour(Colour(0xffb83b2b));
    g.drawLine(cx, cy-r*0.78f, cx + std::sin(angle)*r*0.78f,
               cy - std::cos(angle)*r*0.78f, 3.0f);
    g.setColour(Colour(0xfff0e9dc));
    g.setFont(Font(11.0f, Font::bold));
    g.drawText(String(Slider::getSliderSnapsToMousePosition()), x, y, 1, 1, Justification::centred);
}

DJR2ROpticalTubeCompressorAudioProcessorEditor::DJR2ROpticalTubeCompressorAudioProcessorEditor(DJR2ROpticalTubeCompressorAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(960, 540);
    auto setup = [this](Slider& s)
    {
        s.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(Slider::TextBoxBelow, false, 88, 22);
        s.setLookAndFeel(&metalLook);
        s.setColour(Slider::textBoxTextColourId, Colour(0xff252321));
        s.setColour(Slider::textBoxBackgroundColourId, Colour(0xffe7e0d4));
        s.setColour(Slider::textBoxOutlineColourId, Colour(0xff8b8174));
    };
    setup(peak); setup(gain); setup(warmth);
    addAndMakeVisible(peak); addAndMakeVisible(gain); addAndMakeVisible(warmth);

    mode.addItem("COMPRESS", 1); mode.addItem("LIMIT", 2);
    mode.setColour(ComboBox::backgroundColourId, Colour(0xff252729));
    mode.setColour(ComboBox::textColourId, Colour(0xffffead0));
    mode.setColour(ComboBox::outlineColourId, Colour(0xff9c3026));
    addAndMakeVisible(mode);

    auto label = [](Label& l, const String& t, float size = 15.0f)
    {
        l.setText(t, dontSendNotification);
        l.setJustificationType(Justification::centred);
        l.setColour(Label::textColourId, Colour(0xff17191b));
        l.setFont(Font(size, Font::bold));
    };
    label(peakLabel, "PEAK REDUCTION");
    label(gainLabel, "OUTPUT GAIN");
    label(warmthLabel, "TUBE WARMTH", 12.0f);
    label(modeLabel, "MODE", 12.0f);
    addAndMakeVisible(peakLabel); addAndMakeVisible(gainLabel);
    addAndMakeVisible(warmthLabel); addAndMakeVisible(modeLabel);

    auto& state = audioProcessor.parameters;
    peakAttachment = std::make_unique<SliderAttachment>(state, "peak", peak);
    gainAttachment = std::make_unique<SliderAttachment>(state, "gain", gain);
    warmthAttachment = std::make_unique<SliderAttachment>(state, "warmth", warmth);
    modeAttachment = std::make_unique<ComboAttachment>(state, "mode", mode);
    startTimerHz(24);
}

void DJR2ROpticalTubeCompressorAudioProcessorEditor::paint(Graphics& g)
{
    g.fillAll(Colour(0xff111315));
    auto outer = getLocalBounds().toFloat().reduced(8.0f);
    g.setGradientFill(ColourGradient(Colour(0xff45494b), outer.getX(), outer.getY(),
                                     Colour(0xff090a0b), outer.getRight(), outer.getBottom(), false));
    g.fillRoundedRectangle(outer, 10.0f);

    auto face = outer.reduced(9.0f);
    g.setGradientFill(ColourGradient(Colour(0xfff1eee7), face.getX(), face.getY(),
                                     Colour(0xffbcb9b1), face.getRight(), face.getBottom(), false));
    g.fillRoundedRectangle(face, 5.0f);
    g.setColour(Colour(0xff77746e)); g.drawRoundedRectangle(face, 5.0f, 1.4f);

    // Fine brushed-metal grain.
    g.saveState();
    g.reduceClipRegion(face.toNearestInt());
    for (int yy = 18; yy < getHeight(); yy += 4)
    {
        g.setColour(Colours::white.withAlpha(yy % 8 == 0 ? 0.10f : 0.045f));
        g.drawHorizontalLine(yy, 12.0f, 948.0f);
    }
    g.restoreState();

    // Rack screws.
    for (auto pt : { Point<float>(25,25), Point<float>(935,25), Point<float>(25,515), Point<float>(935,515) })
    {
        g.setColour(Colour(0xff292b2d)); g.fillEllipse(pt.x-10,pt.y-10,20,20);
        g.setColour(Colour(0xffc4c4c1)); g.drawEllipse(pt.x-7,pt.y-7,14,14,1.2f);
        g.setColour(Colour(0xff252729)); g.drawLine(pt.x-5,pt.y+4,pt.x+5,pt.y-4,1.8f);
    }

    // Brand panel.
    g.setColour(Colour(0xff151719)); g.setFont(Font("Arial", 30.0f, Font::bold));
    g.drawText("DJ R2R", 42, 48, 220, 42, Justification::centredLeft);
    g.setColour(Colour(0xffbd3026)); g.setFont(Font("Arial", 23.0f, Font::bold | Font::italic));
    g.drawText("Music", 83, 82, 145, 30, Justification::centred);
    g.setColour(Colour(0xffb52f25)); g.drawLine(42,116,245,116,2.2f);
    g.setColour(Colour(0xff282a2b)); g.setFont(Font(11.0f, Font::bold));
    g.drawText("ANALOG TONE / STEREO", 42, 124, 205, 20, Justification::centred);

    // Amber illuminated VU meter.
    Rectangle<float> meter(310.0f, 48.0f, 340.0f, 214.0f);
    g.setColour(Colour(0xff17191a)); g.fillRoundedRectangle(meter.expanded(7.0f), 7.0f);
    g.setColour(Colour(0xff5e5547)); g.drawRoundedRectangle(meter.expanded(7.0f), 7.0f, 2.0f);
    g.setGradientFill(ColourGradient(Colour(0xffffd17a), meter.getX(), meter.getY(),
                                     Colour(0xffd9822f), meter.getRight(), meter.getBottom(), false));
    g.fillRoundedRectangle(meter, 4.0f);
    g.setColour(Colour(0xff4a2d16)); g.drawRoundedRectangle(meter, 4.0f, 1.0f);
    g.setColour(Colour(0xff2a2118)); g.setFont(Font(13.0f, Font::bold));
    g.drawText("R2R  VU  /  GAIN REDUCTION", 325, 57, 310, 22, Justification::centred);

    const Point<float> c(480.0f, 218.0f);
    const float radius = 112.0f;
    const float a0 = MathConstants<float>::pi * 1.20f;
    const float a1 = MathConstants<float>::pi * 1.80f;
    Path arc; arc.addArc(c.x-radius,c.y-radius,c.x+radius,c.y+radius,a0,a1,true);
    g.setColour(Colour(0xff382719)); g.strokePath(arc, PathStrokeType(2.0f));
    for (int i=0;i<=10;++i)
    {
        float a=jmap((float)i/10.0f,a0,a1);
        float inner=radius-(i%2==0?17.0f:10.0f);
        g.drawLine(c.x+std::cos(a)*inner,c.y+std::sin(a)*inner,
                   c.x+std::cos(a)*radius,c.y+std::sin(a)*radius,i%2==0?2.0f:1.0f);
        if(i%2==0)
        {
            Point<float> p(c.x+std::cos(a)*(radius-34.0f),c.y+std::sin(a)*(radius-34.0f));
            g.setFont(Font(12.0f,Font::bold));
            g.drawText(String(i*2-10),Rectangle<int>((int)p.x-13,(int)p.y-8,26,16),Justification::centred);
        }
    }
    g.setColour(Colour(0xffc02e24)); g.drawLine(480,218,480+std::cos(a0+0.30f)*(radius-20),218+std::sin(a0+0.30f)*(radius-20),2.6f);
    g.fillEllipse(474,212,12,12);
    g.setColour(Colour(0xff3a2517)); g.setFont(Font(15.0f,Font::bold));
    g.drawText(String(audioProcessor.getGainReductionDb(),1)+" dB", 420, 232, 120, 22, Justification::centred);

    // Main model title and lower rack rail.
    g.setColour(Colour(0xff17191b)); g.setFont(Font("Arial", 25.0f, Font::bold | Font::italic));
    g.drawText("DJ R2R", 325, 286, 155, 35, Justification::centredRight);
    g.setColour(Colour(0xffbd3026));
    g.drawText("OPTICAL TUBE", 485, 286, 180, 35, Justification::centredLeft);
    g.setColour(Colour(0xff202224)); g.setFont(Font(17.0f,Font::bold));
    g.drawText("COMPRESSOR  /  LIMITER", 320, 320, 330, 25, Justification::centred);
    g.setColour(Colour(0xff9e3026)); g.fillRect(22, 492, 916, 3);
    g.setColour(Colour(0xff242628)); g.fillRect(12, 497, 936, 25);
    g.setColour(Colour(0xfff2eee5)); g.setFont(Font(10.0f,Font.bold));
    g.drawText("DJ R2R MUSIC   •   VINTAGE OPTO CHARACTER   •   BUILT FOR THE STUDIO",
               260, 498, 440, 20, Justification::centred);
}

void DJR2ROpticalTubeCompressorAudioProcessorEditor::resized()
{
    peakLabel.setBounds(265, 365, 190, 24);
    peak.setBounds(265, 382, 190, 112);
    gainLabel.setBounds(505, 365, 190, 24);
    gain.setBounds(505, 382, 190, 112);
    warmthLabel.setBounds(755, 150, 150, 22);
    warmth.setBounds(755, 168, 150, 105);
    modeLabel.setBounds(45, 315, 100, 22);
    mode.setBounds(45, 340, 155, 34);
}
