# Midi Fighter 64 performance-optimized Custom Firmware

This repository contains the source code of my custom firmware for the [Midi Fighter 64](https://store.djtechtools.com/products/midi-fighter-64). Critically, this firmware enables Apollo Studio support and greatly enhances Ableton Live-based performances. The modification is easy to install, free to use and works with every currently existing Ableton Live project file.

## Installation

Download the latest custom firmware file with the desired patches from the [Launchpad Utility](https://fw.mat1jaczyyy.com) with the `Midi Fighter 64 (CFW)` option selected.

To upload the custom firmware to your Midi Fighter 64, use the official [Midi Fighter Utility](https://store.djtechtools.com/pages/midi-fighter-utility)'s Load Custom Firmware feature. Connect your Midi Fighter 64, and then navigate to `Tools` -> `Midifighter` -> `Load Custom Firmware` -> `For a 64` and select the downloaded firmware file.

## Building

### Prerequisites:

You will need to install the **avr-toolchain** and **make** to build the custom firmware yourself.

#### Windows

On windows, you have to open the command prompt and enter:

```bash
winget install avr-gcc
```

To install **make** on your windows system, you can either install [Git Bash](https://winget.run/pkg/Git/Git) or use [Chocolately](https://community.chocolatey.org/packages/make).

#### macOS

On macOS, you will need to install [brew](https://brew.sh/) first. After that, you can install all everything you need by entering this in your terminal:

```shell
brew tap osx-cross/avr
brew install avr-gcc make
```

`make` will be preinstalled by installing brew and the Xcode Command-Line Tools.

#### Linux (Ubuntu)

On Ubuntu Linux you will need to enter this into your terminal to install the avr-toolchain and make

```shell
sudo apt-get update
sudo apt-get install gcc-avr binutils-avr avr-libc
```

If you are using any other distro, I recommend you try to find the packages using your prefered package manager.

### Building the Firmware

Building the Firmware is easy. If you installed the avr-toolchain and make on your system, you can enter the root directory of this repository with your Terminal and enter:

```shell
make
```

This should result in a "midifighter64.hex" file in a new `/build` directory, which you can then flash.