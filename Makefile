NAME = MCC_PRJ

CC = avr-gcc
# Lembrar de remover -g depois de realizar os testes
CFLAGS = -Wall -Os -Wl,--gc-sections -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -iquoteinclude
LDLIBS = 
LDFLAGS = -mmcu=atmega328p

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=obj/%.o)
ELF := bin/$(NAME).elf
HEX := $(ELF:.elf=.hex)

h: hex
e: elf
o: obj
c: clean
hex: $(HEX)
elf: $(ELF)
obj: $(OBJ)

$(HEX): $(ELF)
	avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $(ELF) $(HEX)

$(ELF): $(OBJ)
	$(CC) -o $(ELF) $(OBJ) $(LDFLAGS) $(CFLAGS) -MMD -MP

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS) -MMD -MP

arduino: $(HEX)
	avrdude -pm328p -b57600 -carduino -P/dev/ttyS4 -Uflash:w:$<:a
	#avrdude -pm328p -carduino -P/dev/ttyS3 -Uflash:w:$<:a

.PHONY: clean
clean:
	@rm -rf $(OBJ) $(ELF) $(HEX) $(OBJ:%.o=%.d)

-include $(OBJ:%.o=%.d)