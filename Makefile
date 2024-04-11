default:
		arduino-cli compile --fqbn arduino:avr:nano ./main/main.ino
		sudo arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old ./main/main.ino

build:
		arduino-cli compile --fqbn arduino:avr:nano ./main/main.ino


upload:
		sudo arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old ./main/main.ino

monitor:
		sudo arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
