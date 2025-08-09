# Makefile for YAML to C Header and back to YAML project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Executables
CTOYMAL = cToYaml

# Source files
SRC = cToYaml.c

# Default target: build the C program
all: $(CTOYMAL)

$(CTOYMAL): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Generate header from mock.yaml
generate-header: yaml_parser.py mock.yaml
	./yaml_parser.py mock.yaml

# Generate YAML from the header (requires generate-header and build)
generate-yaml: generate-header $(CTOYMAL)
	./$(CTOYMAL) generated.yaml

# Regenerate header from generated.yaml
regenerate-header: yaml_parser.py generated.yaml
	./yaml_parser.py generated.yaml

# Regenerate YAML from the regenerated header
regenerate-yaml: regenerate-header $(CTOYMAL)
	./$(CTOYMAL) regenerated.yaml

# Verify by diff (should have no output if identical)
verify: regenerate-yaml
	diff generated.yaml regenerated.yaml

# Clean up generated files
clean:
	rm -f $(CTOYMAL) outputheader.h generated.yaml regenerated.yaml

.PHONY: all generate-header generate-yaml regenerate-header regenerate-yaml verify clean