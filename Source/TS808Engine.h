#pragma once

#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------------
//  Modelo de circuito del Ibanez TS808 (estilo "Tube Screamer").
//
//  Cadena por canal (se ejecuta a frecuencia sobremuestreada):
//    1. Entrada: bloqueo de DC (condensador de acoplo)
//    2. Etapa de ganancia del op-amp (no inversora):
//          H(s) = 1 + Zf/Zg
//          Zf = (51k + Drive[0..500k]) || 51pF
//          Zg = 4.7k + 0.047uF            -> joroba de medios ~720 Hz
//       seguida de saturación suave por raíles del op-amp (±4.5 V)
//    3. Recortador de diodos: R=1k, C=47nF, par de diodos 1N914 antiparalelo
//       (ecuación diferencial resuelta con Euler implícito + Newton-Raphson)
//    4. Control de tono: pasa-bajos variable (~720 Hz .. 3.2 kHz) mezclado
//       con la señal directa para recuperar agudos.
// -----------------------------------------------------------------------------
namespace gs
{
struct Biquad
{
    double b0 = 1.0, b1 = 0.0, b2 = 0.0, a1 = 0.0, a2 = 0.0;
    double z1 = 0.0, z2 = 0.0;

    inline double process (double x) noexcept
    {
        const double y = b0 * x + z1;
        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;
        return y;
    }

    void reset() noexcept { z1 = z2 = 0.0; }
};

class TS808Channel
{
public:
    static constexpr double kInputVolts = 0.25;   // 0 dBFS = 0.25 V de pico a la entrada del pedal
    static constexpr double kRailVolts  = 4.5;    // saturación del op-amp (pila 9 V / 2)
    static constexpr double kOutputGain = 1.0;    // V -> escala digital

    void prepare (double oversampledRate)
    {
        fs = oversampledRate;
        dcR = 1.0 - (2.0 * 3.14159265358979323846 * 12.0 / fs);
        reset();
        setDrive (0.5);
        setTone (0.5);
    }

    void reset() noexcept
    {
        gainStage.reset();
        dcX1 = dcY1 = 0.0;
        vCap = 0.0;
        toneState = 0.0;
    }

    // drive: 0..1
    void setDrive (double drive01) noexcept
    {
        drive01 = std::clamp (drive01, 0.0, 1.0);

        const double potTaper = drive01 * drive01;              // potenciómetro 500k logarítmico (aprox.)
        const double Rf = 51.0e3 + 500.0e3 * potTaper;
        const double Cf = 51.0e-12;
        const double Rg = 4.7e3;
        const double Cg = 0.047e-6;

        // H(s) = N(s)/D(s)
        const double tf = Rf * Cf;
        const double tg = Rg * Cg;
        const double d1 = tf + tg;
        const double n1 = d1 + Rf * Cg;
        const double s2 = tf * tg;

        const double k = 2.0 * fs;
        const double k2 = k * k;

        const double n0c = 1.0 + n1 * k + s2 * k2;
        const double n1c = 2.0 - 2.0 * s2 * k2;
        const double n2c = 1.0 - n1 * k + s2 * k2;

        const double d0c = 1.0 + d1 * k + s2 * k2;
        const double d1c = 2.0 - 2.0 * s2 * k2;
        const double d2c = 1.0 - d1 * k + s2 * k2;

        gainStage.b0 = n0c / d0c;
        gainStage.b1 = n1c / d0c;
        gainStage.b2 = n2c / d0c;
        gainStage.a1 = d1c / d0c;
        gainStage.a2 = d2c / d0c;
    }

    // tone: 0..1
    void setTone (double tone01) noexcept
    {
        toneAmount = std::clamp (tone01, 0.0, 1.0);
        const double fc = 720.0 * std::pow (3200.0 / 720.0, toneAmount);
        const double g = std::tan (3.14159265358979323846 * fc / fs);
        toneA = g / (1.0 + g);
    }

    inline float process (float input) noexcept
    {
        // 1) entrada + bloqueo de DC
        const double x = (double) input * kInputVolts;
        const double xd = x - dcX1 + dcR * dcY1;
        dcX1 = x;
        dcY1 = xd;

        // 2) etapa de ganancia + saturación del op-amp
        double v = gainStage.process (xd);
        v = kRailVolts * std::tanh (v / kRailVolts);

        // 3) recortador de diodos (R=1k, C=47nF, 1N914 x2 antiparalelo)
        v = clipDiodes (v);

        // 4) tono
        const double lpv = (v - toneState) * toneA;
        const double lp = lpv + toneState;
        toneState = lp + lpv;
        const double hp = v - lp;
        const double out = lp + toneAmount * 0.85 * hp;

        return (float) (out * kOutputGain);
    }

private:
    inline double clipDiodes (double vin) noexcept
    {
        constexpr double R  = 1.0e3;
        constexpr double C  = 47.0e-9;
        constexpr double Is = 2.52e-9;
        constexpr double nVt = 1.752 * 0.02585;

        const double T = 1.0 / fs;
        const double cT = C / T;

        double vc = vCap;
        for (int i = 0; i < 8; ++i)
        {
            const double e = vc / nVt;
            const double sh = std::sinh (e);
            const double ch = std::cosh (e);
            const double f  = cT * (vc - vCap) + 2.0 * Is * sh - (vin - vc) / R;
            const double df = cT + 2.0 * Is * ch / nVt + 1.0 / R;
            double dv = f / df;
            dv = std::clamp (dv, -0.15, 0.15);
            vc -= dv;
            vc = std::clamp (vc, -2.0, 2.0);
            if (std::abs (dv) < 1.0e-7)
                break;
        }

        vCap = vc;
        return vc;
    }

    double fs = 192000.0;
    Biquad gainStage;

    double dcR = 0.9995, dcX1 = 0.0, dcY1 = 0.0;
    double vCap = 0.0;
    double toneState = 0.0, toneA = 0.1, toneAmount = 0.5;
};
} // namespace gs
