from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

from pd_code_to_diagram import pd_code_diagram_sanity
from pd_code_to_diagram import from_diagram
from pd_code_to_diagram.main import _find_compiler, _validate_pd_code, create_exe_file


TREFOIL = [[1, 5, 2, 4], [3, 1, 4, 6], [5, 3, 6, 2]]


class ValidationTests(unittest.TestCase):
    def test_accepts_canonical_pd_code(self):
        self.assertEqual(_validate_pd_code(TREFOIL), TREFOIL)

    def test_rejects_boolean_gap_and_empty_codes(self):
        with self.assertRaisesRegex(ValueError, "at least one"):
            _validate_pd_code([])
        with self.assertRaisesRegex(ValueError, "positive integers"):
            _validate_pd_code([[True, 1, 2, 2]])
        with self.assertRaisesRegex(ValueError, "1..2n"):
            _validate_pd_code([[1, 1, 3, 3]])

    def test_ambiguous_crossings_fail_without_an_infinite_loop(self):
        with patch.object(from_diagram, "get_pd_code_crossing", return_value=None):
            with self.assertRaisesRegex(ValueError, "could not infer"):
                from_diagram._resolve_crossings(
                    [[0]], [[""]], [(0, 0)], verbose=False
                )


class IntegrationTests(unittest.TestCase):
    def test_coordinate_dot_product_uses_both_components(self):
        source_root = (
            Path(__file__).resolve().parents[1]
            / "pd_code_to_diagram"
            / "cpp_src"
        )
        program = """
#include <iostream>
#include "Utils/Coord2dPosition.h"

int main() {
    std::cout << Coord2dPosition::dot({2.0, 3.0}, {4.0, 5.0});
}
"""
        with tempfile.TemporaryDirectory(prefix="pd_dot_product_") as tmp:
            tmp_path = Path(tmp)
            source = tmp_path / "dot_product.cpp"
            executable = tmp_path / "dot_product.exe"
            source.write_text(program, encoding="utf-8")
            subprocess.run(
                [
                    _find_compiler(),
                    "-std=c++17",
                    f"-I{source_root}",
                    str(source),
                    "-o",
                    str(executable),
                ],
                check=True,
                text=True,
            )
            result = subprocess.run(
                [str(executable)],
                check=True,
                text=True,
                stdout=subprocess.PIPE,
            )
        self.assertEqual(float(result.stdout), 23.0)

    def test_bundled_engine_builds_and_round_trips_trefoil(self):
        success, message = create_exe_file(force=True)
        self.assertTrue(success, message)
        self.assertTrue(Path(message.split(": ", 1)[-1]).exists())
        matches, recovered = pd_code_diagram_sanity(TREFOIL)
        self.assertTrue(matches, recovered)


if __name__ == "__main__":
    unittest.main()
