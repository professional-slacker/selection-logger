# Selection Logger Makefile
# Cross-platform build system for Windows (Wine testing) and Linux

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
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
TARGETS = selection-logger$(EXE_EXT) selection-logger-auto$(EXE_EXT)

# Source files
COMMON_SRCS = clipboard.cpp platform.cpp win32_compat.cpp

# Platform-specific main sources
ifeq ($(PLATFORM),windows)
    MAIN_SRCS = memory_daemon_cross.cpp $(COMMON_SRCS)
    AUTO_SRCS = memory_daemon_auto_cross.cpp $(COMMON_SRCS)
else
    MAIN_SRCS = memory_daemon.cpp $(COMMON_SRCS)
    AUTO_SRCS = memory_daemon_auto.cpp $(COMMON_SRCS)
endif

# Object files
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)
AUTO_OBJS = $(AUTO_SRCS:.cpp=.o)

# Windows-specific sources
WIN32_SRCS = service_win32.cpp main_win32.cpp
WIN32_OBJS = $(WIN32_SRCS:.cpp=.o)

# Default target
all: $(TARGETS)

# Release target (statically linked for Windows)
release: clean
ifeq ($(PLATFORM),windows)
	$(MAKE) STATIC=1
else
	$(MAKE)
endif

# Clean target
clean:
	rm -f *.o $(TARGETS) test_*

# Windows-specific objects (always needed for Windows builds)
ifeq ($(PLATFORM),windows)
    WIN32_OBJS = service_win32.o main_win32.o
else
    WIN32_OBJS =
endif

# Main executable (DEPRECATED: manual mode)
selection-logger$(EXE_EXT): $(MAIN_OBJS) $(WIN32_OBJS)
	$(CXX) -o $@ $(MAIN_OBJS) $(WIN32_OBJS) $(LDFLAGS)
	@echo "WARNING: selection-logger (manual mode) is DEPRECATED"
	@echo "Use selection-logger-auto for automatic monitoring"

# Auto-monitoring executable (RECOMMENDED)
selection-logger-auto$(EXE_EXT): $(AUTO_OBJS) $(WIN32_OBJS)
	$(CXX) -o $@ $(AUTO_OBJS) $(WIN32_OBJS) $(LDFLAGS)

# Compile rules
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows-specific sources compilation
service_win32.o: service_win32.cpp service_win32.h
	$(CXX) $(CXXFLAGS) -c service_win32.cpp -o service_win32.o

main_win32.o: main_win32.cpp
	$(CXX) $(CXXFLAGS) -c main_win32.cpp -o main_win32.otion
service_win32.o: service_win32.cpp
ifeq ($(PLATFORM),windows)
	$(CXX) $(CXXFLAGS) -c $< -o $@
else
	@echo "Skipping Windows-only file: $<"
	@touch $@  # Create empty object file for Linux builds
endif

main_win32.o: main_win32.cpp
ifeq ($(PLATFORM),windows)
	$(CXX) $(CXXFLAGS) -c $< -o $@
else
	@echo "Skipping Windows-only file: $<"
	@touch $@  # Create empty object file for Linux builds
endif

# Clean
clean:
	rm -f *.o $(TARGETS) selection-logger.exe selection-logger-auto.exe

# Test targets
test: all
ifeq ($(PLATFORM),linux)
	@echo "Running Linux tests..."
	./test_selection || true
	./test_clipboard_formats || true
	./test_platform || true
else
	@echo "Windows build - use 'make test-wine' for Wine testing"
endif

# Wine testing
test-wine: selection-logger.exe
	@echo "Testing with Wine..."
	@echo "Note: Wine clipboard access may require X11 integration"
	wine selection-logger.exe --help || true

# Build for Windows (cross-compile)
windows: clean
	$(MAKE) CXX=x86_64-w64-mingw32-g++ PLATFORM=windows

# Build for Linux (native)
linux: clean
	$(MAKE) PLATFORM=linux

# Install (Linux only)
install: selection-logger
	@echo "Installing to /usr/local/bin..."
	sudo cp selection-logger /usr/local/bin/
	sudo cp selection-logger-auto /usr/local/bin/

# Uninstall (Linux only)
uninstall:
	@echo "Removing from /usr/local/bin..."
	sudo rm -f /usr/local/bin/selection-logger
	sudo rm -f /usr/local/bin/selection-logger-auto

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