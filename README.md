# Zigbee 3.0 Firmware for original LYWSD03MMC Sensor

This repository contains the Zigbee firmware for Xiaomi LYWSD03MMC Bluetooth temperature and humidity sensor.

## Overview

![](./assets/device.jpg)

The LYWSD03MMC is a Bluetooth temperature and humidity sensor that can be integrated into a Zigbee network using
this firmware. This repository hosts the code and related resources to flash the device and make 
it compatible with Zigbee networks.

## Features
- Full-featured firmware to convert Xiaomi LYWSD03MC device with default ZCL battery, temperature and relative humidity clusters
- Display support for known revisions
- OTA support in firmware and binaries in ZCL format for update 
- Flashable over-the-air from custom ATC firmware https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html
- Flashable over SWS-UART interface using one of:

  - https://pvvx.github.io/ATC_MiThermometer/USBCOMFlashTx.html
  - https://github.com/pvvx/ATC_MiThermometer/blob/master/TLSR825xComFlasher.py 
  - https://github.com/pvvx/TLSRPGM 

## Getting Started

### Prerequisites

- Zigbee compatible hardware (e.g., Zigbee coordinator or gateway).
- Necessary tools for flashing firmware to the sensor.

### Building firmware

1. Clone TC32 toolchain according to your host OS:
    ```sh
    git clone https://github.com/devbis/tc32.git -b linux
    ```
    ```sh
    git clone https://github.com/devbis/tc32.git -b macos
    ```
    ```sh
    git clone https://github.com/devbis/tc32.git -b windows
    ```

2. Clone this repository and SDK:

    ```sh
    git clone https://github.com/devbis/z03mmc.git
    git clone https://github.com/devbis/tl_zigbee_sdk.git -b 3.6.8.5 --depth 1
   
    cd z03mmc
    ```
   
3. Configure and build:

   Building requires at Python 3.9 or later.

    ```sh
    cmake -DSDK_PREFIX=$(pwd)/../tl_zigbee_sdk -DTOOLCHAIN_PREFIX=$(pwd)/../tc32 -B build .
    cmake --build build --target z03mmc.zigbee
    ```

    Firmware binary is located at `build/src/z03mmc.bin`
    The binary with OTA header is at the same folder, ending with `z03mmc.zigbee`


## Flashing over the air (easy way)
1. Open an awesome tool from ATC_MiThermometer https://pvvx.github.io/ATC_MiThermometer/TelinkMiFlasher.html
2. Click "Connect" button and find device LYWSD03MMC, wait for connection (Connected in logs)
3. On a new device with stock firmware click "Do Activation" and wait some time.
4. Next "Select Firmware", choose file with the transitional firmware [ATC_ota_400000](./assets/ATC_ota_40000.bin), click "Start Flashing". This step is required even if you already installed a custom bluetooth firmware. Not flashing this file will likely cause your device to get bricked and require flashing via USB/UART!
5. You will see in logs "Update done after NN seconds"
6. Connect to the device again (with name ATC_802190 or similar, based on mac-address). If it doesn't appear, remove and reinsert the battery and refresh the webpage with the flashing tool.
7. Flash the latest [z03mmc.bin](https://github.com/devbis/z03mmc/releases) firmware over transitional firmware to convert it to zigbee. Use https://devbis.github.io/telink-zigbee/ page if previous flasher stops because of the firmware size.
8. The device should now show up in your Zigbee bridge (If joining is enabled, of course). If it doesn't, reinsert the battery and/or short the RESET and GND contacts on the board for 3 seconds.

## Flashing firmware with USB to UART

### Prerequisites: 
1. TTL-USB adaptor
2. 1k-1.8k Ohm resistor
3. python3 with pyserial module installed


To flash a new firmware via an standard USB to UART adapter, simply connect the Thermometer as seen in the picture [Mi_SWS_Connection.jpg](./assets/Mi_SWS_Connection.jpg) to the USB to UART converter and run the TLSR825xComFlasher.py tool.

Example: `python3 TLSR825xComFlasher.py -p COM3 wf 0 z03mmc.bin`

Example: `python3 TLSR825xComFlasher.py -p /dev/ttyUSB0 wf 0 z03mmc.bin`

In case if the SWS pin is used by the firmware, try this sequence:
1. Power off the sensor
2. `python3 TLSR825xComFlasher.py -p <YOUR_COM_PORT> -t5000 wf 0 z03mmc.bin`
3. Now you have 5 seconds to power on the sensor
4. In case the chip has not started being flashed, run `python3 TLSR825xComFlasher.py -p <YOUR_COM_PORT> wf 0 z03mmc.bin` without the timeout again.
   
   If the flashing fails reduce baud rate down to 340000 or increase timeouts in the script.

5. If you flashed the module but the screen is remaining blank, try `python3 TLSR825xComFlasher.py -p <YOUR_COM_PORT> ea` to erase all flash and then write the firmware again.

The UART flasher software uses the tool from https://github.com/pvvx/ATC_MiThermometer. Thanks to pvvx for the awesome work on this!

## Return to Bluetooth firmware

The only supported method to reverting firmware is flashing via USB-UART dongle. See previous chapter.

## Zigbee OTA upgrades

The already flashed firmware supports OTA zigbee upgrade via standard flow.
See zigbee2mqtt, ZHA, and HOMEd documentation for details.


## Related Work
z03mmc is based on the original work of @pvvx, and @atc1441, who developed the initial firmware versions for bluetooth-capable device.
- https://github.com/pvvx/ATC_MiThermometer
- https://github.com/atc1441/ATC_MiThermometer

## Usage

1. Flash the firmware
2. Enable pairing mode on Zigbee coordinator
3. In case it is not joining, close the RESET and GND contacts on the board for 3 seconds to reset Zigbee settings. Replug the battery may require
4. For zigbee2mqtt you need to add custom converter if you use version 1.33.1 or earlier

## License

This project is licensed under the GNU General Public License 3.0 or later - see the [LICENSE.txt](LICENSE.txt) file for details.
