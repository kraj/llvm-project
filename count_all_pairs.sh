#!/bin/bash

# Script to count total ldp/stp instructions across all files in two directories
# and show the overall change - using parallel processing for speed

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <original_dir> <patched_dir>"
    echo "  Counts total ldp/stp instructions in all assembly files"
    exit 1
fi

ORIGINAL_DIR="$1"
PATCHED_DIR="$2"

if [ ! -d "$ORIGINAL_DIR" ]; then
    echo "Error: Original directory '$ORIGINAL_DIR' does not exist"
    exit 1
fi

if [ ! -d "$PATCHED_DIR" ]; then
    echo "Error: Patched directory '$PATCHED_DIR' does not exist"
    exit 1
fi

echo "Counting all ldp/stp instructions in both directories..."
echo "Using parallel processing for speed..."
echo ""

# Count in original (using xargs for parallel processing)
echo "Counting in original directory..."
orig_ldp=$(find "$ORIGINAL_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bldp\b' 2>/dev/null | wc -l | tr -d '[:space:]')
orig_stp=$(find "$ORIGINAL_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bstp\b' 2>/dev/null | wc -l | tr -d '[:space:]')
orig_ldr=$(find "$ORIGINAL_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bldr\b' 2>/dev/null | wc -l | tr -d '[:space:]')
orig_str=$(find "$ORIGINAL_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bstr\b' 2>/dev/null | wc -l | tr -d '[:space:]')

# Count in patched
echo "Counting in patched directory..."
patched_ldp=$(find "$PATCHED_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bldp\b' 2>/dev/null | wc -l | tr -d '[:space:]')
patched_stp=$(find "$PATCHED_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bstp\b' 2>/dev/null | wc -l | tr -d '[:space:]')
patched_ldr=$(find "$PATCHED_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bldr\b' 2>/dev/null | wc -l | tr -d '[:space:]')
patched_str=$(find "$PATCHED_DIR" -name "*.s" -type f | xargs -P12 grep -h '\bstr\b' 2>/dev/null | wc -l | tr -d '[:space:]')

# Calculate totals
orig_total=$((orig_ldp + orig_stp))
patched_total=$((patched_ldp + patched_stp))

# Calculate differences
ldp_diff=$((patched_ldp - orig_ldp))
stp_diff=$((patched_stp - orig_stp))
ldr_diff=$((patched_ldr - orig_ldr))
str_diff=$((patched_str - orig_str))
total_diff=$((patched_total - orig_total))

echo ""
echo "Overall Instruction Counts Across ALL Files:"
echo "============================================="
printf "%-12s %12s %12s %12s %12s\n" "Instruction" "Original" "Patched" "Change" "Change %"
printf "%-12s %12s %12s %12s %12s\n" "-----------" "--------" "-------" "------" "--------"

# Function to print with percentage
print_row() {
    local name="$1"
    local orig="$2"
    local patched="$3"
    local diff="$4"

    local sign=""
    if [ $diff -gt 0 ]; then
        sign="+"
    fi

    local pct=""
    if [ $orig -gt 0 ]; then
        pct=$(awk -v d="$diff" -v o="$orig" 'BEGIN{printf "%.2f%%", d*100/o}')
    fi

    printf "%-12s %'12d %'12d %12s %12s\n" "$name" "$orig" "$patched" "${sign}${diff}" "$pct"
}

# Use printf with thousands separators if supported
LC_NUMERIC=en_US.UTF-8
print_row "ldp" "$orig_ldp" "$patched_ldp" "$ldp_diff"
print_row "stp" "$orig_stp" "$patched_stp" "$stp_diff"
print_row "ldr" "$orig_ldr" "$patched_ldr" "$ldr_diff"
print_row "str" "$orig_str" "$patched_str" "$str_diff"

echo ""
echo "------------  ------------ ------------ ------------ ------------"
print_row "ldp+stp" "$orig_total" "$patched_total" "$total_diff"

echo ""
echo "Analysis:"
echo "========="

if [ $total_diff -gt 0 ]; then
    echo "✓ Overall IMPROVEMENT: $(printf "%'d" $total_diff) more pair instructions"
    improvement_pct=$(awk -v d="$total_diff" -v o="$orig_total" 'BEGIN{printf "%.2f", d*100/o}')
    echo "  Improvement: ${improvement_pct}% increase in load/store pairs"
else
    echo "✗ Overall REGRESSION: $(printf "%'d" ${total_diff#-}) fewer pair instructions"
    regression_pct=$(awk -v d="${total_diff#-}" -v o="$orig_total" 'BEGIN{printf "%.2f", d*100/o}')
    echo "  Regression: ${regression_pct}% decrease in load/store pairs"
fi

# Expected conversion ratios
echo ""
echo "Conversion Patterns:"
if [ $ldp_diff -lt 0 ] && [ $ldr_diff -gt 0 ]; then
    ratio=$(awk -v l="${ldp_diff#-}" -v r="$ldr_diff" 'BEGIN{printf "%.2f", r/l}')
    echo "  - Each lost LDP became ~$ratio LDR instructions (expected: ~2.0)"
fi

if [ $stp_diff -lt 0 ] && [ $str_diff -gt 0 ]; then
    ratio=$(awk -v s="${stp_diff#-}" -v r="$str_diff" 'BEGIN{printf "%.2f", r/s}')
    echo "  - Each lost STP became ~$ratio STR instructions (expected: ~2.0)"
fi

# File count
echo ""
echo "File Statistics:"
orig_files=$(find "$ORIGINAL_DIR" -name "*.s" -type f | wc -l | tr -d '[:space:]')
patched_files=$(find "$PATCHED_DIR" -name "*.s" -type f | wc -l | tr -d '[:space:]')
echo "  Original directory: $orig_files assembly files"
echo "  Patched directory: $patched_files assembly files"