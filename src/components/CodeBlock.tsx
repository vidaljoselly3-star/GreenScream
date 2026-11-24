import { useState } from "react";

export default function CodeBlock({ code }: { code: string }) {
  const [copied, setCopied] = useState(false);
  const copy = async () => {
    try {
      await navigator.clipboard.writeText(code);
      setCopied(true);
      setTimeout(() => setCopied(false), 1500);
    } catch {
      /* ignore */
    }
  };
  return (
    <div className="relative mt-3 rounded-xl border border-white/10 bg-black/60">
      <button
        onClick={copy}
        className="absolute right-2 top-2 rounded-md bg-white/10 px-2 py-1 text-xs text-white/80 transition hover:bg-white/20"
      >
        {copied ? "¡Copiado!" : "Copiar"}
      </button>
      <pre className="overflow-x-auto p-4 pr-20 font-mono text-[13px] leading-relaxed text-emerald-200">{code}</pre>
    </div>
  );
}
