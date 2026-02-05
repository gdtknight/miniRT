#!/bin/bash
# Baseline benchmark runner for miniRT
# Usage: ./scripts/bench.sh [scene_file] [num_trials]
#
# Runs the specified scene (or all perf scenes), captures stdout metrics,
# and saves to /tmp/miniRT_bench_*.txt
#
# After the window opens and the first full-quality render completes,
# press ESC to close. The script captures stdout output.

set -euo pipefail

MINIRT="./miniRT"
OUTDIR="/tmp/miniRT_bench"
TRIALS="${2:-3}"

SCENES=(
	"scenes/perf/perf_timing.rt"
	"scenes/perf/perf_spheres_20.rt"
	"scenes/perf/perf_spheres_50.rt"
	"scenes/perf/perf_all_objects.rt"
)

if [ ! -x "$MINIRT" ]; then
	echo "Error: $MINIRT not found. Run 'make' first."
	exit 1
fi

mkdir -p "$OUTDIR"

if [ "${1:-}" != "" ]; then
	SCENES=("$1")
fi

for scene in "${SCENES[@]}"; do
	name=$(basename "$scene" .rt)
	echo ""
	echo "=== Scene: $name ==="
	echo "File: $scene"
	echo ""
	for trial in $(seq 1 "$TRIALS"); do
		outfile="${OUTDIR}/${name}_trial${trial}.txt"
		echo "Trial $trial/$TRIALS: Press ESC after render completes..."
		$MINIRT "$scene" > "$outfile" 2>&1
		echo "  Saved to $outfile"
		if [ -s "$outfile" ]; then
			echo "  --- Output preview ---"
			cat "$outfile"
			echo "  ----------------------"
		else
			echo "  (no output captured)"
		fi
	done
done

echo ""
echo "All results saved to $OUTDIR/"
echo "Files:"
ls -la "$OUTDIR/"
