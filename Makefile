.PHONY: docs clean clean_objs cicd
.PHONY: all
.PHONY: build

#=========== PROJECT NAME ===========#
PROJECT := phsm

# Build makefile
BUILD_MAKEFILE := Makefile.build

# Build type: test, firmware, validation, library
BUILD_TARGET ?= test

# Architecture. Should be written capitalized letters
ARCH ?= NATIVE

COMPILE_MODE ?= DEBUG

BUILD_ARG_LIST = \
	PROJECT=$(PROJECT) \
	ARCH=$(ARCH) \
	COMPILE_MODE=$(COMPILE_MODE) \
	WARNINGS=y 
	

all: build

build:
	@${MAKE} -f $(BUILD_MAKEFILE) $(BUILD_TARGET) $(BUILD_ARG_LIST)

clean_objs:
	@rm -rf build/

clean: clean_objs
	@rm -f $(PROJECT)*
	@rm -f *.out

docs:
	@doxygen docs/doxygen.cfg