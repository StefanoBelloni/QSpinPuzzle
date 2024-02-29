#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "puzzle/spin_marble.h"
#include "puzzle/spin_puzzle_game.h"
#include "puzzle/spin_puzzle_side.h"

namespace py = pybind11;

PYBIND11_MODULE(spinbind11_puzzle, m) {

  // =================================================================== //
  py::class_<puzzle::SpinMarble>(m, "SpinMarble")
      // =================================================================== //
      .def(py::init<>())
      .def(py::init<int, puzzle::Color>())
      .def("color", &puzzle::SpinMarble::color)
      .def("id", &puzzle::SpinMarble::id)
      .def("is_valid", &puzzle::SpinMarble::is_valid)
      .def("__str__", &puzzle::SpinMarble::to_string)
      .def("__repr__", &puzzle::SpinMarble::to_string)
      .def(py::self == py::self)
      .def(py::self != py::self);

  // =================================================================== //
  py::class_<puzzle::SpinPuzzleSide<>>(m, "SpinPuzzleSide")
      // =================================================================== //
      .def(py::init<std::array<puzzle::SpinMarble, 30>>())
      .def("__str__", &puzzle::SpinPuzzleSide<>::to_string)
      .def("is_border_rotation_possible",
           &puzzle::SpinPuzzleSide<>::is_border_rotation_possible)
      .def("is_rotation_possible",
           &puzzle::SpinPuzzleSide<>::is_rotation_possible)
      .def("__repr__", &puzzle::SpinPuzzleSide<>::to_string)
      .def("trifoild_status", &puzzle::SpinPuzzleSide<>::get_trifoild_status)
      .def("rotate_marbles", &puzzle::SpinPuzzleSide<>::rotate_marbles)
      .def("rotate_internal_disk",
           &puzzle::SpinPuzzleSide<>::rotate_internal_disk)
      // https://pybind11.readthedocs.io/en/stable/classes.html
      .def("begin",
           py::overload_cast<puzzle::LEAF>(&puzzle::SpinPuzzleSide<>::begin))
      .def("begin", py::overload_cast<>(&puzzle::SpinPuzzleSide<>::begin))
      .def("marbles",
           [](puzzle::SpinPuzzleSide<> &spinSide) {
             if (spinSide.get_trifoild_status() ==
                 puzzle::TREFOIL::BORDER_ROTATION) {
               std::vector<puzzle::SpinMarble> marbles;
               auto it = spinSide.begin(puzzle::LEAF::TREFOIL);
               for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::N_MARBLES;
                    ++n, ++it) {
                 marbles.emplace_back(*it);
               }
               return marbles;
             }
             std::vector<puzzle::SpinMarble> marbles;
             auto it = spinSide.begin(puzzle::LEAF::NORTH);
             for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::GROUP_SIZE;
                  ++n, ++it) {
               marbles.emplace_back(*it);
             }
             it = spinSide.begin(puzzle::LEAF::EAST);
             for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::GROUP_SIZE;
                  ++n, ++it) {
               marbles.emplace_back(*it);
             }
             it = spinSide.begin(puzzle::LEAF::WEST);
             for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::GROUP_SIZE;
                  ++n, ++it) {
               marbles.emplace_back(*it);
             }
             return marbles;
           })
      .def("border",
           [](puzzle::SpinPuzzleSide<> &spinSide) {
             if (spinSide.get_trifoild_status() !=
                 puzzle::TREFOIL::BORDER_ROTATION) {
               return std::vector<puzzle::SpinMarble>();
             }
             std::vector<puzzle::SpinMarble> marbles;
             auto it = spinSide.begin(puzzle::LEAF::TREFOIL);
             for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::N_MARBLES;
                  ++n, ++it) {
               marbles.emplace_back(*it);
             }
             return marbles;
           })
      .def("north",
           [](puzzle::SpinPuzzleSide<> &spinSide) {
             if (spinSide.get_trifoild_status() ==
                 puzzle::TREFOIL::BORDER_ROTATION) {
               return std::vector<puzzle::SpinMarble>();
             }
             std::vector<puzzle::SpinMarble> marbles;
             auto it = spinSide.begin(puzzle::LEAF::NORTH);
             for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::GROUP_SIZE;
                  ++n, ++it) {
               marbles.emplace_back(*it);
             }
             return marbles;
           })
      .def("east",
           [](puzzle::SpinPuzzleSide<> &spinSide) {
             if (spinSide.get_trifoild_status() ==
                 puzzle::TREFOIL::BORDER_ROTATION) {
               return std::vector<puzzle::SpinMarble>();
             }
             std::vector<puzzle::SpinMarble> marbles;
             auto it = spinSide.begin(puzzle::LEAF::EAST);
             for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::GROUP_SIZE;
                  ++n, ++it) {
               marbles.emplace_back(*it);
             }
             return marbles;
           })
      .def("west", [](puzzle::SpinPuzzleSide<> &spinSide) {
        if (spinSide.get_trifoild_status() ==
            puzzle::TREFOIL::BORDER_ROTATION) {
          return std::vector<puzzle::SpinMarble>();
        }
        std::vector<puzzle::SpinMarble> marbles;
        auto it = spinSide.begin(puzzle::LEAF::WEST);
        for (size_t n = 0; n < puzzle::SpinPuzzleSide<>::GROUP_SIZE;
             ++n, ++it) {
          marbles.emplace_back(*it);
        }
        return marbles;
      });

  // =================================================================== //
  py::class_<puzzle::SpinPuzzleSide<>::iterator>(m, "MarbleItem")
      // =================================================================== //
      .def(py::init<>())
      .def(py::self += int())
      .def(py::self -= int())
      .def(py::self == py::self)
      .def("id",
           [](const puzzle::SpinPuzzleSide<>::iterator &iter) {
             return iter->id();
           })
      .def("color",
           [](const puzzle::SpinPuzzleSide<>::iterator &iter) {
             return iter->color();
           })
      .def("get_angle",
           [](const puzzle::SpinPuzzleSide<>::iterator &iter) {
             return iter.get_angle();
           })
      .def("__str__",
           [](puzzle::SpinPuzzleSide<>::iterator &iter) {
             return iter->to_string();
           })
      .def("__repr__", [](puzzle::SpinPuzzleSide<>::iterator &iter) {
        return iter->to_string();
      });

  // =================================================================== //
  py::class_<puzzle::SpinPuzzleGame>(m, "SpinPuzzleGame")
      // =================================================================== //
      .def(py::init<>())
      .def("is_game_solved", &puzzle::SpinPuzzleGame::is_game_solved)
      .def("process_key", &puzzle::SpinPuzzleGame::process_key)
      .def("get_keybord_state", &puzzle::SpinPuzzleGame::get_keybord_state)
      .def("rotate_marbles", &puzzle::SpinPuzzleGame::rotate_marbles)
      .def("rotate_internal_disk",
           &puzzle::SpinPuzzleGame::rotate_internal_disk)
      .def("spin_leaf", py::overload_cast<puzzle::LEAF, double>(
                            &puzzle::SpinPuzzleGame::spin_leaf))
      .def("spin_leaf",
           py::overload_cast<puzzle::LEAF>(&puzzle::SpinPuzzleGame::spin_leaf))
      .def("swap_side", &puzzle::SpinPuzzleGame::swap_side)
      .def("get_side", py::overload_cast<>(&puzzle::SpinPuzzleGame::get_side),
           py::return_value_policy::reference)
      .def("get_side",
           py::overload_cast<puzzle::SIDE>(&puzzle::SpinPuzzleGame::get_side),
           py::return_value_policy::reference)
      .def("get_active_side", &puzzle::SpinPuzzleGame::get_active_side)
      .def("get_phase_shift_internal_disk",
           [](puzzle::SpinPuzzleGame &game) {
             return game.get_side().get_phase_shift_internal_disk();
           })
      .def("reset", &puzzle::SpinPuzzleGame::reset)
      .def("shuffle", &puzzle::SpinPuzzleGame::shuffle, py::arg("seed") = 0,
           py::arg("commands") = 10000, py::arg("check") = false)
      .def("__str__", &puzzle::SpinPuzzleGame::to_string)
      .def("__repr__", &puzzle::SpinPuzzleGame::to_string)
      .def("current_time_step", &puzzle::SpinPuzzleGame::current_time_step);

  // =================================================================== //
  py::enum_<puzzle::LEAF>(m, "LEAF")
      // =================================================================== //
      .value("NORTH", puzzle::LEAF::NORTH)
      .value("EAST", puzzle::LEAF::EAST)
      .value("WEST", puzzle::LEAF::WEST)
      .value("TREFOIL", puzzle::LEAF::TREFOIL)
      .value("INVALID", puzzle::LEAF::INVALID);
  // =================================================================== //
  py::enum_<puzzle::SIDE>(m, "SIDE")
      // =================================================================== //
      .value("FRONT", puzzle::SIDE::FRONT)
      .value("BACK", puzzle::SIDE::BACK);
  // =================================================================== //
  py::enum_<puzzle::TREFOIL>(m, "TREFOIL")
      // =================================================================== //
      .value("INVALID", puzzle::TREFOIL::INVALID)
      .value("LEAF_ROTATION", puzzle::TREFOIL::LEAF_ROTATION)
      .value("LEAF_SPINNING", puzzle::TREFOIL::LEAF_SPINNING)
      .value("BORDER_ROTATION", puzzle::TREFOIL::BORDER_ROTATION);
  // =================================================================== //
  py::enum_<puzzle::ROTATION>(m, "ROTATION")
      // =================================================================== //
      .value("OK", puzzle::ROTATION::OK)
      .value("INVALID", puzzle::ROTATION::INVALID);

  // =================================================================== //
  // CONSTANTS
  // =================================================================== //
  // KEYs
  m.attr("Key_N") = py::int_(puzzle::Key_N);
  m.attr("Key_P") = py::int_(puzzle::Key_P);
  m.attr("Key_E") = py::int_(puzzle::Key_E);
  m.attr("Key_W") = py::int_(puzzle::Key_W);
  m.attr("Key_Left") = py::int_(puzzle::Key_Left);
  m.attr("Key_Right") = py::int_(puzzle::Key_Right);
  m.attr("Key_PageUp") = py::int_(puzzle::Key_PageUp);
  m.attr("Key_PageDown") = py::int_(puzzle::Key_PageDown);
  m.attr("Key_I") = py::int_(puzzle::Key_I);
  // Colors
  m.def("color_to_str", &puzzle::color_to_str,
        "convert a color from int to its string");
  m.attr("white") = py::int_(puzzle::white);
  m.attr("black") = py::int_(puzzle::black);
  m.attr("red") = py::int_(puzzle::red);
  m.attr("darkRed") = py::int_(puzzle::darkRed);
  m.attr("green") = py::int_(puzzle::green);
  m.attr("darkGreen") = py::int_(puzzle::darkGreen);
  m.attr("blue") = py::int_(puzzle::blue);
  m.attr("darkBlue") = py::int_(puzzle::darkBlue);
  m.attr("cyan") = py::int_(puzzle::cyan);
  m.attr("darkCyan") = py::int_(puzzle::darkCyan);
  m.attr("magenta") = py::int_(puzzle::magenta);
  m.attr("darkMagenta") = py::int_(puzzle::darkMagenta);
  m.attr("yellow") = py::int_(puzzle::yellow);
  m.attr("darkYellow") = py::int_(puzzle::darkYellow);
  m.attr("gray") = py::int_(puzzle::gray);
  m.attr("darkGray ") = py::int_(puzzle::darkGray);
  m.attr("lightGray") = py::int_(puzzle::lightGray);
  // Consts
  m.attr("INVALID_COLOR") = py::int_(puzzle::SpinMarble::INVALID_COLOR);
  m.attr("INVALID_ID") = py::int_(puzzle::SpinMarble::INVALID_ID);
  // =================================================================== //
}