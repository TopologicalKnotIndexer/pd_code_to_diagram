"""Small subprocess helper used by the layout wrapper."""

import subprocess


def run_program_with_input(
    program_path: str,
    args: list[str] | None = None,
    input_str: str = "",
    encoding: str = "utf-8",
    timeout: float = 120,
) -> tuple[str, str, int]:
    command = [program_path, *(args or [])]
    result = subprocess.run(
        command,
        input=input_str,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding=encoding,
        errors="replace",
        timeout=timeout,
        check=False,
    )
    return result.stdout, result.stderr, result.returncode
