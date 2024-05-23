# ESP-UI PC Simulator with VSCode

* [中文版本](./README_CN.md)

esp-ui-simulator_vscode is a [VSCode](https://code.visualstudio.com) project that enables running [LVGL](https://github.com/lvgl/lvgl) and [esp-ui](https://github.com/espressif/esp-ui) on a PC. It is used to simulate the esp-ui system UI, making it convenient for users to debug stylesheets and apps.

> [!WARNING]
> * esp-ui-simulator_vscode has been tested only on `Linux` and `MacOS`, but may also work on `Windows WSL`.

## Dependencies

### Build Tools

Ensure that `GCC`, `GDB`, `bear`, and `make` are installed on your system. All requirements are pre-configured in the [.workspace](./esp-ui-simulator_vscode.code-workspace) file (simply double-click this file to open the project).

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
git clone --recursive https://github.com/esp-arduino-libs/esp-ui-simulator_vscode
```

2. Double-clicking the VSCode workspace file [esp-ui-simulator_vscode.code-workspace](./esp-ui-simulator_vscode.code-workspace) to open the project.

3. Place your apps in the [components/esp-ui-app](./components/esp-ui-app) directory and create and install them in the `main()` function in [main/main.cpp](./main/main.cpp).

4. Press F5 to build and debug. The system UI of esp-ui should appear in a new window, and you can access all debugging features of VSCode through GDB.

> [!NOTE]
> * To allow temporary modifications between the simulator and device code, a global definition `SIMULATOR=1` has been added to the Makefile.

> [!WARNING]
> * Please use the workspace file [esp-ui-simulator_vscode.code-workspace](./esp-ui-simulator_vscode.code-workspace) to open the project, as it contains all the configurations for building and debugging. Otherwise, you won't be able to directly use the `F5` key to build and debug.
