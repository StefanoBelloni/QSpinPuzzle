from setuptools import setup, find_packages

setup(
    name="qspyn_puzzle",
    version="0.0.1",
    author="Stefano Belloni",
    author_email="bulbasch@kotik.by",
    description="Qt UI for the implementation of the logic game 'SpinPuzzleGame'",
    packages=find_packages(),
    install_requires=['pyside6'],
    include_package_data=True,
)