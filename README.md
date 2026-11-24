# GreenScream by Vidal — VST3 para Windows (overdrive estilo TS808)

Plugin **VST3** de overdrive (compañía **Vidal**, código de fabricante `Vdal`) que emula el circuito del *Ibanez Tube Screamer TS808*
(la misma función que el *UAD TS Overdrive*), pensado para **Reaper** en Windows.

## Controles (solo lo necesario)

| Control | Función |
|---------|---------|
| **Drive** (0–10) | Ganancia del op-amp: 51 kΩ + potenciómetro 500 kΩ |
| **Tone** (0–10) | Filtro de tono del TS (≈720 Hz → 3,2 kHz) |
| **Level** (0–10) | Volumen de salida (−24 dB … +12 dB, ≈0 dB en 6,7) |
| **Footswitch** | Bypass con crossfade (sin clics) |

Doble clic en un potenciómetro = valor por defecto.

## Presets de fábrica

El selector de la parte superior del plugin carga Drive, Tone y Level de una vez (el footswitch/bypass no se toca).
Los knobs siguen libres: si mueves uno después de elegir un preset, el selector pasa a **Custom**.

| Preset | Drive | Tone | Level |
|--------|:-----:|:----:|:-----:|
| Default (Init) | 5,0 | 5,0 | 6,7 |
| Clean Boost | 0,5 | 5,0 | 8,0 |
| Warm Blues Crunch | 4,0 | 4,5 | 6,0 |
| Classic Rock Lead | 7,5 | 6,0 | 5,5 |
| Bass Grit (Saturación Bajo) | 6,0 | 3,2 | 6,0 |
| Vocal Edge (Calor Vocal) | 1,8 | 4,5 | 5,5 |

Para añadir o cambiar presets edita `plugin/Source/Presets.h` (valores en la escala 0–10 de los knobs; si añades uno, sube `kNumFactoryPresets`).

## Modelo DSP

1. Bloqueo de DC de entrada.
2. Etapa de ganancia: `H(s) = 1 + Zf/Zg`, con `Zf = (51k + Drive) ‖ 51 pF` y `Zg = 4,7 kΩ + 47 nF` (la “joroba” de medios del TS), discretizada con transformada bilineal.
3. Saturación suave del op-amp (raíles ±4,5 V).
4. Recortador de diodos 1N914 antiparalelo (R = 1 kΩ, C = 47 nF) resuelto **muestra a muestra** con Euler implícito + Newton-Raphson.
5. Control de tono.
6. **Sobremuestreo ×4** (reduce aliasing) con latencia informada al host (Reaper compensa el retardo).
7. Level + mezcla bypass con señal seca alineada.

El runtime de MSVC va enlazado estáticamente: **no hace falta instalar el Visual C++ Redistributable**.

## Compilar en tu GitHub (sin instalar nada en tu PC)

1. Crea un repositorio vacío en GitHub (por ejemplo `greenscream-by-vidal`).
2. Desde la carpeta de este proyecto:

```bash
git init
git add .
git commit -m "GreenScream by Vidal VST3"
git branch -M main
git remote add origin https://github.com/TU_USUARIO/greenscream-by-vidal.git
git push -u origin main
```

3. Entra en la pestaña **Actions** → *Build VST3 (Windows)*. Tarda unos 5–10 minutos (descarga JUCE y compila).
4. Al terminar, descarga el artefacto **GreenScream-by-Vidal-VST3-Windows** (zip con `GreenScream by Vidal.vst3`).
5. (Opcional) Para publicar una *Release* con el zip: `git tag v1.0.0 && git push origin v1.0.0`.

## Instalar en Reaper

1. Copia la carpeta `GreenScream by Vidal.vst3` a `C:\Program Files\Common Files\VST3\`.
2. En Reaper: **Options → Preferences → Plug-ins → VST → Re-scan** (o *Clear cache and re-scan*).
3. Añade **GreenScream by Vidal** desde el FX de una pista (busca “GreenScream”).

## Compilar en local (opcional)

Requisitos: Visual Studio 2022 (carga de trabajo C++) y CMake ≥ 3.22.

```bat
cmake -S plugin -B build -A x64
cmake --build build --config Release
```

El resultado queda en `build/GreenScreamOD_artefacts/Release/VST3/GreenScream by Vidal.vst3`.

## Licencia

Construido con [JUCE](https://juce.com) (AGPLv3 / licencia comercial). Si distribuyes el plugin, respeta la licencia de JUCE.
“Ibanez”, “Tube Screamer”, “TS808” y “UAD” son marcas de sus respectivos propietarios; este proyecto es un modelo original e independiente sin afiliación con ellos.
