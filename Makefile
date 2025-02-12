#
# A simple arcade Space Game
# GPL2 license, see License.txt
#
# Makefile adapted from KOS example:
# Copyright (C) 2024 Jason Rost (OniEnzeru)
#


# Directories

BUILD_DIR=build

include $(N64_INST)/include/n64.mk
include $(N64_INST)/include/t3d.mk

N64_CXXFLAGS += -Isrc/CeresEngine/src -O3 -ffunction-sections -fdata-sections

# File aggregators
SRCS		:= src/main.cpp

# Starship Madness game sources
include ./StarshipMadnessSourcesMk.txt

# Ceres 3D engine sources
include ./ceresSourcesMk.txt

# Compiler Flags
#KOS_CPPSTD	:= -std=c++20
#LDLIBS 		:= -lstb_image -lGL -lkmg -lkosutils -lwav

assets_png = $(wildcard assets/textures/*.png)
assets_wav = $(wildcard assets/audios/*.wav)
assets_stl = $(wildcard assets/stls/*.stl)
assets_xyzuv = $(wildcard assets/xyzuv/*.xyzuv)
assets_fonts = $(wildcard assets/fonts/*.ttf)

assets_conv = $(addprefix filesystem/textures/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/audios/,$(notdir $(assets_wav:%.wav=%.wav64))) \
			  $(addprefix filesystem/stls/,$(notdir $(assets_stl:%.stl=%.stl))) \
			  $(addprefix filesystem/xyzuv/,$(notdir $(assets_xyzuv:%.xyzuv=%.xyzuv))) \
			  $(addprefix filesystem/fonts/,$(notdir $(assets_fonts:%.ttf=%.font64))) 

all: StarshipMadness64.z64

filesystem/textures/%.sprite: assets/textures/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -d -o filesystem/textures "$<"

filesystem/stls/%.stl: assets/stls/%.stl
	@mkdir -p $(dir $@)
	@echo "    [STL] $@"
	cp "$<" $@

filesystem/xyzuv/%.xyzuv: assets/xyzuv/%.xyzuv
	@mkdir -p $(dir $@)
	@echo "    [XYZUV] $@"
	cp "$<" $@

filesystem/audios/%.wav64: assets/audios/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	@$(N64_AUDIOCONV) --wav-compress 1 -o filesystem/audios $<

filesystem/fonts/%.font64: assets/fonts/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem/fonts "$<"

filesystem/fonts/LazenbyCompLiquid_Large.font64: MKFONT_FLAGS+=--size 64 --outline 2
filesystem/fonts/LazenbyCompLiquid_Small.font64: MKFONT_FLAGS+=--size 24 --outline 2


$(BUILD_DIR)/StarshipMadness64.dfs: $(assets_conv)
$(BUILD_DIR)/StarshipMadness64.elf: $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

StarshipMadness64.z64: N64_ROM_TITLE="Starship Madness 64"
StarshipMadness64.z64: $(BUILD_DIR)/StarshipMadness64.dfs

clean:
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean