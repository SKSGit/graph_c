#!/bin/bash
# Tests for matrix multiplication (mm) and its interaction with identity (id)

PASS=0
FAIL=0

# Run the program with piped commands, extract the final matrix state.
# Matrix rows are printed as tab-separated integers; debug/info lines are filtered out.
run_test() {
    local name="$1"
    local dims="$2"
    local commands="$3"
    local expected="$4"  # space-separated values, rows separated by newlines
    local rows="$5"

    actual=$(printf '%s\n' "$commands" | ./main $dims 2>/dev/null \
        | grep -P '^-?\d+(\t-?\d+)*\t*$' \
        | tail -n "$rows" \
        | sed 's/\t$//' \
        | tr '\t' ' ')

    if [ "$actual" = "$expected" ]; then
        echo "PASS: $name"
        ((PASS++))
    else
        echo "FAIL: $name"
        echo "  Expected:"
        echo "$expected" | sed 's/^/    /'
        echo "  Got:"
        echo "$actual" | sed 's/^/    /'
        ((FAIL++))
    fi
}

# ── mm tests ──────────────────────────────────────────────────────────────────

# Test 1: 2×2 × 2×2, known values
# A (saved) = [[1,2],[3,4]], B (current) = [[5,6],[7,8]]
# A × B = [[1*5+2*7, 1*6+2*8], [3*5+4*7, 3*6+4*8]] = [[19,22],[43,50]]
run_test "2x2 basic multiplication" "2 2" \
"set 0 0 1
set 0 1 2
set 1 0 3
set 1 1 4
save 1
set 0 0 5
set 0 1 6
set 1 0 7
set 1 1 8
mm 1
exit" \
"19 22
43 50" 2

# Test 2: diagonal × known
# A (saved) = [[2,0],[0,3]], B (current) = [[4,5],[6,7]]
# A × B = [[8,10],[18,21]]
run_test "2x2 diagonal × general" "2 2" \
"set 0 0 2
set 0 1 0
set 1 0 0
set 1 1 3
save 1
set 0 0 4
set 0 1 5
set 1 0 6
set 1 1 7
mm 1
exit" \
"8 10
18 21" 2

# Test 3: non-square 2×3 × 3×2 = 2×2
# A (saved, 2×3) = [[1,2,3],[4,5,6]]
# B (current, 3×2) = [[7,8],[9,10],[11,12]]
# A × B = [[58,64],[139,154]]
run_test "non-square 2x3 × 3x2" "2 2" \
"resize 2 3
set 0 0 1
set 0 1 2
set 0 2 3
set 1 0 4
set 1 1 5
set 1 2 6
save 1
resize 3 2
fill 0
set 0 0 7
set 0 1 8
set 1 0 9
set 1 1 10
set 2 0 11
set 2 1 12
mm 1
exit" \
"58 64
139 154" 2

# ── id + mm combination tests ─────────────────────────────────────────────────

# Test 4: A × I = A  (identity as current matrix)
# A (saved) = [[3,4],[5,6]], current = I
# mm 1 = A × I = A
run_test "id: A × I = A" "2 2" \
"set 0 0 3
set 0 1 4
set 1 0 5
set 1 1 6
save 1
id
mm 1
exit" \
"3 4
5 6" 2

# Test 5: I × A = A  (identity as saved matrix)
# saved = I, current = A = [[3,4],[5,6]]
# mm 1 = I × A = A
run_test "id: I × A = A" "2 2" \
"id
save 1
set 0 0 3
set 0 1 4
set 1 0 5
set 1 1 6
mm 1
exit" \
"3 4
5 6" 2

# Test 6: I × I = I
run_test "id: I × I = I" "2 2" \
"id
save 1
id
mm 1
exit" \
"1 0
0 1" 2

# Test 7: (A × I) result re-multiplied by A should give A²
# A = [[1,0],[0,2]], A² = [[1,0],[0,4]]
# Step 1: set A, save as 1
# Step 2: make current = I, run mm 1 → A × I = A
# Step 3: save result as 2, reload A
# Step 4: mm 2 → A × A = A²
run_test "id: chained (A×I)×A = A²" "2 2" \
"set 0 0 1
set 0 1 0
set 1 0 0
set 1 1 2
save 1
id
mm 1
save 2
set 0 0 1
set 0 1 0
set 1 0 0
set 1 1 2
mm 2
exit" \
"1 0
0 4" 2

echo ""
echo "Results: $PASS passed, $FAIL failed"
[ $FAIL -eq 0 ] && exit 0 || exit 1
