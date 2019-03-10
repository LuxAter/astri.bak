
SHELL=/bin/bash
ifndef .VERBOSE
    .SILENT:
endif
ROOT=$(shell pwd)
CXX=clang++
CC=clang
CCIGNORE=
CXXIGNORE=
CCFLAGS=-fPIC -Wall -Wpedantic --static
CXXFLAGS=-std=c++17 -fPIC -Wall -Wpedantic --static
LINK=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lcurl
SOURCE=src
INCLUDE_DIR=include
INCLUDE=
BUILD=build
COMMON_INCLUDE=-I$(ROOT)/$(INCLUDE_DIR) $(INCLUDE)

SCAN_COLOR=\033[1;35m
BUILD_COLOR=\033[32m
CLEAN_COLOR=\033[1;33m
LINK_COLOR=\033[1;32m
INSTALL_COLOR=\033[1;36m
CMD_COLOR=\033[1;34m
HELP_COLOR=\033[1;34m

define scan_target
printf "%b%s%b\n" "$(SCAN_COLOR)" "Scaning dependencies for target $(1)" "\033[0m"
endef
define complete_target
printf "%s\n" "Built target $(1)"
endef
define clean_target
printf "%b%s%b\n" "$(CLEAN_COLOR)" "Cleaning target $(1)" "\033[0m"
endef
define install_target
printf "%b%s%b\n" "$(INSTALL_COLOR)" "Installing target $(1)" "\033[0m"
endef
define uninstall_target
printf "%b%s%b\n" "$(INSTALL_COLOR)" "Unnstalling target $(1)" "\033[0m"
endef
define print_build_c
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(BUILD_COLOR)" "Building C object $$str" "\033[0m"
endef
define print_build_cpp
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(BUILD_COLOR)" "Building C++ object $$str" "\033[0m"
endef
define print_link_lib
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(LINK_COLOR)" "Linking static library $$str" "\033[0m"
endef
define print_link_exe
str=$$(realpath --relative-to="$(ROOT)" "$(1)");    printf "%b%s%b\n" "$(LINK_COLOR)" "Linking executable $$str" "\033[0m"
endef
define print_run_cmd
printf "%b%s%b\n" "$(CMD_COLOR)" "Running '$(1)'" "\033[0m"
endef
define help
printf "%b%*s%b: %s\n" "$(HELP_COLOR)" 20 "$(1)" "\033[0m" "$(2)"
endef

all: build-astri.exe
clean: clean-astri.exe
install: install-astri.exe
uninstall: uninstall-astri.exe

# ASTRI.EXE {{{

ASTRI.EXE=astri.exe
ASTRI.EXE_FILES=$(shell find "$(SOURCE)/" -name "*.c") $(shell find "$(SOURCE)/" -name "*.cpp")
ASTRI.EXE_OBJS=$(ASTRI.EXE_FILES:%=$(ROOT)/$(BUILD)/%.o)
-include $(ASTRI.EXE_OBJS:.o=.d)

build-astri.exe:  pre-astri.exe $(ASTRI.EXE)
	$(call complete_target,$(shell basename $(ASTRI.EXE)))

clean-astri.exe:
	$(call clean_target,$(shell basename $(ASTRI.EXE)))
	if [ -e "$(ASTRI.EXE)" ]; then rm $(ASTRI.EXE); fi

pre-astri.exe:
	$(call scan_target,$(shell basename $(ASTRI.EXE)))

$(ASTRI.EXE): $(ASTRI.EXE_OBJS) FORCE
	$(call print_link_exe,$(shell basename $(ASTRI.EXE)))
	$(CXX) $(ASTRI.EXE_OBJS)  $(LINK) $(COMMON_INCLUDE) -o $(ASTRI.EXE)

install-astri.exe: build-astri.exe
	$(call install_target,$(shell basename $(ASTRI.EXE)))
	mkdir -p $(INSTALL_PATH)/bin/
	cp $(ASTRI.EXE) $(INSTALL_PATH)/bin

uninstall-astri.exe:
	$(call uninstall_target,$(shell basename $(ASTRI.EXE)))
	if [ -e "$(INSTALL_PATH)/bin/$(shell basename $(ASTRI.EXE))" ]; then rm $(INSTALL_PATH)/bin/$(shell basename $(ASTRI.EXE)); fi

# }}}

$(ROOT)/$(BUILD)/%.cpp.o: %.cpp
	mkdir -p $(@D)
	$(call print_build_cpp,$@)
	$(CXX) $(CXXFLAGS) -MMD -c $(COMMON_INCLUDE) $< -o $@

$(ROOT)/$(BUILD)/%.c.o: %.c
	mkdir -p $(@D)
	$(call print_build_c,$@)
	$(CC) $(CCFLAGS) -MMD -c $(COMMON_INCLUDE) $< -o $@

FORCE:
