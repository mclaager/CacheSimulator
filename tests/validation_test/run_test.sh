#!/bin/bash

mkdir -p results/

for FILE in validations/*;
do
  echo -e "\e[4;37mTesting $FILE...\e[0m"

  BLOCK_SIZE=$(grep "BLOCKSIZE:" "$FILE" | awk '{print $2}' | tr -cd 'a-zA-Z0-9._-')
  L1_SIZE=$(grep "L1_SIZE:" "$FILE" | awk '{print $2}' | tr -cd 'a-zA-Z0-9._-')
  L1_ASSOC=$(grep "L1_ASSOC:" "$FILE" | awk '{print $2}' | tr -cd 'a-zA-Z0-9._-')
  L2_SIZE=$(grep "L2_SIZE:" "$FILE" | awk '{print $2}' | tr -cd 'a-zA-Z0-9._-')
  L2_ASSOC=$(grep "L2_ASSOC:" "$FILE" | awk '{print $2}' | tr -cd 'a-zA-Z0-9._-')

  REPLACEMENT_POLICY=$(grep "REPLACEMENT POLICY:" "$FILE" | awk '{print $3}' | tr -cd 'a-zA-Z0-9._-')
  if [ "$REPLACEMENT_POLICY" = "LRU" ]; then
    REPL_POL=0
  elif [ "$REPLACEMENT_POLICY" = "FIFO" ]; then
    REPL_POL=1
  elif [ "$REPLACEMENT_POLICY" = "optimal" ]; then
    REPL_POL=2
  else
    REPL_POL=0
  fi

  INCLUSION_PROPERTY=$(grep "INCLUSION PROPERTY:" "$FILE" | awk '{print $3}' | tr -cd 'a-zA-Z0-9._-')
  if [ "$INCLUSION_PROPERTY" = "non-inclusive" ]; then
    INCL_PROP=0
  elif [ "$INCLUSION_PROPERTY" = "inclusive" ]; then
    INCL_PROP=1
  else
    INCL_PROP=0
  fi

  TRACE_FILE=$(grep "trace_file:" "$FILE" | awk '{print $2}' | tr -cd 'a-zA-Z0-9._-')

  OUTPUT_FILE="results/output_$(basename "$FILE")"

  #echo "../../src/sim_cache "$BLOCK_SIZE" "$L1_SIZE" "$L1_ASSOC" "$L2_SIZE" "$L2_ASSOC" "$REPL_POL" "$INCL_PROP" "../../data/traces/$TRACE_FILE" > "$OUTPUT_FILE""
  ../../sim_cache "$BLOCK_SIZE" "$L1_SIZE" "$L1_ASSOC" "$L2_SIZE" "$L2_ASSOC" "$REPL_POL" "$INCL_PROP" "../../data/traces/$TRACE_FILE" > "$OUTPUT_FILE"

  DIFF_OUTPUT=$(diff -iwy --suppress-common-lines "$OUTPUT_FILE" "$FILE")
  if [ -z "$DIFF_OUTPUT" ]; then
    echo -e "\e[1;32mTEST PASS\e[0m"
  else
    echo -e "\e[1;91mTEST FAIL\e[0m\n"
    echo -e "$DIFF_OUTPUT" | awk '{print "\t" $0}'
    echo ""
  fi
done
