# ESP-UI PC Simulator with VSCode

* [English Version](./README.md)

esp-ui-simulator_vscode 是一个能够在 PC 上运行 [LVGL](https://github.com/lvgl/lvgl) 和 [esp-ui](https://github.com/espressif/esp-ui) 的 [VSCode](https://code.visualstudio.com) 工程，用于模拟运行 esp-ui 的系统 UI，方便用户调试样式表以及 app。

> [!WARNING]
> * esp-ui-simulator_vscode 仅在 `Linux` 和 `MacOS` 上进行了测试，但也可能在 `Windows WSL` 上工作。

## 依赖项

### 编译工具

请确保系统中已安装 `GCC`  `GDB` `bear` 和 `make`，所有的需求都已在 [.workspace](./esp-ui-simulator_vscode.code-workspace) 文件中预配置好（只需双击此文件打开工程）。

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
git clone --recursive https://github.com/esp-arduino-libs/esp-ui-simulator_vscode
```

2. 双击打开 VSCode 工作区文件 [esp-ui-simulator_vscode.code-workspace](./esp-ui-simulator_vscode.code-workspace) 打开工程。

3. 在 [components/esp-ui-app](./components/esp-ui-app) 目录下存放 apps，并在 [main/main.cpp](./main/main.cpp) 的 `main()` 函数中进行创建和安装。

4. 按 F5 构建和调试，此时 esp-ui 的系统 UI 应该会显示在一个新窗口中。同时，用户可以通过 GDB 访问 VSCode 的所有调试功能。

> [!NOTE]
> * 为了在模拟器和设备代码之间进行临时修改，Makefile 中已添加了 `SIMULATOR=1` 全局定义。

> [!WARNING]
> * 请使用工作区文件 [esp-ui-simulator_vscode.code-workspace](./esp-ui-simulator_vscode.code-workspace) 打开工程，因为它包含了所有编译和调试的配置。否则，你将无法直接使用 `F5` 键进行编译和调试。
