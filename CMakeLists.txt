cmake_minimum_required(VERSION 3.5)

project(QSpinPuzzle VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

option(USE_QT "Enable this if you want to use QT" no)

add_subdirectory(extern/pybind11)

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

if ("${RUN_COVERAGE}" STREQUAL TRUE)
    # https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
endif()


list(APPEND CMAKE_PREFIX_PATH "~/Qt/6.6.1/gcc_64")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (USE_QT)
    find_package(
        QT NAMES Qt6 Qt5 
        COMPONENTS Widgets LinguistTools
    )
    find_package(
        Qt${QT_VERSION_MAJOR} 
        COMPONENTS Widgets LinguistTools
    )
endif(USE_QT)

# find_package (
    # Eigen3 3.3
    # REQUIRED NO_MODULE
# )

set(TS_FILES resources/QSpinPuzzle_en_IE.ts)

set(PROJECT_SOURCES
        # =========================== #
        src/main.cpp
        # =========================== #
        src/widgets/spin_puzzle_window.cpp
        src/widgets/spin_puzzle_window.h
        # =========================== #
        ${TS_FILES}
        # =========================== #
)

# ============================================== #
# LIBRARY FOR SPIN PUZZLE GAME
# ============================================== #
add_library(spinpuzzle SHARED
    src/puzzle/spin_marble.h 
    src/puzzle/spin_marble.cpp
    src/puzzle/spin_puzzle_side.h 
    src/puzzle/spin_puzzle_side.cpp
    src/puzzle/spin_puzzle_game.h 
    src/puzzle/spin_puzzle_game.cpp
    src/puzzle/spin_puzzle_definitions.h
    src/puzzle/spin_puzzle_definitions.cpp
    src/puzzle/spin_action_provider.cpp
    src/puzzle/spin_action_provider.h
)

# ============================================================================ #
# pybind11 method:
# ============================================================================ #
if(NOT DEFINED ANDROID OR (NOT "${RUN_COVERAGE}" STREQUAL TRUE))
    pybind11_add_module(
        spyn_puzzle 
        src/pybind11/puzzle.cpp
        )

    target_include_directories(
        spyn_puzzle 
        PRIVATE 
        src
    )

    target_link_libraries(
        spyn_puzzle 
        PUBLIC 
        spinpuzzle
    )
endif()
# ============================================================================ #

## ========================================================================== ##
##                              TESTS WITH GTEST                              ##
## ========================================================================== ##

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# if (WIN32)
if ("${RUN_COVERAGE}" STREQUAL TRUE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif()

enable_testing()

include(CTest)
include(GoogleTest)

add_executable(
  t_puzzle
  tests/t_marbles.cpp
  tests/t_puzzle_side.cpp
  tests/t_puzzle_game.cpp
  tests/t_definition.cpp
)
target_include_directories(
    t_puzzle
    PRIVATE 
    src
)
target_link_libraries(
  t_puzzle
  GTest::gtest_main
  spinpuzzle
)

add_test(t_puzzle t_puzzle)
# gtest_discover_tests(t_puzzle)

if ("${RUN_COVERAGE}" STREQUAL TRUE)
    # https://stackoverflow.com/questions/13116488/detailed-guide-on-using-gcov-with-cmake-cdash
    set(
	    COVERAGE_EXCLUDES 
	    "${CMAKE_CURRENT_BINARY_DIR}/_deps/*"                      # gtest
	    "${CMAKE_CURRENT_BINARY_DIR}/../extern/*"                  # pybind11
	    "${CMAKE_CURRENT_BINARY_DIR}/../src/pybind11/puzzle.cpp"   # py library def
	    )   # exclude gtest
    setup_target_for_coverage_gcovr_html(
        NAME t_puzzle_html
        EXECUTABLE t_puzzle
    )
endif()

# ============================================================================ #
# Qt installation available
# ============================================================================ #
if (USE_QT)
    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(QSpinPuzzle
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            # =========================== #
            src/widgets/spin_puzzle_widget.h 
            src/widgets/spin_puzzle_widget.cpp
            # =========================== #
            README.md
            # =========================== #
            images/spin_puzzle.png
            images/QSpinPuzzle_Front.png
            # =========================== #
        )

        target_include_directories(
            QSpinPuzzle 
            PRIVATE 
            src
        )

    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET QSpinPuzzle APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

        qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    else()
        if(ANDROID)
            add_library(QSpinPuzzle SHARED
                ${PROJECT_SOURCES}
            )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
        else()
            add_executable(QSpinPuzzle
                ${PROJECT_SOURCES}
            )
        endif()

        qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    endif()

    target_link_libraries(
        spinpuzzle
        PRIVATE
        # Qt${QT_VERSION_MAJOR}::Widgets
    )

    target_link_libraries(
        QSpinPuzzle 
        PRIVATE 
        spinpuzzle
        Qt${QT_VERSION_MAJOR}::Widgets
    )

    # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
    # If you are developing for iOS or macOS you should consider setting an
    # explicit, fixed bundle identifier manually though.
    if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QSpinPuzzle)
    endif()
    set_target_properties(QSpinPuzzle PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )

    include(GNUInstallDirs)
    install(TARGETS QSpinPuzzle
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(QSpinPuzzle)
    endif()

endif(USE_QT)
# ============================================================================ #
