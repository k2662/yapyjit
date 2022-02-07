import setuptools
import glob


with open("README.md", "r") as fh:
    long_description = fh.read()


setuptools.setup(
    name="yapyjit",
    version="0.0.1a1",
    author="flandre.info",
    author_email="flandre@scarletx.cn",
    description="Yet Another Python JIT",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com",
    include_package_data=True,
    ext_modules=[setuptools.Extension(
        "yapyjit",
        glob.glob("src/**/*.c", recursive=True)
        + glob.glob("src/**/*.cpp", recursive=True),
    )],
    include_dirs=["include"],
    classifiers=[
        "Programming Language :: Python :: 3 :: Only",
        "License :: OSI Approved :: Apache Software License"
    ],
    python_requires='~=3.6'
)
