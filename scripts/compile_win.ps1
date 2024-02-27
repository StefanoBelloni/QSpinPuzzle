$ErrorActionPreference = "Stop"

set-Location ${PSScriptRoot}\..

Write-Output ${PWD}

$root = "${PSScriptRoot}\.."
$path = "${PSScriptRoot}\..\packages\spinbind11_puzzle\build"
$venv = "${PSScriptRoot}\..\venv"
$activate = "${venv}\Scripts\Activate.ps1"
$packages = "${venv}\Lib\site-packages\"

$p1 = "${PSScriptRoot}\..\packages\spinbind11_puzzle"
$p2 = "${PSScriptRoot}\..\packages\spyn_puzzle"
$p3 = "${PSScriptRoot}\..\packages\qappy"

If(!(test-path -PathType container ${path}))
{
      New-Item -ItemType Directory -Path ${path}
}

If(!(test-path -PathType container ${venv}))
{
    python -m venv ${venv}
}

invoke-expression -Command ${activate}


Set-Location ${path}

cmake ${root}
cmake --build .

Set-Location ${root}

pip install ${p1}
pip install ${p2}
pip install ${p3}

Copy-Item ${path}\libspinpuzzle.dll   -Destination ${packages}
Copy-Item ${path}\libspinpuzzle.dll.a -Destination ${packages}

python tests\test_marbles.py
python tests\test_puzzle_side.py

python -m qspyn_puzzle
