#!/bin/bash

test_src_file=test_disasm.c
test_obj_file=test_disasm.o
ref_output_file="output/reference_asm.txt"
breakarm_output_file="output/breakarm_output.txt"

echo "==> Extracting inline assembly from: $test_src_file"
grep -Po '^\s*"\K[^"]+' $test_src_file | sed 's/\\n$//' > "$ref_output_file"
echo "==> Using breakarm to disassemble: $test_obj_file"
../src/breakarm "$test_obj_file" | tail -n +10 | head -n -1 | cut -f3 > "$breakarm_output_file"

echo "==> comparing '$ref_output_file' with '$breakarm_output_file'"
diff_output=$(diff -i "$ref_output_file" "$breakarm_output_file")

if [ -z "$diff_output" ]; then
  echo "==> SUCCESS: Assembly instructions are identical!"
else
  echo "==> FAIL: Assembly instructions are not identical!"
  echo "$diff_output"
fi