# vss-unification 🇧🇷 🤖⚽

### **Overview**

- Operational System: [Ubuntu](https://ubuntu.com/) 20.04 LTS or above:
    - _The operating systems mentioned above were used in the development of this repository and other repositories that
      were used as sub-modules. Any other operating system may require different installation scripts than those
      provided;_

- This repository contains submodules:
    - If you want to clone a repository including its submodules you can use the  `--recursive`  parameter.

```
git clone --recursive [URL to Git repo]
```

-
    - If you already have cloned a repository and now want to load its submodules you have to use  `submodule update`.

```
git submodule update --init
# if there are nested submodules:
git submodule update --init --recursive
```

### **Dependencies**

- [CMake](https://cmake.org/) 3.15 or above;
- [C++20](https://en.cppreference.com/w/cpp/20) or above:
    - _gcc ≥ 11;_
    - _Some small features used were merged into C++20;_
- [Qt Framework](https://www.qt.io/) 6.1.0 or above:
    - _Some dependencies require Qt;_
    - _Qt provides several libraries that facilitate the development of the interface, in addition to powerful classes,
      namespaces, macros and more;_
- [Google Protobuf](https://developers.google.com/protocol-buffers);
    - Protocol buffers are Google's language-neutral, platform-neutral, extensible mechanism for serializing structured
      data;
- OpenGL;
- [boost](https://www.boost.org/):
    - _Boost libraries are intended to be widely useful, and usable across a broad spectrum of applications;_

> all required dependencies can be installed using `scripts/setup.py`

### Building (Linux only)

- First clone into your preferred location:

```
cd /path/to/repository/
git clone git@github.com:robocin/vss-base-software.git
cd vss-base-software
git submodule update --init --recursive
```

Create a build directory within the project (this is ignored by .gitignore):

```
mkdir build
cd build
```

Run CMake to generate the makefile:

```
cmake ..
```

Then compile the program (for a safe multithreading compilation use: `-j$(grep -c ^processor /proc/cpuinfo)`):

```
make -j$(grep -c ^processor /proc/cpuinfo)
```

The executable will be located on the  `bin`  directory.

### How to Contribute

It is recommended to use the [CLion](https://www.jetbrains.com/clion/download/) IDE;

---
> 2021, José Cruz
