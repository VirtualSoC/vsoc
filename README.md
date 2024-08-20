# vSoC: Efficient Virtual System-on-Chip on Heterogeneous Hardware

vSoC is the first virtual mobile SoC that enables virtual devices to efficiently share data through a unified SVM framework.

### 1. Getting Started with vSoC

#### 1.1. Hardware Requirements

vSoC works best on a Windows machine with an Intel x64 CPU and an NVIDIA dedicated GPU with a recent driver version (>550.00).

**Minimal** hardware configurations include:
  - 4-core CPU
  - 8 GB DDR4 memory
  - 1920x1080 display
  - 128 GB storage

Note: other hardware/software combinations (e.g. Intel GPUs or macOS) also work, but they are less tested and are prone to bugs and inefficiencies. We plan to improve the cross-platform compatibility of vSoC with time.

#### 1.2 Software Prerequisites
  1. Ensure that you have turned on Intel-VT in the BIOS settings. It is turned on for most PCs by default.
  2. Enable Hyper-V in Windows (check the [Microsoft documentation](https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/quick-start/enable-hyper-v) for details).

### 2. Installation

#### 2.1. Download vSoC

You can download vSoC binary in [Github releases](https://github.com/VirtualSoC/vsoc/releases/tag/beta-2408). Download and extract the zip file.

#### 2.2 Run vSoC

After extracting, you should see a `run.cmd` script file in the root directory of the binary. To run vSoC, double click on the script. A command line window should pop up immediately, and after a few seconds, a GUI window should appear.

Use vSoC in the same way that you use mobile phones. We implement an emulated touchscreen and an emulated keyboard so that you can interact with the emulator directly with your keyboard and mouse.

To shutdown vSoC, click the upper-right `×` button, and then click the `Power off` button inside the emulator. If vSoC is not responding, close the command line window to shut down by force.

#### 2.3. Advanced Usage and Troubleshooting

* For establishing `adb` connection with vSoC, please type `adb connect 127.0.0.1:5555` in your terminal. If you have not installed `adb`, please refer to [this guide](https://www.xda-developers.com/install-adb-windows-macos-linux/).

* If you want to change screen resolution, please open `run.cmd` and change the `display_width` and `display_height` parameters.

* If vSoC cannot function properly, please power it off and then restart it with `run.cmd`.

* If vSoC is not booting, you can try to erase the disk by replacing the `\img\userdata.qcow2` file with the original version in the zip file. **WARNING: this action will wipe everything in the emulator, including the apps you install!**

* If the above does not work, please file an Github issue or contact us and provide the `log.txt` file in the root directory of the binary.

### 3. Building

See [build-vsoc.md](docs/build-vsoc.md) for detailed instructions.

### 4. Developing vSoC

### 4.1. Code Organization

vSoC is based on QEMU 7.1, and most of the code is from the upstream QEMU. vSoC primarily adds a new QEMU device which consists of several modules with codenames `express-*`. The codename convention is inherited from [Trinity Emulator](https://github.com/TrinityEmulator/TrinityEmulator), a major source of inspiration for this work.

|  Module  |  Purpose  |
|  ----  | ----  |
| `hw/express-mem` | The module that implements the unified SVM framework for vSoC virtual devices. The SVM framework includes the hypergraphs responsible for statistic tracking, the prefetch engine that optimizes SVM performance, and the virtual command fences which enables efficient ordering of asynchronous shared resource operations among vSoC devices. |
| `hw/express-camera` | vSoC camera virtual device, implemented using `libavdevice`, a cross-platform library for manipulating peripheral devices. |
| `hw/express-codec` | vSoC codec/ISP virtual device, implemented using libavcodec, a widely used codec library that supports software and hardware decoding/encoding. We further use the OpenGL interop extensions to achieve in- GPU video rendering with certain video formats. |
| `hw/express-gpu` | vSoC GPU/display virtual device, implemented using Trinity's graphics projection technique. The display is implemented with glfw, a cross-platform library for window management. |
| `hw/express-input` | vSoC input virtual devices, implemented with GLFW events. |
| `hw/express-network` | vSoC cellular modem virtual device, implemented with reference to Google Android Emulator’s telephony module. |
| `hw/express-sensor` | vSoC sensor virtual devices, emulated purely in software. |
| `hw/teleport-express` | vSoC transport library based on `virtio`, but with additional functionalities like asynchronous commands, cluster commands and virtual interrupts. |

### 4.2. Debugging vSoC

To debug vSoC, you should first set up the development environment detailed in [build-vsoc.md](docs/build-vsoc.md). Then, you can use the `gdb` or `gdb-multiarch` debugger in the MSYS2 MinGW x64 terminal.

### 7. Licensing
Our code is under the GPLv2 license.
