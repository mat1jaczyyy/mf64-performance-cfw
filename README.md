# Midi Fighter 64 performance-optimized Custom Firmware

This repository contains the source code of my custom firmware for the [Midi Fighter 64](https://store.djtechtools.com/products/midi-fighter-64). Critically, this firmware enables Apollo Studio support and greatly enhances Ableton Live-based performances. The modification is easy to install, free to use and works with every currently existing Ableton Live project file.

## Installation

Download the latest custom firmware file with the desired patches from the [Launchpad Utility](https://fw.mat1jaczyyy.com) with the `Midi Fighter 64 (CFW)` option selected.

To upload the custom firmware to your Midi Fighter 64, use the official [Midi Fighter Utility](https://store.djtechtools.com/pages/midi-fighter-utility)'s Load Custom Firmware feature. Connect your Midi Fighter 64, and then navigate to `Tools` -> `Midifighter` -> `Load Custom Firmware` -> `For a 64` and select the downloaded firmware file.

## Building

Prerequisites:
- [Atmel Studio 7.0](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio#Downloads)
- [WinAVR](https://sourceforge.net/projects/winavr/files/latest/download)
    - **WARNING**: WinAVR will likely **COMPLETELY OVERWRITE YOUR SYSTEM PATH ENVIRONMENT VARIABLE**. Please ensure you have a backup of it before installing!
        - If you end up losing your PATH variable, **do not reboot your machine** and follow the steps outlined [here](https://superuser.com/a/1127136).
    - Patch WinAVR with a modified [msys-1.0.dll](https://www.madwizard.org/download/electronics/msys-1.0-vista64.zip) in `utils/bin`.

Build `Midi Fighter 64.atsln` with Atmel Studio, and the output file will be located at `midi_fighter_64/midifighter64.hex`.

To upload the final output file to a Midi Fighter 64, use the Midi Fighter Utility's Load Custom Firmware feature as explained in the Installation section.
