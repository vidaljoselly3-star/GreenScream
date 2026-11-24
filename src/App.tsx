import Pedal from "./components/Pedal";
import CodeBlock from "./components/CodeBlock";

const gitCommands = `git init
git add .
git commit -m "GreenScream by Vidal VST3"
git branch -M main
git remote add origin https://github.com/TU_USUARIO/greenscream-by-vidal.git
git push -u origin main`;

const tagCommands = `git tag v1.0.0
git push origin v1.0.0`;

const chain = [
  { t: "Entrada", d: "Bloqueo de DC (condensador de acoplo)." },
  { t: "Etapa de ganancia", d: "H(s) = 1 + Zf/Zg · (51k + Drive) ‖ 51 pF sobre 4,7 kΩ + 47 nF → joroba de medios ≈ 720 Hz." },
  { t: "Op-amp", d: "Saturación suave por raíles de ±4,5 V." },
  { t: "Diodos 1N914", d: "Par antiparalelo con R=1 kΩ y C=47 nF, resuelto muestra a muestra (Newton-Raphson)." },
  { t: "Tone", d: "Filtro de tono 720 Hz → 3,2 kHz con recuperación de agudos." },
  { t: "Level + Bypass", d: "−24 dB … +12 dB y bypass con crossfade, seco alineado por latencia." },
];

const controls = [
  ["Drive", "0–10", "Cantidad de saturación"],
  ["Tone", "0–10", "Oscuro ↔ brillante"],
  ["Level", "0–10", "Volumen de salida"],
  ["Footswitch", "On / Bypass", "Sin clics"],
];

