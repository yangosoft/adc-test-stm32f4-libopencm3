BINARY = led

LDSCRIPT = stm32f429i-discovery.ld

CP      = arm-none-eabi-objcopy
OD      = arm-none-eabi-objdump

# OBJS    += print

$(BINARY).bin: $(BINARY).elf
	@ echo "[Copying]"
	$(CP) -Obinary  $(BINARY).elf $(BINARY).bin
	$(OD) -S $(BINARY).elf > $(BINARY).lst
	
	
include ./Makefile.include
