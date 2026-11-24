#include "PluginEditor.h"

namespace
{
    constexpr int kWidth  = 460;
    constexpr int kHeight = 660;

    void setupKnob (juce::Slider& s, juce::Component& parent, double defaultValue)
    {
        s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
        s.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                               juce::MathConstants<float>::pi * 2.75f, true);
        s.setDoubleClickReturnValue (true, defaultValue);
        s.setMouseDragSensitivity (180);
        s.setPopupDisplayEnabled (true, true, &parent);
        s.setNumDecimalPlacesToDisplay (1);
        parent.addAndMakeVisible (s);
    }
}

GreenScreamAudioProcessorEditor::GreenScreamAudioProcessorEditor (GreenScreamAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&lookAndFeel);

    setupKnob (driveSlider, *this, 5.0);
    setupKnob (toneSlider,  *this, 5.0);
    setupKnob (levelSlider, *this, 6.7);

    // Selector de presets de fabrica
    for (int i = 0; i < gs::kNumFactoryPresets; ++i)
        presetBox.addItem (juce::String::fromUTF8 (gs::kFactoryPresets[(size_t) i].name), i + 1);

    presetBox.setJustificationType (juce::Justification::centred);
    presetBox.setTextWhenNothingSelected ("Custom");
    presetBox.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    presetBox.setWantsKeyboardFocus (false);
    presetBox.onChange = [this]
    {
        const int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0)
        {
            lastPresetShown = idx;
            audioProcessor.applyPreset (idx, true);   // mueve Drive / Tone / Level; los knobs siguen libres
        }
    };
    addAndMakeVisible (presetBox);

    footswitch.setClickingTogglesState (true);
    footswitch.setMouseCursor (juce::MouseCursor::PointingHandCursor);
    addAndMakeVisible (footswitch);

    driveAttach  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "drive",  driveSlider);
    toneAttach   = std::make_unique<SliderAttachment> (audioProcessor.apvts, "tone",   toneSlider);
    levelAttach  = std::make_unique<SliderAttachment> (audioProcessor.apvts, "level",  levelSlider);
    bypassAttach = std::make_unique<ButtonAttachment> (audioProcessor.apvts, "bypass", footswitch);

    setSize (kWidth, kHeight);
    setResizable (false, false);

    lastActive = isActive();
    syncPresetBox();
    startTimerHz (20);
}

GreenScreamAudioProcessorEditor::~GreenScreamAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

bool GreenScreamAudioProcessorEditor::isActive() const
{
    return ! footswitch.getToggleState();   // el parámetro es "bypass"
}

void GreenScreamAudioProcessorEditor::syncPresetBox()
{
    // Refleja el estado real de los knobs: si coinciden con un preset se muestra su nombre;
    // si el usuario mueve un knob (o la automatizacion lo cambia) pasa a "Custom".
    const int idx = audioProcessor.findMatchingPreset();
    if (idx != lastPresetShown)
    {
        lastPresetShown = idx;
        presetBox.setSelectedId (idx >= 0 ? idx + 1 : 0, juce::dontSendNotification);
    }
}

void GreenScreamAudioProcessorEditor::timerCallback()
{
    syncPresetBox();

    const bool a = isActive();
    if (a != lastActive)
    {
        lastActive = a;
        repaint (ledRect.getSmallestIntegerContainer().expanded (20));
    }
}

void GreenScreamAudioProcessorEditor::resized()
{
    bodyRect = juce::Rectangle<float> (18.0f, 18.0f, (float) getWidth() - 36.0f, (float) getHeight() - 36.0f);

    presetBox.setBounds (getWidth() / 2 - 140, 36, 280, 28);

    const int knobSize = 132;
    const int rowY = 80;
    const int gap = (getWidth() - 3 * knobSize) / 4;

    driveSlider.setBounds (gap, rowY, knobSize, knobSize);
    toneSlider.setBounds  (gap * 2 + knobSize, rowY, knobSize, knobSize);
    levelSlider.setBounds (gap * 3 + knobSize * 2, rowY, knobSize, knobSize);

    driveLabel = { driveSlider.getX(), rowY + knobSize - 2, knobSize, 24 };
    toneLabel  = { toneSlider.getX(),  rowY + knobSize - 2, knobSize, 24 };
    levelLabel = { levelSlider.getX(), rowY + knobSize - 2, knobSize, 24 };

    const int fsSize = 120;
    footswitch.setBounds (getWidth() / 2 - fsSize / 2, getHeight() - fsSize - 52, fsSize, fsSize);

    ledRect = juce::Rectangle<float> (16.0f, 16.0f).withCentre ({ (float) getWidth() * 0.5f, (float) footswitch.getY() - 26.0f });
}

void GreenScreamAudioProcessorEditor::drawScrew (juce::Graphics& g, juce::Point<float> c, float r)
{
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffe0e0e0), c.x - r, c.y - r,
                                             juce::Colour (0xff5a5a5a), c.x + r, c.y + r, false));
    g.fillEllipse (juce::Rectangle<float> (r * 2.0f, r * 2.0f).withCentre (c));
    g.setColour (juce::Colours::black.withAlpha (0.65f));
    g.drawLine (c.x - r * 0.6f, c.y - r * 0.2f, c.x + r * 0.6f, c.y + r * 0.2f, 1.6f);
}

