# ESP-Brookesia PC VSCode 模拟器

[![Build Test Application](https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode/actions/workflows/build_test.yml/badge.svg)](https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode/actions/workflows/build_test.yml)

* [English Version](./README.md)

## 目录

- [ESP-Brookesia PC VSCode 模拟器](#esp-brookesia-pc-vscode-模拟器)
  - [目录](#目录)
  - [概述](#概述)
  - [安装依赖项](#安装依赖项)
    - [Windows](#windows)
    - [Linux](#linux)
  - [使用方法](#使用方法)

## 概述

esp-brookesia-simulator_vscode 是一个能够在 PC 上运行 [LVGL](https://github.com/lvgl/lvgl) 和 [ESP-Brookesia](https://github.com/espressif/esp-brookesia) 的 [VSCode](https://code.visualstudio.com) 工程，用于模拟运行 ESP-Brookesia 的系统 UI，方便用户调试样式表以及 app。

esp-brookesia-simulator_vscode 是参考 [lv_port_pc_vscode](https://github.com/lvgl/lv_port_pc_vscode) 进行的移植和修改，从而支持编译 C++ 文件。

> [!NOTE]
> 经测试，esp-brookesia-simulator_vscode 可以运行在 `Linux`、`MacOS`、`Windows` 和 `Windows WSL` 平台。

## 安装依赖项

### Windows

推荐用户通过 [MSYS2](https://www.msys2.org/) 安装 `MinGW` 的 `gcc`、`gdb`、`cmake` 、`make` 和 `sdl2`。下面以 64 位 Windows 系统为例，安装步骤如下：

1. 下载（[官方链接](https://www.msys2.org/) / [镜像链接](https://mirrors.tuna.tsinghua.edu.cn/msys2/distrib/x86_64/)）并安装 MSYS2。

2. 打开 MSYS2 的 `MSYS2 MINGW64` 终端，执行以下命令：

```bash
pacman -Syu
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2 mingw-w64-x86_64-cmake
```

3. 在系统环境变量 `PATH` 中添加路径 *C:\\<install_path>\msys64\mingw64\bin* (其中，`<install_path>` 是软件的安装路径)。

### Linux

用户可以直接通过终端安装 `gcc`、`gdb`、`cmake`、`make` 和 `sdl2`：

```bash
sudo apt-get update && sudo apt-get install -y gcc gdb cmake make build-essential libsdl2-dev
```

## 使用方法

1. 克隆工程及相关子模块：

```bash
git clone --recursive https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode
```

2. 双击打开 VSCode 工作区文件 *[simulator.code-workspace](./simulator.code-workspace)* 打开工程。

> [!WARNING]
> 请使用工作区文件打开工程，因为它包含了所有编译和调试的配置。否则，你将无法直接使用 `F5` 键进行编译和调试。

3. 按 F5 构建和调试，此时 ESP-Brookesia 的系统 UI 应该会显示在一个新窗口中。同时，用户可以通过 GDB 访问 VSCode 的所有调试功能。

4. （可选）调整工程的配置

    - 通过修改 *[CMakeLists.txt](./CMakeLists.txt#L7)* 文件中如下的定义来调整显示窗口的分辨率：
    ```cmake
    set(DISP_DEF "-DDISP_HOR_RES=1024 -DDISP_VER_RES=600")  # Resolution of the display
    ```
    - 通过修改 *[components/lv_conf.h](./components/lv_conf.h)* 文件中的定义来调整 LVGL 的配置。
    - 通过修改 *[components/esp_brookesia_conf.h](./components/esp_brookesia_conf.h)* 文件中的定义来调整 ESP-Brookesia 的配置。

> [!NOTE]
> 为了在模拟器和设备代码之间进行临时修改，*[CMakeLists.txt](./CMakeLists.txt)* 中已添加了 `SIMULATOR=1` 全局定义。

5. （可选）添加自定义样式表和 app

    - 工程默认会编译 *[components/esp-brookesia-app](./components/esp-brookesia-app)* 目录下的 `.c` 和 `.cpp` 文件，并包含此目录作为头文件目录，用户可以将自定义 apps 放置在此目录下。
    - 工程默认会编译 *[components/esp-brookesia-stylesheet](./components/esp-brookesia-stylesheet)* 目录下的 `.c` 和 `.cpp` 文件，并包含此目录作为头文件目录，用户可以将自定义样式表放置在此目录下。
