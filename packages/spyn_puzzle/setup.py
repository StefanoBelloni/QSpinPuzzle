from setuptools import setup, find_packages

setup(
    name="spyn_puzzle",
    version="0.0.1",
    author="Stefano Belloni",
    author_email="bulbasch@kotik.by",
    description="wrapper for the c++ version build with pybind11: this package load the PYTHONPATH and add to the DLL search path",
    packages=find_packages(),
    include_package_data=True,
)