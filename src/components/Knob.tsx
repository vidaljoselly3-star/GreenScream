import { useRef } from "react";

type Props = {
  label: string;
  value: number;
  onChange: (v: number) => void;
  defaultValue: number;
  size?: number;
};

const START = -135;
const END = 135;

function polar(cx: number, cy: number, r: number, deg: number) {
  const a = (deg * Math.PI) / 180;
  return { x: cx + r * Math.sin(a), y: cy - r * Math.cos(a) };
}

export default function Knob({ label, value, onChange, defaultValue, size = 140 }: Props) {
  const drag = useRef<{ y: number; v: number } | null>(null);
  const c = size / 2;
  const R = size / 2;
  const angle = START + (value / 10) * (END - START);

  const onPointerDown = (e: React.PointerEvent) => {
    (e.target as Element).setPointerCapture(e.pointerId);
    drag.current = { y: e.clientY, v: value };
  };
  const onPointerMove = (e: React.PointerEvent) => {
    if (!drag.current) return;
    const dv = ((drag.current.y - e.clientY) / 180) * 10;
    onChange(Math.min(10, Math.max(0, drag.current.v + dv)));
  };
  const onPointerUp = () => {
    drag.current = null;
  };
  const onWheel = (e: React.WheelEvent) => {
    onChange(Math.min(10, Math.max(0, value - Math.sign(e.deltaY) * 0.3)));
  };

  const kr = R * 0.56;
  const br = kr * 0.9;
  const tr = br * 0.72;
  const id = `k-${label}`;

  return (
    <div className="flex flex-col items-center select-none">
      <svg
        width={size}
        height={size}
        viewBox={`0 0 ${size} ${size}`}
        className="cursor-ns-resize touch-none"
        onPointerDown={onPointerDown}
        onPointerMove={onPointerMove}
        onPointerUp={onPointerUp}
        onDoubleClick={() => onChange(defaultValue)}
        onWheel={onWheel}
        role="slider"
        aria-label={label}
        aria-valuemin={0}
        aria-valuemax={10}
        aria-valuenow={Number(value.toFixed(1))}
      >
        <defs>
          <linearGradient id={`${id}-ring`} x1="0" y1="0" x2="1" y2="1">
            <stop offset="0" stopColor="#6a6a6a" />
            <stop offset="1" stopColor="#101010" />
          </linearGradient>
          <radialGradient id={`${id}-body`} cx="0.35" cy="0.3" r="0.9">
            <stop offset="0" stopColor="#3b3b3b" />
            <stop offset="1" stopColor="#0b0b0b" />
          </radialGradient>
          <linearGradient id={`${id}-cap`} x1="0" y1="0" x2="0" y2="1">
            <stop offset="0" stopColor="#2c2c2c" />
            <stop offset="1" stopColor="#050505" />
          </linearGradient>
        </defs>

        {Array.from({ length: 11 }).map((_, i) => {
          const a = START + (i / 10) * (END - START);
          const major = i % 5 === 0;
          const p1 = polar(c, c, R * 0.7, a);
          const p2 = polar(c, c, R * (major ? 0.84 : 0.8), a);
          const t = polar(c, c, R * 0.95, a);
          return (
            <g key={i}>
              <line
                x1={p1.x}
                y1={p1.y}
                x2={p2.x}
                y2={p2.y}
                stroke="white"
                strokeOpacity={major ? 0.95 : 0.7}
                strokeWidth={major ? 2 : 1.4}
              />
              <text
                x={t.x}
                y={t.y}
                fill="white"
                fontSize={R * 0.17}
                fontWeight={700}
                textAnchor="middle"
                dominantBaseline="central"
              >
                {i}
              </text>
            </g>
          );
        })}

        <circle cx={c + 2} cy={c + 4} r={kr} fill="black" opacity={0.45} />
        <circle cx={c} cy={c} r={kr} fill={`url(#${id}-ring)`} />
        <circle cx={c} cy={c} r={br} fill={`url(#${id}-body)`} />
        <circle cx={c} cy={c} r={br} fill="none" stroke="white" strokeOpacity={0.07} strokeWidth={br * 0.1} strokeDasharray="1.5 3" />
        <circle cx={c} cy={c} r={tr} fill={`url(#${id}-cap)`} />
        <circle cx={c} cy={c} r={tr} fill="none" stroke="white" strokeOpacity={0.12} />
        <g transform={`rotate(${angle} ${c} ${c})`}>
          <rect x={c - 1.8} y={c - br * 0.92} width={3.6} height={br * 0.55} rx={1.5} fill="white" />
        </g>
      </svg>
      <span className="-mt-1 text-xl font-extrabold tracking-wide text-white">{label}</span>
      <span className="text-[11px] font-mono text-white/70">{value.toFixed(1)}</span>
    </div>
  );
}
