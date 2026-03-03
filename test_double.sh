#!/bin/bash
PASS=0
FAIL=0

run_test() {
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
        echo "  actual output:    $output"
        ((FAIL++))
    fi
}

# fill with integer value - should display as integer-like double
run_test "fill integer value" \
    "fill 5" \
    "5"

# fill with decimal value - core double test
run_test "fill decimal value" \
    "fill 3.5" \
    "3.5"

# set a decimal value at a position
run_test "set decimal value" \
    "set 1 1 7.25" \
    "7.25"

# mult by integer scalar
run_test "mult by integer scalar" \
    "$(printf 'fill 4\nmult 3')" \
    "12"

# mult by decimal scalar
run_test "mult by decimal scalar" \
    "$(printf 'fill 2\nmult 1.5')" \
    "3"

# save then load restores values
run_test "save and load" \
    "$(printf 'fill 9.9\nsave 1\nfill 0\nload 1')" \
    "9.9"

# matrix add
run_test "matrix add" \
    "$(printf 'fill 3\nsave 1\nfill 2\nma 1')" \
    "5"

# matrix subtract
run_test "matrix subtract" \
    "$(printf 'fill 10\nsave 1\nfill 3\nms 1')" \
    "-7"

# identity matrix on 3x3
run_test "identity matrix diagonal" \
    "id" \
    "1"

# identity matrix off-diagonal is 0
run_test "identity matrix off-diagonal" \
    "id" \
    "0"

# transpose: fill row 0 col 1 with 9, after transpose it should be row 1 col 0
run_test "transpose moves value" \
    "$(printf 'fill 0\nset 0 1 9\ntr')" \
    "9"

# matrix multiply: identity * identity = identity
run_test "matrix multiply identity * identity" \
    "$(printf 'id\nsave 1\nid\nmm 1')" \
    "1"

echo ""
echo "Results: $PASS passed, $FAIL failed"
[ $FAIL -eq 0 ] && exit 0 || exit 1
