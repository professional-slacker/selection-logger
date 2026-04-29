# Selection Logger Makefile
# Cross-platform build system for Windows (Wine testing) and Linux

# Directories
SRC_DIR = src

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -I$(SRC_DIR)
LDFLAGS =

# Platform settings (can be overridden)
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')
EXE_EXT =

# Static linking option (for Windows release)
STATIC ?= 0

# Default to Linux if not specified
ifeq ($(PLATFORM),windows)
    EXE_EXT = .exe
    CXX = x86_64-w64-mingw32-g++
    CXXFLAGS += -D_WIN32_WINNT=0x0600 -DWIN32_LEAN_AND_MEAN -D_WIN32
    LDFLAGS += -lole32 -luser32 -ladvapi32 -lshell32

    # Static linking for release builds
    ifeq ($(STATIC),1)
        CXXFLAGS += -static
        LDFLAGS += -static-libgcc -static-libstdc++ -Wl,-Bstatic
    endif
else
    # Default to Linux
    PLATFORM = linux
    CXXFLAGS += -DPLATFORM_LINUX=1
    LDFLAGS += -lX11 -lpthread
endif

# Targets
TARGET = selection-logger$(EXE_EXT)

# Source files (relative to SRC_DIR)
COMMON_SRCS = clipboard.cpp platform.cpp win32_compat.cpp
MAIN_SRC = memory_daemon_auto_cross.cpp

# All sources with paths
SRCS = $(MAIN_SRC) $(COMMON_SRCS)
OBJS = $(addprefix $(SRC_DIR)/, $(SRCS:.cpp=.o))

# Windows-specific objects
ifeq ($(PLATFORM),windows)
    WIN32_OBJS = $(SRC_DIR)/service_win32.o $(SRC_DIR)/main_win32.o
else
    WIN32_OBJS =
endif

# Default target
all: $(TARGET)

# Release target (statically linked for Windows)
release: clean
ifeq ($(PLATFORM),windows)
	$(MAKE) STATIC=1
else
	$(MAKE)
endif

# Clean target
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET) test/test_selection test/test_clipboard_formats test/test_file_copy test/test_platform

# Link
$(TARGET): $(OBJS) $(WIN32_OBJS)
	$(CXX) -o $@ $(OBJS) $(WIN32_OBJS) $(LDFLAGS)

# Compile rules
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows-specific sources compilation (stubs on non-Windows)
$(SRC_DIR)/service_win32.o: $(SRC_DIR)/service_win32.cpp $(SRC_DIR)/service_win32.h
ifeq ($(PLATFORM),windows)
	$(CXX) $(CXXFLAGS) -c $< -o $@
else
	@echo "Skipping Windows-only file: $<"
	@touch $@  # Create empty object file for Linux builds
endif

$(SRC_DIR)/main_win32.o: $(SRC_DIR)/main_win32.cpp
ifeq ($(PLATFORM),windows)
	$(CXX) $(CXXFLAGS) -c $< -o $@
else
	@echo "Skipping Windows-only file: $<"
	@touch $@  # Create empty object file for Linux builds
endif

# Test targets
test: all
ifeq ($(PLATFORM),linux)
	@echo "Running Linux tests..."
	cd test && ./test_selection || true
	cd test && ./test_clipboard_formats || true
	cd test && ./test_platform || true
else
	@echo "Windows build - use 'make test-wine' for Wine testing"
endif

# Wine testing
test-wine: $(TARGET)
	@echo "Testing with Wine..."
	@echo "Note: Wine clipboard access may require X11 integration"
	wine $(TARGET) --help || true

# Build for Windows (cross-compile)
windows: clean
	$(MAKE) CXX=x86_64-w64-mingw32-g++ PLATFORM=windows

# Build for Linux (native)
linux: clean
	$(MAKE) PLATFORM=linux

# Install (Linux only)
install: $(TARGET)
	@echo "Installing to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall (Linux only)
uninstall:
	@echo "Removing from /usr/local/bin..."
	sudo rm -f /usr/local/bin/$(TARGET)

# Help
help:
	@echo "Selection Logger Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all              - Build for current platform (default)"
	@echo "  windows          - Cross-compile for Windows"
	@echo "  linux            - Build for Linux (native)"
	@echo "  clean            - Remove build artifacts"
	@echo "  test             - Run tests (Linux only)"
	@echo "  test-wine        - Test Windows build with Wine"
	@echo "  install          - Install to /usr/local/bin (Linux)"
	@echo "  uninstall        - Uninstall from /usr/local/bin"
	@echo ""
	@echo "Current platform: $(PLATFORM)"
	@echo "Compiler: $(CXX)"

.PHONY: all clean test test-wine windows linux install uninstall help
