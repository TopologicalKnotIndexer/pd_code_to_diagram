from pathlib import Path
import unittest
from unittest.mock import patch

from pd_code_to_diagram import pd_code_diagram_sanity
from pd_code_to_diagram import from_diagram
from pd_code_to_diagram.main import _validate_pd_code, create_exe_file


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
    def test_bundled_engine_builds_and_round_trips_trefoil(self):
        success, message = create_exe_file(force=True)
        self.assertTrue(success, message)
        self.assertTrue(Path(message.split(": ", 1)[-1]).exists())
        matches, recovered = pd_code_diagram_sanity(TREFOIL)
        self.assertTrue(matches, recovered)


if __name__ == "__main__":
    unittest.main()
