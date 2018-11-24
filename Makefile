NAME = MCC_PRJ

CC = avr-gcc
# Lambrar de remover -g depois de realizar os testes
CFLAGS = -g -Wall -Os -Wl,--gc-sections -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -iquoteinclude
LDLIBS = 
LDFLAGS = -mmcu=atmega328p

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=%.o)
ELF := $(NAME).elf
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

%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS) -MMD -MP

.PHONY: clean
clean:
	@rm -rf $(OBJ) $(ELF) $(HEX) $(OBJ:%.o=%.d)

-include $(OBJ:%.o=%.d)