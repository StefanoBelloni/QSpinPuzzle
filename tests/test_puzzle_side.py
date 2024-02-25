import unittest

import spyn_puzzle as sp


class TestSpinSpin(unittest.TestCase):

    def _get_puzzle(self):
        marbles = []
        m_id = 0
        color = sp.black
        for m_id in range(30):
            marbles.append(sp.SpinMarble(m_id, color))
            if m_id + 1 == 10:
                color = sp.red
            elif m_id + 1 == 20:
                color = sp.green
        return sp.SpinPuzzleSide(marbles)

    def test_iterate_leaf(self):
        N = 10
        puzzle = self._get_puzzle()
        for k, leaf, color in [
            (0, sp.LEAF.NORTH, sp.black),
            (10, sp.LEAF.EAST, sp.red),
            (20, sp.LEAF.WEST, sp.green)
        ]:
            m_id = 0
            it = puzzle.begin(leaf)
            for _ in range(6 * N):
                self.assertEqual(it.id(), (m_id % N) + k)
                self.assertEqual(it.color(), color)
                m_id += 1
                it += 1

    def test_iterate_leaf_by_name(self):
        N = 10
        puzzle = self._get_puzzle()
        for k, iterator, color in [
            (0, puzzle.north, sp.black),
            (10, puzzle.east, sp.red),
            (20, puzzle.west, sp.green)
        ]:
            m_id = 0
            it = iterator()
            for _ in range(6 * N):
                self.assertEqual(it[m_id % N].id(), (m_id % N) + k)
                self.assertEqual(it[m_id % N].color(), color)
                m_id += 1

    def test_rotate_leaf_cw_fraction_next_marble(self):
        N = 10
        puzzle = self._get_puzzle()
        it_north_begin_0 = puzzle.begin(sp.LEAF.NORTH)
        self.assertEqual(it_north_begin_0.get_angle(), 0.0)

        # theta_1 = 9°
        self.assertTrue(puzzle.rotate_marbles(sp.LEAF.NORTH, 360.0 / N / 4))
        it_north = puzzle.marbles()   # all marbles
        it_north_begin_1 = puzzle.begin(sp.LEAF.NORTH)
        self.assertEqual(it_north_begin_1.get_angle(), 360.0 / N / 4)
        self.assertEqual(it_north[0].id(), it_north_begin_1.id())
        self.assertEqual(it_north[0].color(), it_north_begin_1.color())

        # theta_2 = 18° => theta_tot = 27°
        self.assertTrue(puzzle.rotate_marbles(
            sp.LEAF.NORTH, 2 * 360.0 / N / 4))
        # theta_tot = 27° > 36° = end of interval of first marble.
        # marble n.9 is now the first marble
        it_north_begin_2 = puzzle.begin(sp.LEAF.NORTH)
        self.assertEqual(it_north_begin_2.id(), 9)
        self.assertEqual(it_north[N-1].id(), it_north_begin_2.id())
        self.assertEqual(it_north[N-1].color(), it_north_begin_2.color())
        # the marble n.9 is now 9° before it middle point (36° - 27°)
        self.assertEqual(it_north_begin_2.get_angle(), -9)

    def test_rotate_leaf_cw_next_marble(self):
        N = 10
        puzzle = self._get_puzzle()
        it_north_begin_0 = puzzle.begin(sp.LEAF.NORTH)
        self.assertEqual(it_north_begin_0.get_angle(), 0.0)

        # theta_1 = 9°
        self.assertTrue(puzzle.rotate_marbles(sp.LEAF.NORTH, 360.0 / N / 2))
        it_north = puzzle.marbles()   # all marbles
        it_north_begin_1 = puzzle.begin(sp.LEAF.NORTH)
        self.assertEqual(it_north_begin_1.get_angle(), -360.0 / N / 2)

        # theta_2 = 18° => theta_tot = 27°
        self.assertTrue(puzzle.rotate_marbles(sp.LEAF.NORTH, 360.0 / N / 2))
        # theta_tot = 27° > 36° = end of interval of first marble.
        # marble n.9 is now the first marble
        it_north_begin_2 = puzzle.begin(sp.LEAF.NORTH)
        self.assertEqual(it_north_begin_2.id(), 9)
        # the marble n.9 is now 9° before it middle point (36° - 27°)
        self.assertEqual(it_north_begin_2.get_angle(), 0)


if __name__ == '__main__':
    unittest.main()
