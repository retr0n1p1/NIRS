#!/bin/bash

if [ -z "$1" ]; then
    echo "Использование: $0 имя_файла.root"
    exit 1
fi

ROOTFILE="$1"

root -l -b -q "analyze.C(\"$ROOTFILE\")"

