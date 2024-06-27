# Nexus Duplicates Detector

This application is written in C++ and is used to detect duplicate files in a directory based on the Levenshtein distance algorithm.

## Building
### Requirements

- CMake version 3.29 or higher
- C++26

You need to build the project through Visual Studio build tools.

## Usage

The application can be run with either one or two arguments:

```bash
./nexus_duplicates_detector [DIRECTORY_PATH]
```

or

```bash
./nexus_duplicates_detector [DIRECTORY_PATH] [MAXIMUM_DIFFERENT_CHARACTERS_NUMBER]
```

- `DIRECTORY_PATH` is the path to the directory you want to check for duplicate files.
- `MAXIMUM_DIFFERENT_CHARACTERS_NUMBER` (optional) is the maximum number of different characters allowed between file names for them to be considered duplicates.
