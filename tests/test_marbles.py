import unittest

import spyn_puzzle as sp


class TestSpinMarble(unittest.TestCase):

    def test_default(self):
        marble = sp.SpinMarble()
        self.assertEqual(marble.id(), sp.INVALID_ID)
        self.assertEqual(marble.color(), sp.INVALID_COLOR)
        self.assertFalse(marble.is_valid())
        c1 = sp.black
        c2 = sp.red
        self.assertNotEqual(c1, c2)

    def test_array(self):
        marbles = [sp.SpinMarble(i, sp.blue) for i in range(10)]
        copy_marbles = [m for m in marbles]
        for m1, m2 in zip(marbles, copy_marbles):
            self.assertEqual(m1, m2)


if __name__ == '__main__':
    unittest.main()
