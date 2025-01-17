import unittest
from solution1 import Solution1

import numpy as np


class TestSolution1(unittest.TestCase):
    # Тесты
    s = Solution1

    def test1(self):
        arr = np.array([1, 2, 4, 6, 1, 2, 8, 10, 12, 3, 4, 2, 6])
        self.assertEqual(self.s.max_even_sequence(arr), 4)

    def test2(self):
        arr = np.arange(1_000_000)
        self.assertEqual(self.s.max_even_sequence(arr), 1)


if __name__ == "__main__":
    unittest.main()
