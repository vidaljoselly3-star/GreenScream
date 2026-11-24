#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// Estética tipo pedal TS808: caja verde, potenciómetros negros con escala 0-10 y footswitch cromado.
class PedalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PedalLookAndFeel()
    {
        setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
        setColour (juce::TooltipWindow::backgroundColourId, juce::Colour (0xff101010));
        setColour (juce::TooltipWindow::textColourId, juce::Colours::white);

        // Selector de presets: papel crema + verde oscuro, como el panel del nombre
        const juce::Colour cream (0xfff4f1e6), darkGreen (0xff17531f), midGreen (0xff1d6a2a);
        setColour (juce::ComboBox::textColourId, darkGreen);
        setColour (juce::ComboBox::arrowColourId, darkGreen);
        setColour (juce::PopupMenu::backgroundColourId, cream);
        setColour (juce::PopupMenu::textColourId, darkGreen);
        setColour (juce::PopupMenu::headerTextColourId, darkGreen);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, midGreen);
        setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    }

    juce::Font getComboBoxFont (juce::ComboBox&) override { return makeFont (14.0f); }
    juce::Font getPopupMenuFont() override { return makeFont (14.0f); }

    void drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int,
                       juce::ComboBox& box) override
    {
        using juce::Colour;
        const auto r = juce::Rectangle<float> ((float) width, (float) height);
        const bool hot = box.isMouseOver (true) || box.isPopupActive();

        g.setColour (hot ? Colour (0xfffbf9f0) : Colour (0xfff4f1e6));
        g.fillRoundedRectangle (r.reduced (1.0f), 9.0f);

        g.setColour (Colour (0xff0d3c15));                       // borde exterior (como la caja)
        g.drawRoundedRectangle (r.reduced (1.0f), 9.0f, 2.0f);
        g.setColour (Colour (0xff1d6a2a));                       // filete interior (como el panel del nombre)
        g.drawRoundedRectangle (r.reduced (4.0f), 6.0f, 1.2f);

        const float cx = (float) width - 18.0f, cy = (float) height * 0.5f;
        juce::Path arrow;
        arrow.addTriangle (cx - 5.5f, cy - 2.5f, cx + 5.5f, cy - 2.5f, cx, cy + 3.5f);
        g.setColour (box.findColour (juce::ComboBox::arrowColourId));
        g.fillPath (arrow);
    }

    // Texto centrado respecto a toda la caja (zona simetrica a ambos lados de la flecha)
    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds (28, 2, box.getWidth() - 56, box.getHeight() - 4);
        label.setFont (getComboBoxFont (box));
    }

    static juce::Font makeFont (float size, bool bold = true)
    {
        juce::Font f { juce::FontOptions (size) };
        f.setBold (bold);
        return f;
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override
    {
        using juce::Colour;
        auto area = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height);
        const float size = juce::jmin (area.getWidth(), area.getHeight());
        const auto c = area.getCentre();
        const float R = size * 0.5f;
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        auto pt = [&] (float a, float r) { return juce::Point<float> (c.x + r * std::sin (a), c.y - r * std::cos (a)); };

        // Marcas y números 0..10
        for (int i = 0; i <= 10; ++i)
        {
            const float a = rotaryStartAngle + (float) i / 10.0f * (rotaryEndAngle - rotaryStartAngle);
            const bool major = (i % 5 == 0);
            g.setColour (juce::Colours::white.withAlpha (major ? 0.95f : 0.7f));
            g.drawLine (juce::Line<float> (pt (a, R * 0.70f), pt (a, R * (major ? 0.84f : 0.80f))), major ? 2.0f : 1.4f);

            auto tp = pt (a, R * 0.95f);
            g.setFont (makeFont (R * 0.17f));
            g.drawText (juce::String (i), juce::Rectangle<float> (18.0f, 12.0f).withCentre (tp),
                        juce::Justification::centred, false);
        }

        // Sombra del potenciómetro
        const float kr = R * 0.56f;
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.fillEllipse (juce::Rectangle<float> (kr * 2.0f, kr * 2.0f).withCentre (c + juce::Point<float> (2.0f, 4.0f)));

        // Anillo exterior
        g.setGradientFill (juce::ColourGradient (Colour (0xff6a6a6a), c.x - kr, c.y - kr,
                                                 Colour (0xff101010), c.x + kr, c.y + kr, false));
        g.fillEllipse (juce::Rectangle<float> (kr * 2.0f, kr * 2.0f).withCentre (c));

        // Cuerpo
        const float br = kr * 0.90f;
        g.setGradientFill (juce::ColourGradient (Colour (0xff3b3b3b), c.x - br * 0.6f, c.y - br * 0.8f,
                                                 Colour (0xff0b0b0b), c.x + br * 0.7f, c.y + br, true));
        g.fillEllipse (juce::Rectangle<float> (br * 2.0f, br * 2.0f).withCentre (c));

        // Moleteado
        g.setColour (juce::Colours::white.withAlpha (0.06f));
        for (int i = 0; i < 36; ++i)
        {
            const float a = (float) i / 36.0f * juce::MathConstants<float>::twoPi;
            g.drawLine (juce::Line<float> (pt (a, br * 0.92f), pt (a, br)), 1.0f);
        }

        // Tapa superior
        const float tr = br * 0.72f;
        g.setGradientFill (juce::ColourGradient (Colour (0xff2c2c2c), c.x, c.y - tr,
                                                 Colour (0xff050505), c.x, c.y + tr, false));
        g.fillEllipse (juce::Rectangle<float> (tr * 2.0f, tr * 2.0f).withCentre (c));

        // Indicador
        juce::Path p;
        p.addRoundedRectangle (-1.8f, -br * 0.92f, 3.6f, br * 0.55f, 1.5f);
        g.setColour (juce::Colours::white);
        g.fillPath (p, juce::AffineTransform::rotation (angle).translated (c));

        // Brillo
        g.setColour (juce::Colours::white.withAlpha (0.10f));
        g.drawEllipse (juce::Rectangle<float> (tr * 2.0f, tr * 2.0f).withCentre (c), 1.0f);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool down) override
    {
        using juce::Colour;
        auto area = b.getLocalBounds().toFloat().reduced (4.0f);
        const float size = juce::jmin (area.getWidth(), area.getHeight());
        auto c = area.getCentre();
        const float push = down ? 3.0f : 0.0f;

        // base
        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillEllipse (juce::Rectangle<float> (size, size).withCentre (c + juce::Point<float> (0.0f, 5.0f)));

        g.setGradientFill (juce::ColourGradient (Colour (0xfff2f2f2), c.x - size * 0.4f, c.y - size * 0.5f,
                                                 Colour (0xff5b5b5b), c.x + size * 0.4f, c.y + size * 0.5f, false));
        g.fillEllipse (juce::Rectangle<float> (size, size).withCentre (c));

        // hexágono / tuerca
        const float s2 = size * 0.80f;
        g.setGradientFill (juce::ColourGradient (Colour (0xffc9c9c9), c.x, c.y - s2 * 0.5f + push,
                                                 Colour (0xff767676), c.x, c.y + s2 * 0.5f + push, false));
        g.fillEllipse (juce::Rectangle<float> (s2, s2).withCentre (c + juce::Point<float> (0.0f, push)));

        // cabeza del pulsador
        const float s3 = size * 0.60f;
        g.setGradientFill (juce::ColourGradient (Colour (0xffe9e9e9), c.x - s3 * 0.3f, c.y - s3 * 0.5f + push,
                                                 Colour (0xff8a8a8a), c.x + s3 * 0.4f, c.y + s3 * 0.5f + push, false));
        g.fillEllipse (juce::Rectangle<float> (s3, s3).withCentre (c + juce::Point<float> (0.0f, push)));

        g.setColour (juce::Colours::white.withAlpha (highlighted ? 0.55f : 0.30f));
        g.drawEllipse (juce::Rectangle<float> (s3, s3).withCentre (c + juce::Point<float> (0.0f, push)), 1.2f);
    }
};
