#pragma once

#include <array>

// Presets de fabrica. Los valores usan la misma escala 0-10 que los knobs
// (0 = 0 %, 5 = 50 %, 10 = 100 %). El bypass NO forma parte de los presets.
namespace gs
{
    struct FactoryPreset
    {
        const char* name;   // UTF-8
        float drive;
        float tone;
        float level;
    };

    inline constexpr int kNumFactoryPresets = 6;

    inline constexpr std::array<FactoryPreset, kNumFactoryPresets> kFactoryPresets {{
        { "Default (Init)",                                  5.0f, 5.0f, 6.7f },   // = valores por defecto de los knobs
        { "Clean Boost",                                     0.5f, 5.0f, 8.0f },
        { "Warm Blues Crunch",                               4.0f, 4.5f, 6.0f },
        { "Classic Rock Lead",                               7.5f, 6.0f, 5.5f },
        { "Bass Grit (Saturaci" "\xC3\xB3" "n Bajo)",        6.0f, 3.2f, 6.0f },   // "ó" escrita en UTF-8 explicito
        { "Vocal Edge (Calor Vocal)",                        1.8f, 4.5f, 5.5f }
    }};
}
