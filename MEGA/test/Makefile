FILENAME = mega
PROGRAMMER = arduino
DEVICE = atmega2560
PORT = /dev/cu.usbmodem14101
BAUD = 115200

COMPILE = avr-gcc -Wall -Os -mmcu=$(DEVICE)

default: compile upload clean

compile:
	$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o || exit 1
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).o
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
	avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf
upload:
	avrdude -c $(PROGRAMMER) -p $(DEVICE) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i

clean:
	rm $(FILENAME).o
	rm $(FILENAME).elf
	rm $(FILENAME).hex
