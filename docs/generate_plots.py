#!/usr/bin/env python3

from __future__ import annotations

import re
import subprocess
import tempfile
from pathlib import Path

import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parents[1]
SIMULATOR = ROOT / "simulator"
DOCS = ROOT / "docs"
TRACE_LINES = []


def build_benchmark_trace() -> str:
    lines = []

    # Phase 1: a working set that grows large enough to show the cache-size curve.
    for _ in range(4):
        for block in range(128):
            address = block * 64
            op = "R" if block % 2 == 0 else "W"
            lines.append(f"{op} 0x{address:08X}")

    # Phase 2: eight addresses that intentionally conflict in the same set for the
    # larger associativity experiment.
    for _ in range(64):
        for lane in range(8):
            address = lane * 0x2000
            op = "R" if lane % 2 == 0 else "W"
            lines.append(f"{op} 0x{address:08X}")

    return "\n".join(lines) + "\n"


def run_simulator(trace_path: Path, cache_size: int, associativity: int, block_size: int) -> float:
    result = subprocess.run(
        [
            str(SIMULATOR),
            str(trace_path),
            str(cache_size),
            str(associativity),
            str(block_size),
        ],
        cwd=ROOT,
        capture_output=True,
        text=True,
        check=True,
    )

    match = re.search(r"Hit Rate:\s+([0-9.]+)%", result.stdout)
    if not match:
        raise RuntimeError(f"Could not parse hit rate from output:\n{result.stdout}\n{result.stderr}")

    return float(match.group(1))


def main() -> None:
    if not SIMULATOR.exists():
        raise SystemExit("Build the simulator first with `make`.")

    DOCS.mkdir(exist_ok=True)

    trace_text = build_benchmark_trace()
    with tempfile.NamedTemporaryFile("w", delete=False, suffix=".trace", dir=ROOT) as handle:
        handle.write(trace_text)
        trace_path = Path(handle.name)

    try:
        cache_sizes = [4096, 8192, 16384, 32768]
        curve_hits = [run_simulator(trace_path, size, 4, 64) for size in cache_sizes]

        associativities = [1, 2, 4, 8]
        assoc_hits = [run_simulator(trace_path, 32768, assoc, 64) for assoc in associativities]

        plt.figure(figsize=(8, 5))
        plt.plot(cache_sizes, curve_hits, marker="o", linewidth=2)
        plt.xticks(cache_sizes, [f"{size // 1024}KB" for size in cache_sizes])
        plt.ylim(0, 100)
        plt.xlabel("Cache Size")
        plt.ylabel("Hit Rate (%)")
        plt.title("Miss Rate Curve")
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(DOCS / "miss-rate-curve.png", dpi=160)
        plt.close()

        plt.figure(figsize=(8, 5))
        plt.bar([f"{assoc}-way" for assoc in associativities], assoc_hits, color="#4C78A8")
        plt.ylim(0, 100)
        plt.xlabel("Associativity")
        plt.ylabel("Hit Rate (%)")
        plt.title("Associativity Impact")
        plt.grid(axis="y", alpha=0.3)
        plt.tight_layout()
        plt.savefig(DOCS / "associativity-impact.png", dpi=160)
        plt.close()

        print("Generated docs/miss-rate-curve.png and docs/associativity-impact.png")
    finally:
        trace_path.unlink(missing_ok=True)


if __name__ == "__main__":
    main()