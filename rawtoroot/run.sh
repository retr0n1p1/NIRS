#!/usr/bin/env bash

# Проверка наличия хотя бы одного аргумента
if [ $# -lt 1 ]; then
  echo "Usage: $0 <datafile.sql> [-e <event_number>] [-n <some_integer>] [-q]"
  exit 1
fi

DATAFILE=""
EVENT=""
NUMBER=""
QUIET="false"
ROOTMACRO="analyze.C"

# Разбор аргументов
while [[ $# -gt 0 ]]; do
  case "$1" in
    -e)
      EVENT="$2"
      shift 2
      ;;
    -n)
      NUMBER="$2"
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

# Проверка обязательного файла
if [[ -z "$DATAFILE" ]]; then
  echo "Error: No data file provided"
  exit 1
fi


# Подготовка аргументов для ROOT-скрипта
if [[ -z "$EVENT" ]]; then
  EVENT=0
fi

if [[ -z "$NUMBER" ]]; then
  NUMBER=0
fi
if [[ "$NUMBER" == "0" ]]; then
	echo "Searching for different nint..."
	./count_numbers "${DATAFILE}" | while IFS=' ' read -r nint ent; do
		echo ""
		echo "nint: $nint, entries: $ent"
		if [[ "$EVENT" == 0 ]]; then
			ARGS="\"${DATAFILE}\", $ent, ${QUIET}, $nint"
			root -l -b -q "${ROOTMACRO}(${ARGS})"
		else
			ARGS="\"${DATAFILE}\", ${EVENT}, ${QUIET}, $nint"
			root -l -b -q "${ROOTMACRO}(${ARGS})"
		fi
	done
else
	ARGS="\"${DATAFILE}\", ${EVENT}, ${QUIET}, ${NUMBER}"
	root -l -b -q "${ROOTMACRO}(${ARGS})"
fi
