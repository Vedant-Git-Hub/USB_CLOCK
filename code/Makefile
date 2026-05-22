CC=avr-gcc
FREQ=-DF_CPU=16000000UL
MCU=-mmcu=atmega328p
OBJECTS=build/menu_manager.o build/app.o build/timer.o build/buttons.o build/spi.o build/i2c.o build/main.o build/max7219.o build/ds3231.o
ELFS=build/digital_clock.elf
HEX=build/digital_clock.hex
INCLUDES=-Icore/inc -Iapps/inc

PROGRAMER=usbasp
UC=ATMEGA328P
DEVICE=/dev/ttyACM0
BAUD=115200


default:
	echo "Use make dc to build and download the hex file to Atmega328p"


menu_manager.o: apps/menu_manager.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/menu_manager.o apps/menu_manager.c

app.o: apps/app.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/app.o apps/app.c

timer.o: core/src/timer.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/timer.o core/src/timer.c

buttons.o: core/src/buttons.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/buttons.o core/src/buttons.c

spi.o: core/src/spi.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/spi.o core/src/spi.c

i2c.o: core/src/i2c.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/i2c.o core/src/i2c.c

max7219.o: core/src/max7219.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/max7219.o core/src/max7219.c

ds3231.o: core/src/ds3231.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/ds3231.o core/src/ds3231.c

main.o: main.c
	$(CC) -Os $(FREQ) $(MCU) $(INCLUDES) -c -o build/main.o main.c

dc: menu_manager.o app.o timer.o buttons.o spi.o i2c.o max7219.o ds3231.o main.o 
	$(CC) -Os $(FREQ) $(MCU) $(OBJECTS) -o $(ELFS)
	avr-size -C -x $(ELFS)
	avr-objcopy -O ihex -R .eeprom $(ELFS) $(HEX)
	sudo avrdude -F -V -c $(PROGRAMER) -p $(UC) -P $(DEVICE) -b $(BAUD) -U flash:w:$(HEX)

clean:
	rm $(OBJECTS) $(ELFS) $(HEX)

fuse:
	avrdude -F -p ATMEGA328 -c usbasp -U lfuse:w:0xFF:m
