#
# A simple arcade Space Game
# GPL2 license, see License.txt
#
# Makefile adapted from KOS example:
# Copyright (C) 2024 Jason Rost (OniEnzeru)
#


# Directories
KOS_ROMDISK_DIR := romdisk

CFLAGS += -I../../CeresEngine/src

# File aggregators
SRCS		:= src/main.cpp
OBJS		:= src/main.o romdisk.o

# Starship Madness game sources
include ./StarshipMadnessSourcesMk.txt

# Ceres 3D engine sources
include ./ceresSourcesMk.txt

# Compiler Flags
KOS_CPPSTD	:= -std=c++20
LDLIBS 		:= -lstb_image -lGL -lkmg -lkosutils -lwav

TARGET = starshipmadness.elf

.PHONY: all clean push

all: rm-elf $(TARGET)

clean: rm-elf
	-rm -rf $(OBJS) ./objs/romdisk.*
	find . -name "*.o" -exec rm {} \;


include $(KOS_BASE)/Makefile.rules

$(TARGET): $(OBJS)
	kos-c++ -o $(TARGET) $(OBJS) $(LDLIBS)

dist: $(TARGET)
	-rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)

rm-elf:
	-rm -f $(TARGET)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)
