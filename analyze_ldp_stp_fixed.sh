#!/bin/bash

# Enhanced script to compare ldp/stp instruction counts between two directories
# with detailed analysis and pattern matching

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <original_dir> <patched_dir>"
    echo "  Finds assembly files where ldp/stp count decreased in patched version"
    echo "  Also analyzes patterns of what changed (e.g., stp->str, ldp->ldr)"
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

# Temporary files
RESULTS_FILE=$(mktemp)
DETAILS_FILE=$(mktemp)
FILES_LIST=$(mktemp)

echo "Scanning for assembly files in both directories..."

# Function to analyze what changed between files
analyze_changes() {
    local orig_file="$1"
    local patched_file="$2"

    # Count different instruction types
    local orig_ldp=$(grep -c -E '\bldp\b' "$orig_file" 2>/dev/null || echo 0)
    local orig_stp=$(grep -c -E '\bstp\b' "$orig_file" 2>/dev/null || echo 0)
    local orig_ldr=$(grep -c -E '\bldr\b' "$orig_file" 2>/dev/null || echo 0)
    local orig_str=$(grep -c -E '\bstr\b' "$orig_file" 2>/dev/null || echo 0)

    local patched_ldp=$(grep -c -E '\bldp\b' "$patched_file" 2>/dev/null || echo 0)
    local patched_stp=$(grep -c -E '\bstp\b' "$patched_file" 2>/dev/null || echo 0)
    local patched_ldr=$(grep -c -E '\bldr\b' "$patched_file" 2>/dev/null || echo 0)
    local patched_str=$(grep -c -E '\bstr\b' "$patched_file" 2>/dev/null || echo 0)

    local ldp_diff=$((patched_ldp - orig_ldp))
    local stp_diff=$((patched_stp - orig_stp))
    local ldr_diff=$((patched_ldr - orig_ldr))
    local str_diff=$((patched_str - orig_str))

    echo "${orig_ldp}|${patched_ldp}|${ldp_diff}|${orig_stp}|${patched_stp}|${stp_diff}|${orig_ldr}|${patched_ldr}|${ldr_diff}|${orig_str}|${patched_str}|${str_diff}"
}

# Find all .s files in the original directory
find "$ORIGINAL_DIR" -type f -name "*.s" > "$FILES_LIST"

total_files=$(wc -l < "$FILES_LIST")
processed_files=0
found_regressions=0

echo "  Found $total_files assembly files to check..."

while IFS= read -r orig_file; do
    # Get the relative path from the original directory
    rel_path="${orig_file#$ORIGINAL_DIR/}"

    # Check if the same file exists in the patched directory
    patched_file="$PATCHED_DIR/$rel_path"

    if [ -f "$patched_file" ]; then
        # Count ldp and stp instructions in both files
        orig_count=$(grep -c -E '\b(ldp|stp)\b' "$orig_file" 2>/dev/null || echo 0)
        patched_count=$(grep -c -E '\b(ldp|stp)\b' "$patched_file" 2>/dev/null || echo 0)

        # Calculate the difference (negative means decrease)
        diff=$((patched_count - orig_count))

        # Only record if the count decreased
        if [ $diff -lt 0 ]; then
            # Get detailed analysis
            details=$(analyze_changes "$orig_file" "$patched_file")
            echo "${diff}|${rel_path}|${orig_count}|${patched_count}|${details}" >> "$RESULTS_FILE"
            found_regressions=$((found_regressions + 1))
        fi

        processed_files=$((processed_files + 1))
        if [ $((processed_files % 100)) -eq 0 ]; then
            echo "  Processed $processed_files/$total_files files... ($found_regressions regressions found so far)"
        fi
    fi
done < "$FILES_LIST"

echo "  Total files processed: $processed_files"
echo "  Files with regressions: $found_regressions"

# Clean up temp file
rm -f "$FILES_LIST"

# Check if we found any regressions
if [ ! -s "$RESULTS_FILE" ]; then
    echo "No files found with decreased ldp/stp counts!"
    rm -f "$RESULTS_FILE" "$DETAILS_FILE"
    exit 0
fi

# Sort by difference (most negative first) and show top 10
echo ""
echo "Top 10 files with decreased ldp/stp instructions:"
echo "================================================================================="
echo ""
printf "%-8s %-8s %-8s %-12s %s\n" "Decrease" "Orig" "Patched" "Change %" "File"
printf "%-8s %-8s %-8s %-12s %s\n" "--------" "----" "-------" "--------" "----"

# Sort by the difference (first column) and take top 10
sort -t'|' -n -k1 "$RESULTS_FILE" | head -10 | while IFS='|' read -r diff rel_path orig_count patched_count orig_ldp patched_ldp ldp_diff orig_stp patched_stp stp_diff orig_ldr patched_ldr ldr_diff orig_str patched_str str_diff; do
    # Make the difference positive for display
    abs_diff=$((-diff))
    percentage=""
    if [ "$orig_count" -gt 0 ]; then
        # Calculate percentage using awk instead of bc
        percentage=$(awk -v d="$diff" -v o="$orig_count" 'BEGIN{printf "%.1f%%", d*100/o}')
    fi
    printf "%-8d %-8d %-8d %-12s %s\n" "$abs_diff" "$orig_count" "$patched_count" "$percentage" "$rel_path"

    # Save details for later analysis
    echo "${rel_path}|${orig_ldp}|${patched_ldp}|${ldp_diff}|${orig_stp}|${patched_stp}|${stp_diff}|${orig_ldr}|${patched_ldr}|${ldr_diff}|${orig_str}|${patched_str}|${str_diff}" >> "$DETAILS_FILE"
