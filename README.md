# ESP-Brookesia PC Simulator with VSCode

* [中文版本](./README_CN.md)

esp-brookesia-simulator_vscode is a [VSCode](https://code.visualstudio.com) project that enables running [LVGL](https://github.com/lvgl/lvgl) and [ESP-Brookesia](https://github.com/espressif/esp-brookesia) on a PC. It is used to simulate the esp-brookesia system UI, making it convenient for users to debug stylesheets and apps.

esp-brookesia-simulator_vscode is a port and modification based on [lv_port_pc_vscode](https://github.com/lvgl/lv_port_pc_vscode), designed to support the compilation of C++ files.

> [!WARNING]
> * esp-brookesia-simulator_vscode has been tested only on `Linux`, `MacOS` and `Windows WSL`.

## Dependencies

### Build Tools

On Linux, you can install `GCC`, `GDB`, `bear` and `make` via terminal:

```bash
sudo apt-get update && sudo apt-get install -y gcc gdb bear make
```

### Graphics Drivers

This project supports using **SDL** or **X11** as the LVGL display driver for low-level graphics/mouse/keyboard support. This can be configured in the [Makefile](./Makefile).

#### Install SDL

On Linux, you can install `SDL` via terminal:

```bash
sudo apt-get update && sudo apt-get install -y build-essential libsdl2-dev
```

SDL can also be downloaded from https://www.libsdl.org/.

#### Install X11

On Linux, you can install `X11` via terminal:

```bash
sudo apt-get update && sudo apt-get install -y libx11-dev
```

## Usage

1. Clone the project and related submodules:

```bash
git clone --recursive https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode
```

2. Double-clicking the VSCode workspace file [esp-brookesia-simulator_vscode.code-workspace](./esp-brookesia-simulator_vscode.code-workspace) to open the project.

3. Press F5 to build and debug. The system UI of ESP-Brookesia should appear in a new window, and you can access all debugging features of VSCode through GDB.

> [!NOTE]
> * To allow temporary modifications between the simulator and device code, a global definition `SIMULATOR=1` has been added to the Makefile.
> * The project will by default compile the `.c` 和 `.cpp` files located in the [components/esp-brookesia-app](./components/esp-brookesia-app) directory and include this directory as a header file directory. Users can place their custom apps in this directory.
> * The project will by default compile the `.c` 和 `.cpp` files located in the [components/esp-brookesia-stylesheet](./components/esp-brookesia-stylesheet) directory and include this directory as a header file directory. Users can place their custom stylesheets in this directory.

> [!WARNING]
> * Please use the workspace file [esp-brookesia-simulator_vscode.code-workspace](./esp-brookesia-simulator_vscode.code-workspace) to open the project, as it contains all the configurations for building and debugging. Otherwise, you won't be able to directly use the `F5` key to build and debug.
