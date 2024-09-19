# ESP-Brookesia PC Simulator with VSCode

* [English Version](./README.md)

esp-brookesia-simulator_vscode 是一个能够在 PC 上运行 [LVGL](https://github.com/lvgl/lvgl) 和 [ESP-Brookesia](https://github.com/espressif/esp-brookesia) 的 [VSCode](https://code.visualstudio.com) 工程，用于模拟运行 ESP-Brookesia 的系统 UI，方便用户调试样式表以及 app。

esp-brookesia-simulator_vscode 是参考 [lv_port_pc_vscode](https://github.com/lvgl/lv_port_pc_vscode) 进行的移植和修改，从而支持编译 C++ 文件。

> [!WARNING]
> * esp-brookesia-simulator_vscode 仅在 `Linux`、`MacOS` 和  `Windows WSL` 上进行了测试。

## 依赖项

### 编译和调试工具

在 Linux 上，用户可以通过终端安装 `GCC`、`GDB`、`bear` 和 `make`：

```bash
sudo apt-get update && sudo apt-get install -y gcc gdb bear make
```

### 图形驱动

该工程支持使用 **SDL** 或 **X11** 作为 LVGL 的显示驱动程序，用于底层图形/鼠标/键盘支持，可以在 Makefile 中修改定义。

#### 安装 SDL

在 Linux 上，用户可以通过终端安装：

```bash
sudo apt-get update && sudo apt-get install -y build-essential libsdl2-dev
```

用户可以从 https://www.libsdl.org/ 下载 SDL。

#### 安装 X11

在 Linux 上，用户可以通过终端安装：

```bash
sudo apt-get update && sudo apt-get install -y libx11-dev
```

## 使用方法

1. 克隆工程及相关子模块：

```bash
git clone --recursive https://github.com/esp-arduino-libs/esp-brookesia-simulator_vscode
```

2. 双击打开 VSCode 工作区文件 [esp-brookesia-simulator_vscode.code-workspace](./esp-brookesia-simulator_vscode.code-workspace) 打开工程。

3. 按 F5 构建和调试，此时 ESP-Brookesia 的系统 UI 应该会显示在一个新窗口中。同时，用户可以通过 GDB 访问 VSCode 的所有调试功能。

> [!NOTE]
> * 为了在模拟器和设备代码之间进行临时修改，Makefile 中已添加了 `SIMULATOR=1` 全局定义。
> * 工程默认会编译 [components/esp-brookesia-app](./components/esp-brookesia-app) 目录下的 `.c` 和 `.cpp` 文件，并包含此目录作为头文件目录，用户可以将自定义 apps 放置在此目录下。
> * 工程默认会编译 [components/esp-brookesia-stylesheet](./components/esp-brookesia-stylesheet) 目录下的 `.c` 和 `.cpp` 文件，并包含此目录作为头文件目录，用户可以将自定义样式表放置在此目录下。

> [!WARNING]
> * 请使用工作区文件 [esp-brookesia-simulator_vscode.code-workspace](./esp-brookesia-simulator_vscode.code-workspace) 打开工程，因为它包含了所有编译和调试的配置。否则，你将无法直接使用 `F5` 键进行编译和调试。
