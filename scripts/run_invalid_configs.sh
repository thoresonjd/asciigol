#!/bin/bash
# Run this from the root of the project

INVAL_CONFIG_DIR="./config/invalid"
ASCIIGOL="asciigol"
ASCIIGOL_BIN="./bin/$ASCIIGOL"

if [ ! -d "$INVAL_CONFIG_DIR" ]; then
	echo "$INVAL_CONFIG_DIR not found"
	exit
fi

if [ ! -f "$ASCIIGOL_BIN" ]; then
	make $ASCIIGOL
fi

for FILE in "$INVAL_CONFIG_DIR"/*; do
	if [ -f "$FILE" ]; then
		echo -e "$FILE\n\t$($ASCIIGOL_BIN --file=$FILE)"
	fi
done

