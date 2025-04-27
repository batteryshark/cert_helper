# Makefile for cert_helper

# Compiler settings
CC_WIN32 = i686-w64-mingw32-gcc
CC_WIN64 = x86_64-w64-mingw32-gcc
CFLAGS = -w -s
LIBS = -lcrypt32

# Output directories
BIN_DIR_WIN32 = bin/win32
BIN_DIR_WIN64 = bin/win64

# Source files
SRC = src/cert_helper.c

# Target files
DLL_WIN32 = $(BIN_DIR_WIN32)/cert_helper.dll
EXE_WIN32 = $(BIN_DIR_WIN32)/cert_helper.exe
DLL_WIN64 = $(BIN_DIR_WIN64)/cert_helper.dll
EXE_WIN64 = $(BIN_DIR_WIN64)/cert_helper.exe

# Default target - builds both 32-bit and 64-bit versions
all: win32 win64

# Build all 32-bit targets
win32: $(DLL_WIN32) $(EXE_WIN32)

# Build all 64-bit targets
win64: $(DLL_WIN64) $(EXE_WIN64)

# Create output directories
$(BIN_DIR_WIN32):
	mkdir -p $(BIN_DIR_WIN32)

$(BIN_DIR_WIN64):
	mkdir -p $(BIN_DIR_WIN64)

# Build 32-bit DLL
$(DLL_WIN32): $(SRC) | $(BIN_DIR_WIN32)
	$(CC_WIN32) $(CFLAGS) $(SRC) $(LIBS) -shared -o $@

# Build 32-bit executable
$(EXE_WIN32): $(SRC) | $(BIN_DIR_WIN32)
	$(CC_WIN32) $(CFLAGS) $(SRC) $(LIBS) -DTEST_BIN -o $@

# Build 64-bit DLL
$(DLL_WIN64): $(SRC) | $(BIN_DIR_WIN64)
	$(CC_WIN64) $(CFLAGS) $(SRC) $(LIBS) -shared -o $@

# Build 64-bit executable
$(EXE_WIN64): $(SRC) | $(BIN_DIR_WIN64)
	$(CC_WIN64) $(CFLAGS) $(SRC) $(LIBS) -DTEST_BIN -o $@

# Clean all build output
clean:
	rm -rf $(BIN_DIR_WIN32) $(BIN_DIR_WIN64)

# Rebuild everything
rebuild: clean all

.PHONY: all win32 win64 clean rebuild 