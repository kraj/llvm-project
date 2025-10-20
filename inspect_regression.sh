#!/bin/bash

# Script to extract and compare specific patterns between original and patched assembly files
# Useful for investigating specific regressions found by the comparison scripts

if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <original_dir> <patched_dir> <relative_file_path> [context_lines]"
    echo ""
    echo "  Extracts and compares ldp/stp patterns between original and patched versions"
    echo "  Shows the actual instruction sequences that changed"
    echo ""
    echo "  original_dir:       Directory containing original assembly files"
    echo "  patched_dir:        Directory containing patched assembly files"
    echo "  relative_file_path: Path to the file relative to the directories"
    echo "  context_lines:      Number of context lines to show (default: 3)"
    echo ""
    echo "Example:"
    echo "  $0 /path/to/original /path/to/patched SingleSource/Benchmarks/test.s 5"
    exit 1
fi

ORIGINAL_DIR="$1"
PATCHED_DIR="$2"
REL_PATH="$3"
CONTEXT="${4:-3}"

ORIG_FILE="$ORIGINAL_DIR/$REL_PATH"
PATCHED_FILE="$PATCHED_DIR/$REL_PATH"

if [ ! -f "$ORIG_FILE" ]; then
    echo "Error: Original file not found: $ORIG_FILE"
    exit 1
fi

if [ ! -f "$PATCHED_FILE" ]; then
    echo "Error: Patched file not found: $PATCHED_FILE"
    exit 1
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "Analyzing: $REL_PATH"
echo "=========================================="
echo ""

# Count instructions
orig_ldp=$(grep -c '\bldp\b' "$ORIG_FILE" 2>/dev/null || echo 0)
orig_stp=$(grep -c '\bstp\b' "$ORIG_FILE" 2>/dev/null || echo 0)
orig_ldr=$(grep -c '\bldr\b' "$ORIG_FILE" 2>/dev/null || echo 0)
orig_str=$(grep -c '\bstr\b' "$ORIG_FILE" 2>/dev/null || echo 0)

patched_ldp=$(grep -c '\bldp\b' "$PATCHED_FILE" 2>/dev/null || echo 0)
patched_stp=$(grep -c '\bstp\b' "$PATCHED_FILE" 2>/dev/null || echo 0)
patched_ldr=$(grep -c '\bldr\b' "$PATCHED_FILE" 2>/dev/null || echo 0)
patched_str=$(grep -c '\bstr\b' "$PATCHED_FILE" 2>/dev/null || echo 0)

echo "Instruction Counts:"
echo "-------------------"
printf "%-12s %-10s %-10s %-10s\n" "Instruction" "Original" "Patched" "Change"
printf "%-12s %-10s %-10s %-10s\n" "-----------" "--------" "-------" "------"

ldp_diff=$((patched_ldp - orig_ldp))
stp_diff=$((patched_stp - orig_stp))
ldr_diff=$((patched_ldr - orig_ldr))
str_diff=$((patched_str - orig_str))

# Function to print with color based on change
print_count_row() {
    local name="$1"
    local orig="$2"
    local patched="$3"
    local diff="$4"

    if [ $diff -lt 0 ]; then
        color=$RED
        sign=""
    elif [ $diff -gt 0 ]; then
        color=$GREEN
        sign="+"
    else
        color=$NC
        sign=""
    fi

    printf "%-12s %-10d %-10d ${color}%-10s${NC}\n" "$name" "$orig" "$patched" "${sign}${diff}"
}

print_count_row "ldp" "$orig_ldp" "$patched_ldp" "$ldp_diff"
print_count_row "stp" "$orig_stp" "$patched_stp" "$stp_diff"
print_count_row "ldr" "$orig_ldr" "$patched_ldr" "$ldr_diff"
print_count_row "str" "$orig_str" "$patched_str" "$str_diff"

echo ""
echo "Pair Instructions (ldp+stp):"
total_orig=$((orig_ldp + orig_stp))
total_patched=$((patched_ldp + patched_stp))
total_diff=$((total_patched - total_orig))
print_count_row "TOTAL" "$total_orig" "$total_patched" "$total_diff"