export default function App() {
  return (
    <div className="min-h-screen bg-[#0b0d0c] text-zinc-200">
      <div className="pointer-events-none fixed inset-0 bg-[radial-gradient(ellipse_at_top,rgba(63,166,74,0.18),transparent_60%)]" />

      <header className="relative mx-auto flex max-w-6xl items-center justify-between px-6 py-6">
        <div className="flex items-center gap-3">
          <span className="grid h-9 w-9 place-items-center rounded-lg bg-gradient-to-br from-[#3fa64a] to-[#1d6a2a] text-lg font-black text-white shadow-lg">
            G
          </span>
          <span className="text-lg font-bold tracking-tight text-white">GreenScream by Vidal</span>
        </div>
        <span className="rounded-full border border-emerald-400/30 bg-emerald-400/10 px-3 py-1 text-xs font-medium text-emerald-300">
          VST3 · Windows · Reaper
        </span>
      </header>

      <main className="relative mx-auto max-w-6xl px-6 pb-24">
        {/* HERO */}
        <section className="grid items-center gap-12 py-10 lg:grid-cols-2">
          <div>
            <h1 className="text-4xl font-extrabold leading-tight tracking-tight text-white sm:text-5xl">
              El overdrive clásico de pedal verde,
              <span className="text-emerald-400"> como plugin VST3.</span>
            </h1>
            <p className="mt-5 max-w-xl text-lg text-zinc-400">
              Modelo de circuito del Tube Screamer TS808: etapa de op-amp, diodos 1N914 y control de tono, con
              sobremuestreo ×4. Solo tres potenciómetros y un footswitch, justo lo necesario para trabajar rápido en
              Reaper.
            </p>
            <div className="mt-8 flex flex-wrap gap-3">
              <a
                href="#github"
                className="rounded-xl bg-emerald-500 px-5 py-3 font-semibold text-black shadow-lg shadow-emerald-500/20 transition hover:bg-emerald-400"
              >
                Compilar en mi GitHub
              </a>
              <a
                href="#dsp"
                className="rounded-xl border border-white/15 px-5 py-3 font-semibold text-white transition hover:bg-white/5"
              >
                Cómo funciona
              </a>
            </div>
            <ul className="mt-8 grid max-w-xl grid-cols-2 gap-3 text-sm">
              {controls.map(([n, r, d]) => (
                <li key={n} className="rounded-xl border border-white/10 bg-white/[0.03] p-3">
                  <div className="flex items-baseline justify-between">
                    <span className="font-semibold text-white">{n}</span>
                    <span className="font-mono text-xs text-emerald-300">{r}</span>
                  </div>
                  <div className="mt-0.5 text-zinc-400">{d}</div>
                </li>
              ))}
            </ul>
          </div>

          <div>
            <Pedal />
            <p className="mt-4 text-center text-xs text-zinc-500">
              Vista previa interactiva de la interfaz · arrastra los potenciómetros · doble clic = valor por defecto
            </p>
          </div>
        </section>

        {/* DSP */}
        <section id="dsp" className="py-12">
          <h2 className="text-2xl font-bold text-white">Cadena de señal</h2>
          <p className="mt-2 text-zinc-400">Todo se calcula a 4× la frecuencia de muestreo del proyecto.</p>
          <ol className="mt-6 grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
            {chain.map((c, i) => (
              <li key={c.t} className="rounded-2xl border border-white/10 bg-white/[0.03] p-5">
                <span className="grid h-7 w-7 place-items-center rounded-full bg-emerald-500/20 text-sm font-bold text-emerald-300">
                  {i + 1}
                </span>
                <h3 className="mt-3 font-semibold text-white">{c.t}</h3>
                <p className="mt-1 text-sm text-zinc-400">{c.d}</p>
              </li>
            ))}
          </ol>
        </section>

        {/* GITHUB */}
        <section id="github" className="py-12">
          <h2 className="text-2xl font-bold text-white">Compilar el .vst3 en tu GitHub</h2>
          <p className="mt-2 max-w-3xl text-zinc-400">
            El proyecto incluye el código JUCE (<code className="text-emerald-300">plugin/</code>) y un workflow de GitHub
            Actions (<code className="text-emerald-300">.github/workflows/build-vst3.yml</code>) que compila el plugin en
            un servidor Windows. No necesitas instalar Visual Studio.
          </p>

          <div className="mt-8 grid gap-6 lg:grid-cols-2">
            <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-6">
              <h3 className="font-semibold text-white">1 · Sube el proyecto</h3>
              <p className="mt-1 text-sm text-zinc-400">
                Crea un repositorio vacío en GitHub y, desde la carpeta del proyecto, ejecuta:
              </p>
              <CodeBlock code={gitCommands} />
            </div>

            <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-6">
              <h3 className="font-semibold text-white">2 · Descarga el plugin</h3>
              <p className="mt-1 text-sm text-zinc-400">
                Pestaña <b>Actions</b> → <i>Build VST3 (Windows)</i> (5–10 min) → artefacto{" "}
                <b>GreenScream-by-Vidal-VST3-Windows</b>. Para publicar una Release con el zip:
              </p>
              <CodeBlock code={tagCommands} />
            </div>

            <div className="rounded-2xl border border-white/10 bg-white/[0.03] p-6 lg:col-span-2">
              <h3 className="font-semibold text-white">3 · Instala en Reaper</h3>
              <ol className="mt-3 list-decimal space-y-2 pl-5 text-sm text-zinc-300">
                <li>
                  Copia la carpeta <code className="text-emerald-300">GreenScream by Vidal.vst3</code> a{" "}
                  <code className="text-emerald-300">C:\Program Files\Common Files\VST3\</code>.
                </li>
                <li>
                  En Reaper: <b>Options → Preferences → Plug-ins → VST → Re-scan</b> (o “Clear cache and re-scan”).
                </li>
                <li>
                  Abre el FX de una pista y añade <b>VST3: GreenScream by Vidal (Vidal)</b>.
                </li>
              </ol>
            </div>
          </div>

          <div className="mt-6 rounded-2xl border border-amber-400/20 bg-amber-400/5 p-5 text-sm text-amber-100/90">
            <b>Nota:</b> el runtime de Visual C++ va enlazado estáticamente, así que el plugin funciona sin instalar
            nada más. Está construido con JUCE (AGPLv3 / licencia comercial). “Ibanez”, “Tube Screamer”, “TS808” y
            “UAD” son marcas de sus propietarios; este proyecto es un modelo independiente y sin afiliación.
          </div>
        </section>
      </main>

      <footer className="relative border-t border-white/10 py-6 text-center text-xs text-zinc-500">
        GreenScream by Vidal · VST3 overdrive estilo TS808
      </footer>
    </div>
  );
}
