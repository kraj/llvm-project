#!/bin/bash

# Script to compare ldp/stp instruction counts between two directories
# and find files where the count decreased with our patch

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <original_dir> <patched_dir>"
    echo "  Finds assembly files where ldp/stp count decreased in patched version"
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

# Temporary file to store results
RESULTS_FILE=$(mktemp)

echo "Scanning for assembly files in both directories..."

# Find all .s files in the original directory
find "$ORIGINAL_DIR" -type f -name "*.s" | while read -r orig_file; do
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
            echo "$diff $rel_path $orig_count $patched_count" >> "$RESULTS_FILE"
        fi
    fi
done

# Check if we found any regressions
if [ ! -s "$RESULTS_FILE" ]; then
    echo "No files found with decreased ldp/stp counts!"
    rm -f "$RESULTS_FILE"
    exit 0
fi

# Sort by difference (most negative first) and show top 10
echo ""
echo "Top 10 files with decreased ldp/stp instructions:"
echo "=================================================="
echo ""
printf "%-10s %-10s %-10s %-15s %s\n" "Decrease" "Original" "Patched" "Difference" "File"
printf "%-10s %-10s %-10s %-15s %s\n" "--------" "--------" "-------" "----------" "----"

# Sort by the difference (first column) and take top 10
sort -n "$RESULTS_FILE" | head -10 | while read -r diff rel_path orig_count patched_count; do
    # Make the difference positive for display
    abs_diff=$((-diff))
    percentage=""
    if [ $orig_count -gt 0 ]; then
        percentage=$(echo "scale=1; $diff * 100 / $orig_count" | bc)
        percentage=" (${percentage}%)"
    fi
    printf "%-10d %-10d %-10d %-15s %s\n" "$abs_diff" "$orig_count" "$patched_count" "${diff}${percentage}" "$rel_path"
done

echo ""
echo "Summary:"
echo "--------"
total_files=$(wc -l < "$RESULTS_FILE")
total_decrease=$(awk '{sum += $1} END {print -sum}' "$RESULTS_FILE")
total_orig=$(awk '{sum += $3} END {print sum}' "$RESULTS_FILE")
total_patched=$(awk '{sum += $4} END {print sum}' "$RESULTS_FILE")

echo "Total files with regressions: $total_files"
echo "Total ldp/stp in original: $total_orig"
echo "Total ldp/stp in patched: $total_patched"
echo "Total decrease: $total_decrease"

if [ $total_orig -gt 0 ]; then
    percentage=$(echo "scale=2; $total_decrease * 100 / $total_orig" | bc)
    echo "Overall decrease: ${percentage}%"
fi

# Clean up
rm -f "$RESULTS_FILE"