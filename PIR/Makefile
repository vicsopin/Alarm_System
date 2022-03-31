default:
		avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o pir.o pir.c
		avr-gcc -o pir.bin pir.o
		avr-objcopy -O ihex -R .eeprom pir.bin pir.hex
		avrdude -F -V -c arduino -p ATMEGA328P -P /dev/tty.usbmodem14201 -b 115200 -U flash:w:pir.hex