void GreenScreamAudioProcessorEditor::paint (juce::Graphics& g)
{
    using juce::Colour;

    // Fondo
    g.setGradientFill (juce::ColourGradient (Colour (0xff2a2a2c), 0.0f, 0.0f,
                                             Colour (0xff0c0c0d), 0.0f, (float) getHeight(), false));
    g.fillAll();

    // Sombra de la caja
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillRoundedRectangle (bodyRect.translated (0.0f, 6.0f), 22.0f);

    // Caja verde
    g.setGradientFill (juce::ColourGradient (Colour (0xff3fa64a), bodyRect.getX(), bodyRect.getY(),
                                             Colour (0xff1d6a2a), bodyRect.getRight(), bodyRect.getBottom(), false));
    g.fillRoundedRectangle (bodyRect, 22.0f);

    // Brillo superior
    g.setGradientFill (juce::ColourGradient (juce::Colours::white.withAlpha (0.20f), 0.0f, bodyRect.getY(),
                                             juce::Colours::transparentWhite, 0.0f, bodyRect.getY() + 160.0f, false));
    g.fillRoundedRectangle (bodyRect.reduced (2.0f), 20.0f);

    // Borde
    g.setColour (Colour (0xff0d3c15));
    g.drawRoundedRectangle (bodyRect, 22.0f, 3.0f);
    g.setColour (juce::Colours::white.withAlpha (0.18f));
    g.drawRoundedRectangle (bodyRect.reduced (3.0f), 19.0f, 1.0f);

    // Tornillos
    const float inset = 26.0f;
    drawScrew (g, { bodyRect.getX() + inset, bodyRect.getY() + inset }, 6.5f);
    drawScrew (g, { bodyRect.getRight() - inset, bodyRect.getY() + inset }, 6.5f);
    drawScrew (g, { bodyRect.getX() + inset, bodyRect.getBottom() - inset }, 6.5f);
    drawScrew (g, { bodyRect.getRight() - inset, bodyRect.getBottom() - inset }, 6.5f);

    // Etiquetas
    g.setColour (juce::Colours::white);
    g.setFont (PedalLookAndFeel::makeFont (20.0f));
    g.drawText ("DRIVE", driveLabel, juce::Justification::centred);
    g.drawText ("TONE",  toneLabel,  juce::Justification::centred);
    g.drawText ("LEVEL", levelLabel, juce::Justification::centred);

    // Panel de nombre
    auto panel = juce::Rectangle<float> (bodyRect.getX() + 40.0f, (float) driveLabel.getBottom() + 30.0f,
                                         bodyRect.getWidth() - 80.0f, 150.0f);
    g.setColour (juce::Colours::black.withAlpha (0.25f));
    g.fillRoundedRectangle (panel.translated (0.0f, 3.0f), 10.0f);
    g.setColour (Colour (0xfff4f1e6));
    g.fillRoundedRectangle (panel, 10.0f);
    g.setColour (Colour (0xff1d6a2a));
    g.drawRoundedRectangle (panel.reduced (5.0f), 7.0f, 1.5f);

    // Cada linea se centra en el ancho util del panel (con margen interior) y, si el texto
    // no cupiera, se reduce horizontalmente en vez de cortarse.
    const auto inner = panel.reduced (16.0f, 0.0f).toNearestInt();
    const int top = (int) panel.getY();
    auto line = [&] (int yOffset, int h) { return juce::Rectangle<int> (inner.getX(), top + yOffset, inner.getWidth(), h); };

    // "GreenScream"
    g.setColour (Colour (0xff17531f));
    juce::Font titleFont { juce::FontOptions (40.0f) };
    titleFont.setBold (true);
    titleFont.setItalic (true);
    g.setFont (titleFont);
    g.drawFittedText ("GreenScream", line (10, 48), juce::Justification::centred, 1, 0.7f);

    // "by Vidal"
    juce::Font byFont { juce::FontOptions (20.0f) };
    byFont.setItalic (true);
    g.setColour (Colour (0xff17531f).withAlpha (0.9f));
    g.setFont (byFont);
    g.drawFittedText ("by Vidal", line (56, 26), juce::Justification::centred, 1, 0.7f);

    g.setColour (Colour (0xffc0392b));
    g.setFont (PedalLookAndFeel::makeFont (17.0f));
    g.drawFittedText ("O V E R D R I V E   O D - 8 0 8", line (88, 22), juce::Justification::centred, 1, 0.7f);

    g.setColour (Colour (0xff17531f).withAlpha (0.8f));
    g.setFont (PedalLookAndFeel::makeFont (11.0f, false));
    g.drawFittedText ("TUBE SCREAMER STYLE  -  4x OVERSAMPLED", line (116, 20), juce::Justification::centred, 1, 0.7f);

    // LED
    const bool active = isActive();
    const auto lc = ledRect.getCentre();
    if (active)
    {
        g.setGradientFill (juce::ColourGradient (Colour (0xffff4040).withAlpha (0.55f), lc.x, lc.y,
                                                 Colour (0xffff4040).withAlpha (0.0f), lc.x + 26.0f, lc.y, true));
        g.fillEllipse (juce::Rectangle<float> (52.0f, 52.0f).withCentre (lc));
    }
    g.setColour (Colour (0xff111111));
    g.fillEllipse (ledRect.expanded (3.0f));
    g.setGradientFill (juce::ColourGradient (active ? Colour (0xffff9a9a) : Colour (0xff6b2323), lc.x - 3.0f, lc.y - 3.0f,
                                             active ? Colour (0xffe01212) : Colour (0xff2b0a0a), lc.x + 6.0f, lc.y + 6.0f, false));
    g.fillEllipse (ledRect);

    // Texto del footswitch
    g.setColour (juce::Colours::white.withAlpha (0.9f));
    g.setFont (PedalLookAndFeel::makeFont (13.0f));
    g.drawText (active ? "ON" : "BYPASS", juce::Rectangle<int> (getWidth() / 2 - 60, footswitch.getBottom() + 4, 120, 18),
                juce::Justification::centred);
}
