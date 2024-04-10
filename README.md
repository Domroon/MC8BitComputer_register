# Microcontroller 8Bit Computer

## Install Arduino on ArchLinux

    yay arduino-cli
    arduino-cli core update-index

## Make a condig file if not exists

    sudo nano /home/<your-username>/.arduino15/arduino-cli.yaml

## Config File
    library:
        enable_unsafe_install: true

## Install Arduino Libraries

    set enable_unsafe_install to true
    arduino-cli lib install --git-url https://github.com/PaulStoffregen/Encoder.git

## Compile and Upload

    arduino-cli compile --fqbn arduino:avr:nano ./main/main.ino
    sudo arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old ./main/main.ino

## Serial Monitor

    sudo arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600

## Documentation
https://docs.arduino.cc/hardware/nano/
