#!/usr/bin/env bash
# bench_compare.sh — builds BenchmarkLoader at two commits and compares load times.
#
# Usage:
#   ./scripts/bench_compare.sh <repo-to-benchmark> [repo2 ...] [--runs N]
#
# Example:
#   ./scripts/bench_compare.sh ~/code/linux ~/code/MyProject --runs 7
#
# The script always builds the NEW commit from the current working tree.
# The OLD commit is checked out into a temporary git worktree so neither
# working tree is modified.

set -euo pipefail

OLD_SHA="b4d700de1d42571db6b90c747bd088045298fd65"
NEW_SHA="69853cd810d9d98267baee6ce04ba87bcb43bf62"

ROOT="$(git -C "$(dirname "$0")" rev-parse --show-toplevel)"
WORKTREE="/tmp/gitqlient-bench-${OLD_SHA:0:8}"
BUILD_NEW="$ROOT/build-bench-release"
BUILD_OLD="$WORKTREE/build-bench-release"
TARGET="BenchmarkLoader"
NCPU="$(sysctl -n hw.logicalcpu 2>/dev/null || nproc 2>/dev/null || echo 4)"

# Forward all arguments to the benchmark binaries.
BENCH_ARGS=("$@")

die() { echo "ERROR: $*" >&2; exit 1; }

require_cmd() { command -v "$1" >/dev/null 2>&1 || die "'$1' not found in PATH"; }
require_cmd git
require_cmd cmake
GENERATOR="Unix Makefiles"
command -v ninja >/dev/null 2>&1 && GENERATOR="Ninja"

# ---------------------------------------------------------------------------
# 1. Build NEW commit (current working tree)
# ---------------------------------------------------------------------------
echo "==> Building NEW ($NEW_SHA) at Release…"
cmake -S "$ROOT" -B "$BUILD_NEW" \
      -DCMAKE_BUILD_TYPE=Release \
      -DEXCLUDE_TESTS=OFF \
      -G "$GENERATOR" \
      -Wno-dev >/dev/null 2>&1

cmake --build "$BUILD_NEW" --target "$TARGET" -j"$NCPU" 2>&1 \
    | grep -E "^\[|error:|warning:" | tail -20

NEW_BIN="$BUILD_NEW/tests/Benchmarks/$TARGET"
[ -f "$NEW_BIN" ] || die "New binary not found at $NEW_BIN"

# ---------------------------------------------------------------------------
# 2. Set up OLD worktree
# ---------------------------------------------------------------------------
echo "==> Setting up OLD worktree ($OLD_SHA) at $WORKTREE…"
if [ -d "$WORKTREE" ]; then
    echo "    (worktree already exists, reusing)"
else
    git worktree add "$WORKTREE" "$OLD_SHA"
fi

# Copy the benchmark files — they don't exist at the old commit.
cp -r "$ROOT/tests/Benchmarks" "$WORKTREE/tests/"

# Inject add_subdirectory if not already present.
if ! grep -q "Benchmarks" "$WORKTREE/tests/CMakeLists.txt"; then
    echo 'add_subdirectory(Benchmarks)' >> "$WORKTREE/tests/CMakeLists.txt"
fi

# ---------------------------------------------------------------------------
# 3. Build OLD commit
# ---------------------------------------------------------------------------
echo "==> Building OLD ($OLD_SHA) at Release…"
cmake -S "$WORKTREE" -B "$BUILD_OLD" \
      -DCMAKE_BUILD_TYPE=Release \
      -DEXCLUDE_TESTS=OFF \
      -G "$GENERATOR" \
      -Wno-dev >/dev/null 2>&1

cmake --build "$BUILD_OLD" --target "$TARGET" -j"$NCPU" 2>&1 \
    | grep -E "^\[|error:|warning:" | tail -20

OLD_BIN="$BUILD_OLD/tests/Benchmarks/$TARGET"
[ -f "$OLD_BIN" ] || die "Old binary not found at $OLD_BIN"

# ---------------------------------------------------------------------------
# 4. Run benchmarks in parallel
# ---------------------------------------------------------------------------
OUT_OLD="$(mktemp /tmp/bench-old-XXXXXX)"
OUT_NEW="$(mktemp /tmp/bench-new-XXXXXX)"
trap 'rm -f "$OUT_OLD" "$OUT_NEW"' EXIT

echo ""
echo "==> Running OLD and NEW in parallel…"
"$OLD_BIN" "${BENCH_ARGS[@]}" >"$OUT_OLD" 2>&1 &
PID_OLD=$!
"$NEW_BIN" "${BENCH_ARGS[@]}" >"$OUT_NEW" 2>&1 &
PID_NEW=$!

wait "$PID_OLD" || die "OLD benchmark failed"
wait "$PID_NEW" || die "NEW benchmark failed"

SEP="════════════════════════════════════════════════════════════"

echo ""
echo "$SEP"
echo "  OLD  $OLD_SHA"
echo "$SEP"
cat "$OUT_OLD"

echo ""
echo "$SEP"
echo "  NEW  $NEW_SHA"
echo "$SEP"
cat "$OUT_NEW"

echo ""
echo "Done."
echo "To clean up the worktree: git worktree remove --force $WORKTREE"
