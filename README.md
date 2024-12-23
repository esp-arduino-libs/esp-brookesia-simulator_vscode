# ESP-Brookesia PC VSCode Simulator

[![Build and Run Test Application](https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode/actions/workflows/run_test.yml/badge.svg?branch=master)](https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode/actions/workflows/run_test.yml)

* [中文版本](./README_CN.md)

## Table of Contents

- [ESP-Brookesia PC VSCode Simulator](#esp-brookesia-pc-vscode-simulator)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Installing Dependencies](#installing-dependencies)
    - [Windows](#windows)
    - [Linux](#linux)
  - [Usage](#usage)

## Overview

The `esp-brookesia-simulator_vscode` is a VSCode project that can run [LVGL](https://github.com/lvgl/lvgl) and [ESP-Brookesia](https://github.com/espressif/esp-brookesia) on a PC, simulating the system UI of ESP-Brookesia, which facilitates users in debugging styles and apps.

The `esp-brookesia-simulator_vscode` is a port and modification based on [lv_port_pc_vscode](https://github.com/lvgl/lv_port_pc_vscode) to support compiling C++ files.

> [!NOTE]
> It has been tested that `esp-brookesia-simulator_vscode` can run on `Linux`, `MacOS`, `Windows`, and `Windows WSL` platforms.

## Installing Dependencies

### Windows

It is recommended that users install `gcc`, `gdb`, `cmake`, `make`, and `sdl2` through [MSYS2](https://www.msys2.org/). Below are the installation steps for a 64-bit Windows system:

1. Download ([Official Link](https://www.msys2.org/) / [Mirror Link](https://mirrors.tuna.tsinghua.edu.cn/msys2/distrib/x86_64/)) and install MSYS2.

2. Open the `MSYS2 MINGW64` terminal and execute the following commands:

```bash
pacman -Syu
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2 mingw-w64-x86_64-cmake
```

3. Add the path *C:\\<install_path>\msys64\mingw64\bin* to the system environment variable `PATH` (where `<install_path>` is the installation path of the software).

### Linux

Users can directly install `gcc`, `gdb`, `cmake`, `make`, and `sdl2` through the terminal:

```bash
sudo apt-get update && sudo apt-get install -y gcc gdb cmake make build-essential libsdl2-dev
```

## Usage

1. Clone the project and related submodules:

```bash
git clone --recursive https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode
```

2. Double-click to open the VSCode workspace file *[simulator.code-workspace](./simulator.code-workspace)* to open the project.

> [!WARNING]
> Please use the workspace file to open the project, as it contains all the configurations for compiling and debugging. Otherwise, you will not be able to compile and debug directly using the `F5` key.

3. Press F5 to build and debug, at which point the system UI of ESP-Brookesia should appear in a new window. Meanwhile, users can access all debugging features of VSCode through GDB.

4. Adjust project configuration

    - Modify the definition in the *[CMakeLists.txt](./CMakeLists.txt#L7)* file to adjust the display window resolution:
    ```cmake
    set(DISP_DEF "-DDISP_HOR_RES=1024 -DDISP_VER_RES=600")  # Resolution of the display
    ```
    - Adjust the LVGL configuration by modifying definitions in the *[components/lv_conf.h](./components/lv_conf.h)* file.
    - Adjust the ESP-Brookesia configuration by modifying definitions in the *[components/esp_brookesia_conf.h](./components/esp_brookesia_conf.h)* file.

> [!NOTE]
> A global definition `SIMULATOR=1` has been added to *[CMakeLists.txt](./CMakeLists.txt)* for temporary modifications between the simulator and device code.

5. Add custom stylesheets and apps

    - By default, the project will compile `.c` and `.cpp` files under the *[components/esp-brookesia-app](./components/esp-brookesia-app)* directory and include this directory as a header file directory. Users can place custom apps in this directory.
    - By default, the project will compile `.c` and `.cpp` files under the *[components/esp-brookesia-stylesheet](./components/esp-brookesia-stylesheet)* directory and include this directory as a header file directory. Users can place custom stylesheets in this directory.
