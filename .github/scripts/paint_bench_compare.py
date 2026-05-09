#!/usr/bin/env python3
"""Compare master-vs-PR PaintTelemetry runs and emit a markdown summary.

Filename convention: `<branch>-<workload>-<n>.json`,
branch ∈ {master, pr}, workload ∈ {throughput, scroll}.
"""
from __future__ import annotations

import argparse
import glob
import json
import os
import re
import statistics
import sys
from dataclasses import dataclass


# (pct_threshold, absolute_floor, unit). Both bounds must be exceeded to flag,
# so sub-noise jitter near the floor doesn't trip the gate.
RULES = {
    "throughput.avg_us_per_row": (0.20, 50.0, "µs"),
    "throughput.max_ms_per_row": (0.30, 0.5, "ms"),
    "throughput.slow_rows":      (0.0, 1.0, "rows"),
    "scroll.frame_p50_ms":       (0.25, 1.0, "ms"),
    "scroll.frame_p95_ms":       (0.25, 1.0, "ms"),
    "scroll.frame_p99_ms":       (0.30, 2.0, "ms"),
    "scroll.frame_max_ms":       (0.30, 2.0, "ms"),
    "scroll.slow_rows":          (0.0, 1.0, "rows"),
}


@dataclass
class Sample:
    workload: str
    branch: str
    metrics: dict


FILENAME_RE = re.compile(r"^(?P<branch>master|pr)-(?P<workload>throughput|scroll)-\d+\.json$")


def load_samples(directory: str) -> list[Sample]:
    out = []
    for path in sorted(glob.glob(os.path.join(directory, "*.json"))):
        m = FILENAME_RE.match(os.path.basename(path))
        if not m:
            continue
        with open(path) as f:
            obj = json.load(f)
        out.append(Sample(branch=m.group("branch"), workload=m.group("workload"), metrics=obj))
    return out


def median_metrics(samples: list[Sample]) -> dict:
    """Per-key median; string fields fall back to the first sample's value."""
    if not samples:
        return {}
    keys = samples[0].metrics.keys()
    out = {}
    for k in keys:
        vals = [s.metrics.get(k) for s in samples]
        if all(isinstance(v, (int, float)) and not isinstance(v, bool) for v in vals):
            out[k] = statistics.median(vals)
        else:
            out[k] = vals[0]
    return out


def regressed(master: float, pr: float, rule: tuple[float, float, str]) -> bool:
    pct, floor, _ = rule
    if master <= 0:
        return pr > floor
    delta_pct = (pr - master) / master
    delta_abs = pr - master
    return delta_pct > pct and delta_abs > floor


def fmt_pct(master: float, pr: float) -> str:
    if master <= 0:
        return "n/a" if pr == 0 else f"+inf"
    delta = (pr - master) / master * 100.0
    sign = "+" if delta >= 0 else ""
    return f"{sign}{delta:.1f}%"


def render(master: dict, pr: dict, os_label: str) -> tuple[str, bool]:
    rows = []
    any_regression = False

    rows.append(f"### {os_label}")
    rows.append("")
    rows.append("| workload | metric | master | PR | Δ |")
    rows.append("|---|---|---|---|---|")

    for full_key, rule in RULES.items():
        workload, metric = full_key.split(".")
        m_obj = master.get(workload, {})
        p_obj = pr.get(workload, {})
        if not m_obj or not p_obj:
            continue
        mv = m_obj.get(metric)
        pv = p_obj.get(metric)
        if mv is None or pv is None:
            continue

        flag = regressed(mv, pv, rule)
        any_regression = any_regression or flag
        marker = " ⚠️" if flag else ""

        unit = rule[2]
        rows.append(
            f"| {workload} | {metric} | {mv:.2f} {unit} | {pv:.2f} {unit} | {fmt_pct(mv, pv)}{marker} |"
        )

    return "\n".join(rows) + "\n", any_regression


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("directory", help="Directory of <branch>-<workload>-<n>.json files")
    ap.add_argument("--os-label", default=os.environ.get("RUNNER_OS", "linux"))
    ap.add_argument("--strict", action="store_true",
                    help="Exit 1 on any regression (default: print only)")
    args = ap.parse_args()

    samples = load_samples(args.directory)
    if not samples:
        print(f"No JSON samples found under {args.directory}", file=sys.stderr)
        return 2

    grouped: dict[tuple[str, str], list[Sample]] = {}
    for s in samples:
        grouped.setdefault((s.branch, s.workload), []).append(s)

    master = {wl: median_metrics(grouped.get(("master", wl), [])) for wl in ("throughput", "scroll")}
    pr     = {wl: median_metrics(grouped.get(("pr",     wl), [])) for wl in ("throughput", "scroll")}

    body, regressed_any = render(master, pr, args.os_label)
    print(body)

    if regressed_any:
        print("\n_One or more metrics regressed past the threshold._")
        if args.strict:
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
