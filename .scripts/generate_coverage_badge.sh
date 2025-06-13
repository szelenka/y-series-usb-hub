#!/bin/bash

# Path to the coverage HTML file
COVERAGE_HTML=".coverage/index.html"
BADGE_FILE="coverage_badge.svg"

# Check if the coverage file exists
if [ ! -f "$COVERAGE_HTML" ]; then
    echo "Error: Coverage file not found at $COVERAGE_HTML"
    exit 1
fi

# Extract the coverage percentage from the HTML
COVERAGE_PERCENT=$(grep -A 5 "Lines:" "$COVERAGE_HTML" | grep -E 'headerCovTableEntryHi' | head -1 | sed -E 's/.*>([0-9.]+) %<.*/\1/')

# If first method fails, try alternative approach
if [ -z "$COVERAGE_PERCENT" ] || ! echo "$COVERAGE_PERCENT" | grep -q '^[0-9.]*$'; then
    # Look for the line with the coverage percentage
    COVERAGE_PERCENT=$(grep -A 5 "Lines:" "$COVERAGE_HTML" | grep -A 3 "headerCovTableEntry" | tail -1 | sed -E 's/.*>([0-9.]+).*/\1/')
    
    # Final check
    if [ -z "$COVERAGE_PERCENT" ] || ! echo "$COVERAGE_PERCENT" | grep -q '^[0-9.]*$'; then
        echo "Error: Could not extract coverage percentage from $COVERAGE_HTML"
        echo "Here's the relevant part of the file for debugging:"
        grep -A 10 "Lines:" "$COVERAGE_HTML"
        exit 1
    fi
fi

# Round the percentage to nearest integer for display
ROUNDED_PERCENT=$(printf "%.0f" "$COVERAGE_PERCENT")

# Determine badge color based on coverage percentage
if (( $(echo "$COVERAGE_PERCENT < 50" | bc -l) )); then
    COLOR="e05d44"  # red
elif (( $(echo "$COVERAGE_PERCENT < 80" | bc -l) )); then
    COLOR="dfb317"  # yellow
else
    COLOR="4c1"     # brightgreen
fi

# Create the SVG badge
cat > "$BADGE_FILE" << EOF
<svg xmlns="http://www.w3.org/2000/svg" width="120" height="20">
  <linearGradient id="b" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <mask id="a">
    <rect width="120" height="20" rx="3" fill="#fff"/>
  </mask>
  <g mask="url(#a)">
    <path fill="#555" d="M0 0h67v20H0z"/>
    <path fill="#${COLOR}" d="M67 0h53v20H67z"/>
    <path fill="url(#b)" d="M0 0h120v20H0z"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="DejaVu Sans,Verdana,Geneva,sans-serif" font-size="11">
    <text x="33.5" y="15" fill="#010101" fill-opacity=".3">coverage</text>
    <text x="33.5" y="14">coverage</text>
    <text x="92.5" y="15" fill="#010101" fill-opacity=".3">${ROUNDED_PERCENT}%</text>
    <text x="92.5" y="14">${ROUNDED_PERCENT}%</text>
  </g>
</svg>
EOF

echo "Coverage badge generated at $BADGE_FILE with ${ROUNDED_PERCENT}% coverage"
