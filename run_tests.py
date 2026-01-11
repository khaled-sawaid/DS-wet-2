import os
import argparse
import subprocess
import glob

COMPILATION_FLAGS = ["-std=c++14", "-DNDEBUG", "-Wall"]
TIMEOUT = 10


def run_test(exe_file, test_id, tests_dir):
    input_file = os.path.join(tests_dir, f"test{test_id}.in")
    expected_output_file = os.path.join(tests_dir, f"test{test_id}.out")
    result_file = os.path.join(tests_dir, f"test{test_id}.res")

    if not os.path.isfile(input_file):
        print(f"Input file for test {test_id} not found")
        return False

    if not os.path.isfile(expected_output_file):
        print(f"Expected output file for test {test_id} not found")
        return False

    try:
        with open(input_file, "r") as stdin, open(result_file, "w") as stdout:
            subprocess.run(
                [exe_file],
                stdin=stdin,
                stdout=stdout,
                timeout=TIMEOUT,
                check=True
            )
    except subprocess.TimeoutExpired:
        print(f"Test {test_id} Failed: Timeout")
        return False
    except subprocess.CalledProcessError as e:
        print(f"Test {test_id} Failed: Runtime error ({e})")
        return False

    with open(result_file, "r") as res, open(expected_output_file, "r") as expected:
        if res.read().strip() == expected.read().strip():
            print(f"Test {test_id} Passed")
            return True
        else:
            print(f"Test {test_id} Failed: Wrong output")
            return False


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--tests_dir", default="./tests/")
    parser.add_argument("--code_dir", default="./")
    parser.add_argument("--compiler_path", default="g++")
    parser.add_argument("--clean", action="store_true")
    parser.add_argument("--abort_on_fail", action="store_true")
    parser.add_argument("-t", "--tests", type=int, nargs="*", default=None)
    args = parser.parse_args()

    source_files = glob.glob(os.path.join(args.code_dir, "*.cpp"))
    if not source_files:
        print("No .cpp files found")
        return -1

    exe_file = os.path.abspath(os.path.join(args.code_dir, "main.exe"))

    compile_cmd = [args.compiler_path] + COMPILATION_FLAGS + ["-o", exe_file] + source_files

    try:
        subprocess.run(compile_cmd, check=True)
    except subprocess.CalledProcessError:
        print("Compilation failed")
        return -1

    if args.clean:
        for f in os.listdir(args.tests_dir):
            if f.endswith(".res"):
                os.remove(os.path.join(args.tests_dir, f))
        return 0

    if args.tests is None:
        tests = sorted(
            int(f.split("test")[1].split(".in")[0])
            for f in os.listdir(args.tests_dir)
            if f.endswith(".in")
        )
    else:
        tests = args.tests

    for test_id in tests:
        ok = run_test(exe_file, test_id, args.tests_dir)
        if not ok and args.abort_on_fail:
            return 0

    return 0


if __name__ == "__main__":
    main()
