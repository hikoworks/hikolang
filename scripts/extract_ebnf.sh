#!/usr/bin/env bash

set -euo pipefail

# ---------------------------------------------------------------------------
# Verify that we're running from the repository root.
# ---------------------------------------------------------------------------

REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null || true)"

if [[ -z "$REPO_ROOT" ]]; then
    echo "Error: not inside a Git repository." >&2
    exit 1
fi

CURRENT_DIR="$(pwd -P)"
REPO_ROOT="$(cd "$REPO_ROOT" && pwd -P)"

if [[ "$CURRENT_DIR" != "$REPO_ROOT" ]]; then
    echo "Error: this script must be run from the Git repository root." >&2
    echo "Expected: $REPO_ROOT" >&2
    echo "Current:  $CURRENT_DIR" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

INPUT_DIR="doc/language/syntax"
OUTPUT_FILE="doc/language/syntax.md"
SCRIPT="scripts/extract_ebnf.py"

# ---------------------------------------------------------------------------
# Verify required paths exist.
# ---------------------------------------------------------------------------

if [[ ! -d "$INPUT_DIR" ]]; then
    echo "Error: input directory does not exist: $INPUT_DIR" >&2
    exit 1
fi

if [[ ! -f "$SCRIPT" ]]; then
    echo "Error: Python script does not exist: $SCRIPT" >&2
    exit 1
fi

# ---------------------------------------------------------------------------
# Generate the combined EBNF.
# ---------------------------------------------------------------------------

python3 "$SCRIPT" "$INPUT_DIR" --output "$OUTPUT_FILE"
