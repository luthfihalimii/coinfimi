#!/bin/sh
set -eu

BIN_ARG="${1:-./cryptex}"
BIN_DIR=$(cd "$(dirname "$BIN_ARG")" && pwd)
BIN="$BIN_DIR/$(basename "$BIN_ARG")"

if [ ! -x "$BIN" ]; then
    echo "FAIL: binary not found: $BIN" >&2
    exit 1
fi

TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT
cd "$TMP_DIR"

check_contains() {
    case "$2" in
        *"$1"*) ;;
        *) echo "FAIL: expected output to contain: $1" >&2; exit 1 ;;
    esac
}

OUT=$(printf '2\n\n0\n' | "$BIN")
check_contains "CRYPTOCURRENCY LIST" "$OUT"
check_contains "BTC" "$OUT"
check_contains "USDC" "$OUT"
check_contains "TRX" "$OUT"
check_contains "LINK" "$OUT"
check_contains "XLM" "$OUT"
check_contains "BCH" "$OUT"
check_contains "NEAR" "$OUT"
check_contains "UNI" "$OUT"
check_contains "LTC" "$OUT"
check_contains "SUI" "$OUT"
check_contains "AVAX" "$OUT"
check_contains "HBAR" "$OUT"
check_contains "ZEC" "$OUT"

OUT=$(printf '9\n0\n' | "$BIN")
check_contains "Invalid choice" "$OUT"

OUT=$(printf '1\nUSD\n1234567.89\nUSD\n0\n0\n' | "$BIN")
check_contains "1,234,567.89" "$OUT"

OUT=$(printf '4\n0\n' | "$BIN")
check_contains "CONVERSION HISTORY" "$OUT"

mkdir -p data
{
    echo "timestamp,from_symbol,to_symbol,amount,exchange_rate,result"
    i=1
    while [ "$i" -le 25 ]; do
        echo "2026-01-01 00:00:00,BTC,USD,1.00000000,1.00000000,1.00000000"
        i=$((i + 1))
    done
} > data/history.csv

OUT=$(printf '4\n0\n' | "$BIN")
check_contains "Showing last 20 of 25" "$OUT"

OUT=$(printf '4\n1\n1\n\n0\n' | "$BIN")
check_contains "History cleared." "$OUT"

OUT=$("$BIN" --help 2>&1)
check_contains "Usage:" "$OUT"

OUT=$("$BIN" BTC 0.5 2>&1 || true)
check_contains "Usage:" "$OUT"

OUT=$("$BIN" NOPE 1 USD 2>&1 || true)
check_contains "Unsupported currency" "$OUT"

OUT=$("$BIN" BTC 0 USD 2>&1 || true)
check_contains "Invalid amount" "$OUT"

if "$BIN" NOPE 1 USD >/dev/null 2>&1; then
    echo "FAIL: unsupported CLI currency should exit non-zero" >&2
    exit 1
fi

printf '[TEST] test_ui PASSED.\n'
