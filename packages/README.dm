### PYTHON PACKAGES

Evan though this is not a standard way to handle packages in a single directory
here there are three different packages:

* `spinbind11_puzzle`: the core of the implementation of the SpinPuzzleGame.
* `spyn_puzzle`: this is a wrapper of  `spinbind11_puzzle`. It's only use is to 
   `os.add_dll_directory(f)` from the env. variable `PYTHONPATH`. Mostly for 
   Windows, that cannot find the shared library used during compilation with
   MingW64 (installed with the [MSYS2 project](https://www.msys2.org/) )
* `qspyn_puzzle`: is the UI developed with Qt for python.

**NOTE**: To properly install this project on Windows you can have a look at the script:
`scripts/compile_win.ps1`
