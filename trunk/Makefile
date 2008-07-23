#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	$(shell basename $(CURDIR))
export TOPDIR		:=	$(CURDIR)
export GBFSFILES	:= 	$(notdir $(wildcard data/*.*))
GBFSDIR			:=	data  

#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7 -9 $(TARGET).arm9

$(TARGET).ds.gba	: $(TARGET).ds

#---------------------------------------------------------------------------------
$(TARGET).ds	:	$(TARGET).arm7 $(TARGET).arm9 data.gbfs
	ndstool	-c $(TARGET).ds -7 $(TARGET).arm7 -9 $(TARGET).arm9
	padbin 256 $(TARGET).ds
	cat $(TARGET).ds data.gbfs > $(TARGET).ds.tmp
	mv $(TARGET).ds.tmp $(TARGET).ds

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
data.gbfs :
	@cd $(GBFSDIR) && gbfs $(TOPDIR)/$@ $(GBFSFILES)

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).ds $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9 data.gbfs
