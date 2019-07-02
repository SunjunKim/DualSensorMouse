# DualSensorMouse Manual

This is a short manual on how to upload 'PMW3360_dualsensor.ino' to your DualSensorMouse.



## How to upload

### Verifying ✔

1. Open PMW3360_dualsensor.ino with Adruino IDE.
2. Go to "Tools > Manage Libraries", search "PMW3360 Module" and install the library.
3. Go to [here](https://github.com/SunjunKim/PMW3360_Arduino) and download "/library/AdvMouse".
4. Copy the AdvMouse folder into "/<your arduino directiry>/libraries".
5. Try verifying(✔) the code. It should be compiled without any problem.



### Uploading ➡

1. Go to "File > Preferences" and upload a link below to "Additional Boards Manager URLs".

```
https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
```

2. Go to "Tools > Board > Board Manager" and search "SparkFun AVR boards" and install the boards with the latest version as possible.

3. Select the board of this ino project as **SparkFun Pro Micro.**

4. Select the processor of this ino project as **ATmega32U4 (5V, 16 MHz).** It is important to choose option with 5V, 16MHz otherwise uploading will make your Pro Micro "***bricked***".

5. Check if you can select the serial port. 

   Having the port is grayed out while the mouse plugged in indicates that the driver is not installed.

   Install the SparkFun Pro Micro driver (5V, 16Mhz) compatible to your OS, unplug the mouse and plug again.

6. If you can select the port, try uploading(➡) the code. It should be uploaded without any problem.



## Troubleshoting

If a popup message like "*USB Device Not Recognized*" shows up when plugging the mouse even though you successfully installed the driver, you have a high chance of your SparkFun Pro Micro been "***bricked***". Luckily, you will be able to revive the board by following [this article](https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide#ts-revive).



### Retrieving a "Bricked" Pro Micro ☠

1. Unscrew the back of the DualSensorMouse with a flat-blade screwdriver.
2. Disconnect the micro pin from the Pro Micro. Pull out the Pro Micro and reconnect.
3. Plug the DualSensorMouse to your computer and open Device Manager.
4. By quickly making a "short" twice from RST pin to GND pin, you can activate a "bootloader port" for less than 8 seconds. You will hear sound in both moments when the "bootloader port" is activated and deactivated. The sound is same as when a USB is plugged in and then plugged out.
5. Between those two sounds, your 'PMW3360_dualsensor.ino' should **be finished being uploaded **to your Pro Micro. Try to focus and match the exact time.
6. When you successfully uploaded the ino file between the two sounds, the mouse should be working without any problems.
