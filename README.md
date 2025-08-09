# Code Gen: From UDB to Implementations - Coding Challenge
## Problem Statement
- Write a Python program that reads at least one of the YAML files in the RISC-V Unified Database project (https://github.com/riscv-software-src/riscv-unified-db) under spec/std/isa/inst.
- Same Python program then emits the data in the YAML file as a C header file, format of your choosing.
- Write a C program that includes the C header file generated in step 2.
- Same C program should emit the contents of the C header file in YAML. The YAML file does NOT need to match the original YAML file.
- Repeat steps 1-4 using the generated YAML file as input. The emitted YAML file should match the generated YAML file this time.


## Overview

**This project features a Python script that converts YAML data (sourced from files in the RISC-V Unified Database Git repository) into a collection of structs within a C header file. Any RISC-V architecture instruction, provided it follows the YAML format outlined in the RISC-V UDB, can be expressed as a C variable of the given struct type. As an example, the sample "mock" instruction from the mock.yaml file is implemented as a static variable directly in that header file. This illustrates how any instruction from the RISC-V UDB can be captured programmatically in a machine-readable format through the use of this straightforward single-header library.**

The project includes:
- `yaml_parser.py`: A Python script that reads a YAML file and generates a C header file (`outputheader.h`) containing structured data representations. Usage - 
```
./yaml_parser.py <input.yaml>
```
- `cToYaml.c`: A C program that includes the generated header and emits the data back as a YAML file.Usage - 
```
./cToYaml <output.yaml>
```
- `mock.yaml`: A sample YAML input file, taken from RISC-V UDB git repo linked above, for testing.
- `Makefile`: Automates building, generating, and verifying the project.

The project has been tested to ensure round-trip consistency: generating YAML from the header, then regenerating from that YAML, results in identical files (verified via lack of output from `diff` command).

## Prerequisites

- Python 3 (with `pyyaml` library; install via `pip install pyyaml` if needed).
- GCC (or another C compiler).
- Make (for using the Makefile).

## Usage

1. Generate the C header from `mock.yaml`:
   ```
   make generate-header
   ```
   This runs `./yaml_parser.py mock.yaml` and produces `outputheader.h`. **You have to do this first because otherwise header can't be generated and c program will not compile.**

2. Build the C program:
   ```
   make
   ```

3. Generate YAML from the header:
   ```
   make generate-yaml
   ```
   This runs `./cToYaml generated.yaml` and produces `generated.yaml`.

4. Regenerate the header from `generated.yaml`:
   ```
   make regenerate-header
   ```
   This runs `./yaml_parser.py generated.yaml` and updates `outputheader.h`.

5. Regenerate YAML from the new header:
   ```
   make regenerate-yaml
   ```
   This runs `./cToYaml regenerated.yaml` and produces `regenerated.yaml`.

6. Verify consistency (should show no differences):
   ```
   make verify
   ```
   This runs `diff generated.yaml regenerated.yaml`. A lack of output means it is successful (`generated.yaml` and `regenerated.yaml` are the same.)

7. Clean up generated files:
   ```
   make clean
   ```

## Manual Commands (as tested)

```
$ ./yaml_parser.py mock.yaml
$ gcc -o cToYaml cToYaml.c
$ ./cToYaml ./generated.yaml
$ ./yaml_parser.py generated.yaml
$ ./cToYaml ./regenerated.yaml
$ diff generated.yaml regenerated.yaml
$
```

If the `diff` command produces no output, the round-trip test succeeds.