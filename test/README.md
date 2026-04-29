# Test Suite

This directory contains test scripts and test source files for selection-logger.

## Test Scripts

```
test_wine.sh       # Test Windows build under Wine
test_wine_auto.sh  # Extended Wine test for auto-monitoring mode
```

## Test Source Files

| File | Description |
|---|---|
| `test_selection.cpp` | Tests X11 selection text retrieval |
| `test_clipboard_formats.cpp` | Tests clipboard format enumeration |
| `test_file_copy.cpp` | Tests file-based copy operations |
| `test_platform.cpp` | Tests platform detection logic |

## Building and Running Tests

```bash
make test
```

For Windows binaries under Wine:
```bash
cd test
./test_wine.sh
./test_wine_auto.sh
```

