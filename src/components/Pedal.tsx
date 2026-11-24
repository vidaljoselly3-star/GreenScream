import { useState } from "react";
import Knob from "./Knob";

function Screw({ className }: { className: string }) {
  return (
    <span
      className={`absolute h-3.5 w-3.5 rounded-full bg-gradient-to-br from-zinc-200 to-zinc-600 shadow ${className}`}
    >
      <span className="absolute left-[15%] top-1/2 h-[2px] w-[70%] -rotate-12 bg-black/60" />
    </span>
  );
}

export default function Pedal() {
  const [drive, setDrive] = useState(5);
  const [tone, setTone] = useState(5);
  const [level, setLevel] = useState(6.7);
  const [bypass, setBypass] = useState(false);
  const active = !bypass;

  return (
    <div className="mx-auto w-full max-w-[460px] rounded-[28px] bg-gradient-to-b from-zinc-700 to-zinc-950 p-[18px] shadow-2xl shadow-black/70 ring-1 ring-white/10">
      <div
        className="relative overflow-hidden rounded-[22px] border-[3px] border-[#0d3c15] px-4 pb-5 pt-10 shadow-[0_6px_0_rgba(0,0,0,.5)]"
        style={{ background: "linear-gradient(135deg,#3fa64a,#1d6a2a)" }}
      >
        <div className="pointer-events-none absolute inset-x-0 top-0 h-40 bg-gradient-to-b from-white/20 to-transparent" />
        <Screw className="left-4 top-4" />
        <Screw className="right-4 top-4" />
        <Screw className="bottom-4 left-4" />
        <Screw className="bottom-4 right-4" />

        <div className="relative grid grid-cols-3 gap-1">
          <Knob label="DRIVE" value={drive} onChange={setDrive} defaultValue={5} />
          <Knob label="TONE" value={tone} onChange={setTone} defaultValue={5} />
          <Knob label="LEVEL" value={level} onChange={setLevel} defaultValue={6.7} />
        </div>

        <div className="relative mx-6 mt-6 rounded-xl bg-[#f4f1e6] p-1.5 shadow-[0_3px_0_rgba(0,0,0,.25)]">
          <div className="rounded-lg border border-[#1d6a2a] px-3 py-3 text-center">
            <div className="text-[40px] font-black italic leading-none text-[#17531f]">GreenScream</div>
            <div className="mt-1 text-[20px] italic leading-none text-[#17531f]/90">by Vidal</div>
            <div className="mt-3 text-[15px] font-bold tracking-[0.28em] text-[#c0392b]">OVERDRIVE OD-808</div>
            <div className="mt-1 text-[10px] tracking-wide text-[#17531f]/80">TUBE SCREAMER STYLE · 4x OVERSAMPLED</div>
          </div>
        </div>

        <div className="relative mt-6 flex flex-col items-center">
          <span
            className={`mb-3 h-4 w-4 rounded-full ring-[3px] ring-[#111] transition ${
              active
                ? "bg-gradient-to-br from-red-300 to-red-600 shadow-[0_0_24px_8px_rgba(255,64,64,.55)]"
                : "bg-gradient-to-br from-red-950 to-black"
            }`}
          />
          <button
            onClick={() => setBypass((b) => !b)}
            aria-label="Footswitch"
            className="group relative h-28 w-28 rounded-full bg-gradient-to-br from-zinc-100 to-zinc-600 shadow-[0_6px_6px_rgba(0,0,0,.5)] outline-none focus-visible:ring-4 focus-visible:ring-white/60"
          >
            <span className="absolute inset-[10%] rounded-full bg-gradient-to-b from-zinc-300 to-zinc-500 transition-transform group-active:translate-y-[3px]" />
            <span className="absolute inset-[20%] rounded-full bg-gradient-to-br from-zinc-100 to-zinc-500 ring-1 ring-white/40 transition-transform group-active:translate-y-[3px]" />
          </button>
          <span className="mt-2 text-xs font-bold tracking-wider text-white/90">{active ? "ON" : "BYPASS"}</span>
        </div>
      </div>
    </div>
  );
}
