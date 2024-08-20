# Building vSoC

vSoC can run on Windows and (experimentally) macOS.

## Windows

1. Download and install MSYS2 following the instructions at [https://www.msys2.org/](https://www.msys2.org/). Do **NOT** install MSYS2 in paths that contain spaces, e.g., C:/Program Files.

2. Open the MSYS2 MinGW x64 terminal (**NOT** MSYS2 MSYS terminal) and install dependencies using:

```bash
pacman -S base-devel git unzip wget mingw-w64-x86_64-binutils mingw-w64-x86_64-crt-git mingw-w64-x86_64-headers-git mingw-w64-x86_64-gcc-libs mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-make mingw-w64-x86_64-tools-git mingw-w64-x86_64-pkg-config mingw-w64-x86_64-winpthreads-git mingw-w64-x86_64-libwinpthread-git mingw-w64-x86_64-winstorecompat-git mingw-w64-x86_64-libmangle-git mingw-w64-x86_64-pixman mingw-w64-x86_64-SDL2 mingw-w64-x86_64-glib2 mingw-w64-x86_64-capstone mingw-w64-x86_64-lzo2 mingw-w64-x86_64-libxml2 mingw-w64-x86_64-libjpeg-turbo mingw-w64-x86_64-libpng mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-ninja

cp /mingw64/bin/ar.exe /mingw64/bin/x86_64-w64-mingw32-ar.exe & cp /mingw64/bin/ranlib.exe /mingw64/bin/x86_64-w64-mingw32-ranlib.exe & cp /mingw64/bin/windres.exe /mingw64/bin/x86_64-w64-mingw32-windres.exe & cp /mingw64/bin/objcopy.exe /mingw64/bin/x86_64-w64-mingw32-objcopy.exe  & cp /mingw64/bin/nm.exe /mingw64/bin/x86_64-w64-mingw32-nm.exe & cp /mingw64/bin/strip.exe /mingw64/bin/x86_64-w64-mingw32-strip.exe
```

3. Pull down the source code of vSoC along with the dependencies with:

```bash
git clone https://github.com/VirtualSoC/vsoc.git
cd vsoc
git submodule update --init --recursive
cd ..
```

4. Download vSoC external dependencies. [`glfw-touch`](https://github.com/torkeldanielsson/glfw/tree/touch) and [`cimgui`](https://github.com/cimgui/cimgui) are not available in msys2, and you need to build them by yourselves. Alternatively, we provide prebuilt binaries to ease your building. Download the prebuilt deps with the following commands.

```bash
wget https://github.com/VirtualSoC/vsoc/releases/download/deps/deps.zip
unzip deps.zip
``` 

5. Configure and build the emulator. It typically takes ~15 minutes to complete, depending on the hardware.

```bash
export BASE_DIR=`pwd`
export LIBRARY_PATH="$BASE_DIR/lib/"
export PKG_CONFIG_PATH="$BASE_DIR/lib/pkgconfig/"

cd vsoc
./configure --cross-prefix=x86_64-w64-mingw32- --extra-cflags="-I$BASE_DIR/include" --extra-ldflags="-L$BASE_DIR/lib" --prefix="$BASE_DIR" --bindir="$BASE_DIR/bin" --disable-gtk --enable-sdl --target-list=x86_64-softmmu --disable-werror
make install -j$(nproc)
cd ..
```

Note: `./configure` may report an error like this: `OSError: [WinError 1314] a required privilege is not held by the client`. That error does not affect the building and functioning of vSoC and can be ignored.

6. When the build completes, please extract the guest image as well as `run.sh` from the vSoC binary.

```bash
wget https://github.com/VirtualSoC/vsoc/releases/download/beta-2408/vsoc_android_win_amd64.zip
unzip vsoc_android_win_amd64.zip img/* run.sh
```

7. Use `run.sh` to start vSoC.

```bash
bash run.sh
```

## macOS

1. Download and install Homebrew following the instructions at [https://brew.sh/](https://brew.sh/).

2. Open the macOS terminal and install dependencies using:

```bash
brew install libffi gettext pkg-config autoconf automake pixman ffmpeg
```

3. Pull down vSoC source code with:

```bash
git clone https://github.com/VirtualSoC/vsoc.git
cd vsoc
git submodule update --init --recursive
```

4. Configure and build the emulator. It typically takes ~15 minutes to complete, depending on the hardware.

```bash
./configure --enable-hvf --target-list=x86_64-softmmu,aarch64-softmmu --disable-werror
make install -j$(nproc)
```

5. When the build completes, you can extract ...

## Guest Images

Unfortunately, the source code of the guest systems is complex and involves tens of repositories, and we are still going through internal procedures for open source code.

Therefore, the implementation of the guest systems stays proprietary for now. We will provide the guest source code as soon as we go through the formalities.
