# Directories
SRCDIR = src/
INCDIR = inc/
BUILDDIR = build/
TESTDIR = test/
TIVAWAREDIR = tivaware/
UNITYDIR = Unity/src/
TESTBUILDDIR = testbuild/
TESTRESULTDIR = testbuild/results/
TESTOBJDIR = testbuild/objs/

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PM

# ARM GCC toolchain settings
ARM_PREFIX = arm-none-eabi
ARM_CC = $(ARM_PREFIX)-gcc
ARM_LD = $(ARM_PREFIX)-ld
ARM_AR = $(ARM_PREFIX)-ar
ARM_OBJCOPY = $(ARM_PREFIX)-objcopy
ARM_CFLAGS = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -MD -std=c99 -Wall -pedantic -DPART_${PART} -I$(INCDIR) -I$(TIVAWAREDIR) -Os
ARM_LDFLAGS = -T led_pwm.ld --entry ResetISR --gc-sections



# Definitions for the static analysis
CPPCHECK = cppcheck --enable=all --inconclusive --error-exitcode=1
CLANG_TIDY = clang-tidy

# Get the location of libgcc.a, libc.a and libm.a from the GCC front-end.
LIBGCC:=${shell ${ARM_CC} ${ARM_CFLAGS} -print-libgcc-file-name}
LIBC:=${shell ${ARM_CC} ${ARM_CFLAGS} -print-file-name=libc.a}
LIBM:=${shell ${ARM_CC} ${ARM_CFLAGS} -print-file-name=libm.a}

# Source files
SRC = $(wildcard $(SRCDIR)*.c) 
OBJ = $(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o,$(SRC)) ${TIVAWAREDIR}driverlib/gcc/libdriver.a
SRCFILESFORTEST = src/serial_handler.c
ANALYSIS_SRC = src/led_pwm.c src/serial_handler.c


# Test source files
TESTSRC = $(wildcard $(TESTDIR)*.c)
TESTOBJ = $(patsubst $(TESTDIR)%.c,$(TESTOBJDIR)%.o,$(TESTSRC))
TESTSRCOBJ = $(patsubst $(SRCDIR)%.c,$(TESTOBJDIR)%.o,$(SRCFILESFORTEST))
UNITYOBJ = $(patsubst $(UNITYDIR)%.c,$(TESTOBJDIR)%.o,$(wildcard $(UNITYDIR)*.c))


# GCC settings for unit testing
GCC = gcc
GCC_CFLAGS = -I$(INCDIR) -I$(UNITYDIR) -I$(SRCDIR) -I$(TIVAWAREDIR) -DTEST
TESTOBJS = $(patsubst $(PATHTEST)Test%.c,$(PATHTESTRESULT)Test%.txt,$(SRCTEST))


# Targets
TARGET = $(BUILDDIR)firmware.elf
TESTTARGETS = $(patsubst $(TESTDIR)%.c,$(TESTRESULTDIR)%.txt,$(TESTSRC))

# Default target
all: $(TARGET)

# Build firmware
$(TARGET): $(OBJ)
	$(ARM_LD) $(ARM_LDFLAGS) -o $@ $^ '${LIBM}' '${LIBC}' '${LIBGCC}'
	$(ARM_OBJCOPY) -O binary $@ $(TARGET:.elf=.bin)

$(BUILDDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(BUILDDIR)
	$(ARM_CC) $(ARM_CFLAGS) -c $< -o $@

# Unit testing
test: $(TESTTARGETS)
	@echo "-----------------------\nIGNORES:\n-----------------------"
	@grep -s IGNORE $(TESTRESULTDIR)*.txt || true
	@echo "-----------------------\nFAILURES:\n-----------------------"
	@grep -s FAIL $(TESTRESULTDIR)*.txt || true
	@echo "-----------------------\nPASSED:\n-----------------------"
	@grep -s PASS $(TESTRESULTDIR)*.txt || true
	@echo "\nDONE"

$(TESTRESULTDIR)%.txt: $(TESTBUILDDIR)%.out
	@mkdir -p $(TESTRESULTDIR)
	-./$< > $@ 2>&1

$(TESTBUILDDIR)%.out: $(TESTOBJDIR)%.o $(UNITYOBJ) $(TESTSRCOBJ)
	$(GCC) -o $@ $^

$(TESTOBJDIR)%.o: $(TESTDIR)%.c
	@mkdir -p $(TESTOBJDIR)
	$(GCC) $(GCC_CFLAGS) -c $< -o $@

$(TESTOBJDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(TESTOBJDIR)
	$(GCC) $(GCC_CFLAGS) -c $< -o $@

$(TESTOBJDIR)%.o: $(UNITYDIR)%.c
	@mkdir -p $(TESTOBJDIR)
	$(GCC) $(GCC_CFLAGS) -c $< -o $@

# Maintain the test results after 'make test'
.PRECIOUS: $(TESTRESULTDIR)%.txt

# Static Analysis
static-analysis:
	@echo "Running cppcheck..."
	$(CPPCHECK) $(ANALYSIS_SRC) --suppress=missingIncludeSystem --suppress=checkersReport --inline-suppr -I$(INCDIR) -I$(TIVAWAREDIR) -I$(UNITYDIR) -I$(SRCDIR) -DPART_${PART} 
	@echo "Running clang-tidy..."
	$(CLANG_TIDY) $(ANALYSIS_SRC) -- -I$(INCDIR) -I$(TIVAWAREDIR) -I$(UNITYDIR) -I$(SRCDIR) -DPART_${PART} -std=c99

# Clean up build and test files
clean:
	@rm -rf $(BUILDDIR) $(TESTBUILDDIR) $(TESTRESULTDIR)

.PHONY: all test clean debug