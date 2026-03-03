#!/bin/bash
PASS=0
FAIL=0

# runs a command sequence and checks that expected string IS present in output
expect_present() {
    local desc="$1"
    local input="$2"
    local expected="$3"
    local output
    output=$(printf "%s\nexit\n" "$input" | ./main 3 3 2>/dev/null)
    if echo "$output" | grep -qF -- "$expected"; then
        echo "PASS: $desc"
        ((PASS++))
    else
        echo "FAIL: $desc"
        echo "  expected to find: $expected"
        echo "  actual output snippet: $(echo "$output" | grep -F -- "Warning" | head -3)"
        ((FAIL++))
    fi
}

# runs a command sequence and checks that expected string is NOT present in output
expect_absent() {
    local desc="$1"
    local input="$2"
    local absent="$3"
    local output
    output=$(printf "%s\nexit\n" "$input" | ./main 3 3 2>/dev/null)
    if echo "$output" | grep -qF -- "$absent"; then
        echo "FAIL: $desc (unexpected warning appeared)"
        echo "  did not expect: $absent"
        ((FAIL++))
    else
        echo "PASS: $desc"
        ((PASS++))
    fi
}

DBL_MAX="1.7976931348623157e+308"

echo "--- Option 2 & 3: mult overflow ---"

# fill with DBL_MAX then multiply by 2 -> overflows to Inf
expect_present "option 3: pre-check warns before mult overflow" \
    "$(printf "fill $DBL_MAX\nmult 2")" \
    "Warning: multiplication would overflow"

expect_present "option 2: post-check detects Inf after mult" \
    "$(printf "fill $DBL_MAX\nmult 2")" \
    "Warning: overflow detected at element"

echo ""
echo "--- Option 2: add overflow ---"

# DBL_MAX + DBL_MAX = Inf
expect_present "option 2: post-check detects Inf after add" \
    "$(printf "fill $DBL_MAX\nsave 1\nma 1")" \
    "Warning: overflow detected during add"

echo ""
echo "--- Option 2: subtract overflow ---"

# DBL_MAX - (-DBL_MAX) = Inf
expect_present "option 2: post-check detects Inf after subtract" \
    "$(printf "fill -$DBL_MAX\nsave 1\nfill $DBL_MAX\nms 1")" \
    "Warning: overflow detected during subtract"

echo ""
echo "--- Option 2 & 3: matrix multiply dot product overflow ---"

# saved = all DBL_MAX, current = all DBL_MAX -> dot product = n * DBL_MAX^2 = Inf
expect_present "option 3: pre-check warns before dot product overflow" \
    "$(printf "fill $DBL_MAX\nsave 1\nfill $DBL_MAX\nmm 1")" \
    "Warning: dot product multiplication would overflow"

expect_present "option 2: post-check detects Inf in dot product" \
    "$(printf "fill $DBL_MAX\nsave 1\nfill $DBL_MAX\nmm 1")" \
    "Warning: dot product overflow detected"

echo ""
echo "--- Sanity checks: no false warnings during normal operation ---"

expect_absent "no warnings during normal mult" \
    "$(printf "fill 3\nmult 2")" \
    "Warning:"

expect_absent "no warnings during normal add" \
    "$(printf "fill 1\nsave 1\nfill 2\nma 1")" \
    "Warning:"

expect_absent "no warnings during normal subtract" \
    "$(printf "fill 10\nsave 1\nfill 3\nms 1")" \
    "Warning:"

expect_absent "no warnings during normal matrix multiply" \
    "$(printf "id\nsave 1\nid\nmm 1")" \
    "Warning:"

echo ""
echo "Results: $PASS passed, $FAIL failed"
[ $FAIL -eq 0 ] && exit 0 || exit 1