done

echo ""
echo "Detailed Analysis of Top 10 Regressions:"
echo "========================================="
echo ""

# Analyze patterns in top 10
if [ -f "$DETAILS_FILE" ] && [ -s "$DETAILS_FILE" ]; then
    while IFS='|' read -r rel_path orig_ldp patched_ldp ldp_diff orig_stp patched_stp stp_diff orig_ldr patched_ldr ldr_diff orig_str patched_str str_diff; do
        echo "File: $rel_path"
        echo "  LDP: $orig_ldp -> $patched_ldp (change: $ldp_diff)"
        echo "  STP: $orig_stp -> $patched_stp (change: $stp_diff)"
        echo "  LDR: $orig_ldr -> $patched_ldr (change: $ldr_diff)"
        echo "  STR: $orig_str -> $patched_str (change: $str_diff)"

        # Analyze patterns
        if [ "$ldp_diff" -lt 0 ] && [ "$ldr_diff" -gt 0 ]; then
            echo "  Pattern: LDP instructions replaced by LDR (lost pairing)"
        fi
        if [ "$stp_diff" -lt 0 ] && [ "$str_diff" -gt 0 ]; then
            echo "  Pattern: STP instructions replaced by STR (lost pairing)"
        fi
        echo ""
    done < "$DETAILS_FILE"
fi

echo "Summary Statistics:"
echo "==================="
total_files=$(wc -l < "$RESULTS_FILE")
total_decrease=$(awk -F'|' '{sum += $1} END {print -sum}' "$RESULTS_FILE")
total_orig=$(awk -F'|' '{sum += $3} END {print sum}' "$RESULTS_FILE")
total_patched=$(awk -F'|' '{sum += $4} END {print sum}' "$RESULTS_FILE")

echo "Total files with regressions: $total_files"
echo "Total ldp/stp in original: $total_orig"
echo "Total ldp/stp in patched: $total_patched"
echo "Total decrease: $total_decrease"

if [ "$total_orig" -gt 0 ]; then
    percentage=$(awk -v d="$total_decrease" -v o="$total_orig" 'BEGIN{printf "%.2f", d*100/o}')
    echo "Overall decrease: ${percentage}%"
fi

# Analyze overall patterns
echo ""
echo "Overall Pattern Analysis:"
echo "========================="

total_ldp_decrease=0
total_stp_decrease=0
total_ldr_increase=0
total_str_increase=0

while IFS='|' read -r diff rel_path orig_count patched_count orig_ldp patched_ldp ldp_diff orig_stp patched_stp stp_diff orig_ldr patched_ldr ldr_diff orig_str patched_str str_diff; do
    if [ -n "$ldp_diff" ]; then
        if [ "$ldp_diff" -lt 0 ]; then
            total_ldp_decrease=$((total_ldp_decrease - ldp_diff))
        fi
        if [ "$stp_diff" -lt 0 ]; then
            total_stp_decrease=$((total_stp_decrease - stp_diff))
        fi
        if [ "$ldr_diff" -gt 0 ]; then
            total_ldr_increase=$((total_ldr_increase + ldr_diff))
        fi
        if [ "$str_diff" -gt 0 ]; then
            total_str_increase=$((total_str_increase + str_diff))
        fi
    fi
done < "$RESULTS_FILE"

echo "Total LDP instructions lost: $total_ldp_decrease"
echo "Total STP instructions lost: $total_stp_decrease"
echo "Total LDR instructions gained: $total_ldr_increase"
echo "Total STR instructions gained: $total_str_increase"

if [ "$total_ldp_decrease" -gt 0 ] || [ "$total_stp_decrease" -gt 0 ]; then
    echo ""
    echo "Conversion analysis:"
    if [ "$total_ldp_decrease" -gt 0 ] && [ "$total_ldr_increase" -gt 0 ]; then
        ratio=$(awk -v l="$total_ldp_decrease" -v r="$total_ldr_increase" 'BEGIN{printf "%.1f", r/(l*2)}')
        echo "  - Approx $ratio LDR instructions per lost LDP (expected: 1.0)"
    fi
    if [ "$total_stp_decrease" -gt 0 ] && [ "$total_str_increase" -gt 0 ]; then
        ratio=$(awk -v s="$total_stp_decrease" -v r="$total_str_increase" 'BEGIN{printf "%.1f", r/(s*2)}')
        echo "  - Approx $ratio STR instructions per lost STP (expected: 1.0)"
    fi
fi

# Clean up
rm -f "$RESULTS_FILE" "$DETAILS_FILE"