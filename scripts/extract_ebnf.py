#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
from pathlib import Path


SYNTAX_HEADING = re.compile(
    r"^\s*##\s+Syntax\s*$",
    re.IGNORECASE | re.MULTILINE,
)

SEMANTICS_HEADING = re.compile(
    r"^\s*##\s+Semantics\s*$",
    re.IGNORECASE | re.MULTILINE,
)

REFERENCE_LINK = re.compile(
    r"^[ \t]*"
    r"\[([^\]]+)\]"
    r":[ \t]+"
    r"(?:<([^>]+)>|(\S+))"
    r"(?:[ \t]+(?:\"([^\"]*)\"|'([^']*)'|\(([^)]*)\)))?"
    r"[ \t]*$",
    re.MULTILINE,
)


def extract_syntax(text: str) -> str | None:
    """
    Extract everything between ## Syntax and the next # Semantics.

    The contents are returned completely unchanged.
    """

    syntax_match = SYNTAX_HEADING.search(text)

    if not syntax_match:
        return None

    semantics_match = SEMANTICS_HEADING.search(
        text,
        syntax_match.end(),
    )

    if not semantics_match:
        return None

    return text[syntax_match.end():semantics_match.start()].strip("\n")


def extract_links(text: str) -> dict[str, str]:
    """Extract reference-style Markdown links."""

    links: dict[str, str] = {}

    for match in REFERENCE_LINK.finditer(text):
        label = match.group(1)
        destination = match.group(2) or match.group(3)

        # Markdown reference labels are case-insensitive in practice.
        # Preserve the spelling of the first occurrence.
        key = label.casefold()

        if key not in links:
            links[key] = f"[{label}]: {destination}"

        elif links[key] != f"[{label}]: {destination}":
            print(
                f"WARNING: conflicting Markdown reference "
                f"definition for [{label}]"
            )
            print(f"  existing: {links[key]}")
            print(f"  new:      [{label}]: {destination}")

    return links


def process_directory(
    input_directory: Path,
    output_file: Path,
) -> None:
    snippets: list[tuple[str, str]] = []
    merged_links: dict[str, str] = {}

    markdown_files = sorted(
        path
        for path in input_directory.iterdir()
        if path.is_file()
        and path.suffix.lower() in {".md", ".markdown"}
        and path.resolve() != output_file.resolve()
    )

    for path in markdown_files:
        print(f"Processing {path.name}...")

        text = path.read_text(encoding="utf-8")

        # ---------------------------------------------------------------
        # Extract EBNF WITHOUT MODIFYING IT
        # ---------------------------------------------------------------

        syntax = extract_syntax(text)

        if syntax is not None and syntax.strip():
            snippets.append((path.name, syntax))

        # ---------------------------------------------------------------
        # Collect Markdown reference definitions
        # ---------------------------------------------------------------

        links = extract_links(text)

        for key, definition in links.items():
            if key not in merged_links:
                merged_links[key] = definition

    # ---------------------------------------------------------------
    # Build output
    # ---------------------------------------------------------------

    output: list[str] = [
        "# Combined EBNF",
        "",
    ]

    for filename, syntax in snippets:
        output.append("")

        # IMPORTANT:
        # `syntax` is inserted exactly as extracted.
        output.append(syntax)

        output.append("")

    # ---------------------------------------------------------------
    # Append merged Markdown references
    # ---------------------------------------------------------------

    if merged_links:
        output.append("")

        for definition in sorted(
            merged_links.values(),
            key=str.casefold,
        ):
            output.append(definition)

        output.append("")

    output_file.write_text(
        "\n".join(output),
        encoding="utf-8",
    )

    print()
    print(f"Processed {len(markdown_files)} Markdown files.")
    print(f"Extracted {len(snippets)} EBNF snippets.")
    print(f"Merged {len(merged_links)} Markdown references.")
    print(f"Wrote: {output_file}")


def main() -> None:
    parser = argparse.ArgumentParser(
        description=(
            "Extract EBNF sections from Markdown files while "
            "preserving their original Markdown formatting."
        )
    )

    parser.add_argument(
        "directory",
        type=Path,
        help="Directory containing Markdown files",
    )

    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("combined-ebnf.md"),
        help="Output file (default: combined-ebnf.md)",
    )

    args = parser.parse_args()

    if not args.directory.is_dir():
        parser.error(f"Not a directory: {args.directory}")

    process_directory(
        args.directory,
        args.output,
    )


if __name__ == "__main__":
    main()
