#!/usr/bin/env bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 <datafile.sql> [-e <event_number>] [-q]"
  exit 1
fi

DATAFILE=""
EVENT=""
QUIET="false"
ROOTMACRO="analyze.C"

while [[ $# -gt 0 ]]; do
  case "$1" in
    -e)
      EVENT="$2"
      shift 2
      ;;
    -q)
      QUIET="true"
      shift
      ;;
    *)
      DATAFILE="$1"
      shift
      ;;
  esac
done

if [[ -z "$DATAFILE" ]]; then
  echo "Error: No data file provided"
  exit 1
fi

NEWNAME="${DATAFILE::-4}"

if [[ -n "$EVENT" ]]; then
  ARGS="\"${DATAFILE}\", ${EVENT}, ${QUIET}"
else
  ARGS="\"${DATAFILE}\", 0, ${QUIET}"
fi

root -l -b -q "${ROOTMACRO}(${ARGS})"

