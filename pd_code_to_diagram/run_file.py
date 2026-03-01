import subprocess
import sys

def run_program_with_input(program_path, args=None, input_str="", encoding="utf-8"):
    """
    Execute a specified program using subprocess, pass command-line arguments,
    provide input from a string, and capture stdout and stderr as strings.

    Args:
        program_path (str): Path to the program to run (e.g., "python", "/usr/bin/ls", or "./my_script.sh")
        args (list, optional): List of command-line arguments to pass to the program, 
                               e.g., ["-c", "print('hello')"]. Defaults to None (no arguments).
        input_str (str, optional): String to pass as standard input to the program. Defaults to empty string.
        encoding (str, optional): Character encoding for input/output (prevents garbled text for non-ASCII chars). 
                                  Defaults to "utf-8".

    Returns:
        tuple: (stdout_str, stderr_str, return_code)
               stdout_str: String containing the program's standard output
               stderr_str: String containing the program's standard error
               return_code: Exit code of the program (0 means success)

    Raises:
        FileNotFoundError: If the specified program does not exist or is not executable
        subprocess.TimeoutExpired: If the program execution times out
        Exception: Other runtime exceptions during program execution
    """
    # Handle default value for args to ensure it's always a list
    if args is None:
        args = []
    
    # Construct full command list (program path + arguments)
    cmd = [program_path] + args

    try:
        # Run the program with subprocess:
        # - stdin=subprocess.PIPE: Allow input from string
        # - stdout/stderr=subprocess.PIPE: Capture output/error streams
        # - text=True: Automatically convert byte streams to strings (replaces universal_newlines=True)
        # - encoding: Specify character encoding for input/output
        result = subprocess.run(
            cmd,
            input=input_str,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            encoding=encoding,
            # Optional: Set timeout (seconds) to prevent infinite hanging
            # timeout=30,
        )

        # Return captured output, error, and exit code
        return (result.stdout, result.stderr, result.returncode)

    except FileNotFoundError:
        print(f"Error: Program '{program_path}' not found. Please check the path.", file=sys.stderr)
        raise
    except subprocess.TimeoutExpired:
        print(f"Error: Program '{program_path}' execution timed out.", file=sys.stderr)
        raise
    except Exception as e:
        print(f"Error: An exception occurred while running the program - {str(e)}", file=sys.stderr)
        raise

# ------------------- Test Examples -------------------
if __name__ == "__main__":
    # Example 1: Run Python command with simple code and input
    python_code = """
import sys
# Read standard input
input_data = sys.stdin.read()
print(f"Received input: {input_data}")
# Write to standard error
sys.stderr.write("This is standard error output\\n")
"""
    stdout, stderr, return_code = run_program_with_input(
        program_path="python",
        args=["-c", python_code],
        input_str="Test input content"
    )

    print("=== Standard Output ===")
    print(stdout)
    print("=== Standard Error ===")
    print(stderr)
    print(f"=== Exit Code ===")
    print(return_code)

    # Example 2: Run system command (e.g., ls)
    # stdout, stderr, return_code = run_program_with_input(
    #     program_path="ls",
    #     args=["-l", "."]
    # )