# Find patterns that changed
echo ""
echo "Finding Changed Patterns:"
echo "========================="
echo ""

# Create temporary files for diff
TEMP_ORIG=$(mktemp)
TEMP_PATCHED=$(mktemp)

# Extract ldp/stp/ldr/str patterns with context
grep -n -B"$CONTEXT" -A"$CONTEXT" -E '\b(ldp|stp|ldr|str)\b' "$ORIG_FILE" > "$TEMP_ORIG"
grep -n -B"$CONTEXT" -A"$CONTEXT" -E '\b(ldp|stp|ldr|str)\b' "$PATCHED_FILE" > "$TEMP_PATCHED"

# Show specific examples of lost pairs
echo "Examples of Lost Pair Instructions (Original -> Patched):"
echo "----------------------------------------------------------"

# Find stp instructions in original that don't exist in patched
grep -n '\bstp\b' "$ORIG_FILE" | head -5 | while read -r line; do
    line_num=$(echo "$line" | cut -d: -f1)
    instruction=$(echo "$line" | cut -d: -f2-)

    # Check if this specific stp exists in patched
    if ! grep -q "$(echo "$instruction" | sed 's/[[\.*^$()+?{\\]/\\&/g')" "$PATCHED_FILE"; then
        echo ""
        echo -e "${YELLOW}Lost STP at line $line_num:${NC}"
        echo -e "${RED}Original:${NC}"
        sed -n "$((line_num-2)),$((line_num+2))p" "$ORIG_FILE" | while IFS= read -r l; do
            if echo "$l" | grep -q '\bstp\b'; then
                echo -e "  ${RED}> $l${NC}"
            else
                echo "    $l"
            fi
        done

        # Try to find corresponding area in patched file
        echo -e "${GREEN}Likely replacement in patched:${NC}"
        # Search for nearby context
        context_before=$(sed -n "$((line_num-5)),$((line_num-1))p" "$ORIG_FILE" | grep -v '^\s*$' | tail -1)
        if [ -n "$context_before" ]; then
            grep -n -A4 "$(echo "$context_before" | sed 's/[[\.*^$()+?{\\]/\\&/g')" "$PATCHED_FILE" 2>/dev/null | head -6 | while IFS= read -r l; do
                if echo "$l" | grep -q '\bstr\b'; then
                    echo -e "  ${GREEN}> $l${NC}"
                else
                    echo "    $l"
                fi
            done
        fi
    fi
done

# Similar for ldp
echo ""
echo "Examples of Lost LDP Instructions:"
echo "-----------------------------------"

grep -n '\bldp\b' "$ORIG_FILE" | head -5 | while read -r line; do
    line_num=$(echo "$line" | cut -d: -f1)
    instruction=$(echo "$line" | cut -d: -f2-)

    if ! grep -q "$(echo "$instruction" | sed 's/[[\.*^$()+?{\\]/\\&/g')" "$PATCHED_FILE"; then
        echo ""
        echo -e "${YELLOW}Lost LDP at line $line_num:${NC}"
        echo -e "${RED}Original:${NC}"
        sed -n "$((line_num-2)),$((line_num+2))p" "$ORIG_FILE" | while IFS= read -r l; do
            if echo "$l" | grep -q '\bldp\b'; then
                echo -e "  ${RED}> $l${NC}"
            else
                echo "    $l"
            fi
        done
    fi
done

# Look for specific patterns
echo ""
echo "Pattern Analysis:"
echo "-----------------"

# Check for broken pairs (e.g., two consecutive str that were likely stp)
echo ""
echo "Potential broken store pairs in patched file:"
grep -n -B1 '\bstr\b.*Folded Spill' "$PATCHED_FILE" | grep -A1 '\bstr\b.*Folded Spill' | head -20

echo ""
echo "Potential broken load pairs in patched file:"
grep -n -B1 '\bldr\b.*Folded Reload' "$PATCHED_FILE" | grep -A1 '\bldr\b.*Folded Reload' | head -20

# Clean up
rm -f "$TEMP_ORIG" "$TEMP_PATCHED"

echo ""
echo "=========================================="
echo "Analysis complete for: $REL_PATH"