#!/usr/bin/env python3
import sys
import os
import yaml

typeSuffix = "_t"

def generateTypedef(doc, lstOfTypeDefs, structName, generated):
    if structName in generated:
        return lstOfTypeDefs
    generated.add(structName)

    lstStatements = f"typedef struct {{\n"
    for key, value in doc.items():
        field_name = "".join(ch for ch in key if ch.isalnum() or ch == "_")
        if isinstance(value, str):
            lstStatements += f"\tconst char *{field_name};\n"
        elif isinstance(value, dict):
            lstStatements += f"\t{field_name}{typeSuffix} {field_name};\n"
            lstOfTypeDefs = generateTypedef(value, lstOfTypeDefs, field_name, generated)
        elif isinstance(value, list):
            if value and isinstance(value[0], dict):
                lstStatements += f"\t{field_name}{typeSuffix} *{field_name};\n"
                lstOfTypeDefs = generateTypedef(value[0], lstOfTypeDefs, field_name, generated)
            else:
                lstStatements += f"\tconst char **{field_name};\n"            
            lstStatements += f"\tint {field_name}_count;\n"
        elif isinstance(value, bool):
            lstStatements += f"\tbool {field_name};\n"
        elif isinstance(value, (int, float)):
            lstStatements += f"\tint {field_name};\n"
        else:
            lstStatements += f"\tconst char *{field_name};\n"

    lstOfTypeDefs += lstStatements + f"}} {structName}{typeSuffix};\n\n"
    return lstOfTypeDefs

def generateStruct(doc, indentlevel):
    indent = "\t" * indentlevel
    st = ""
    for key, value in doc.items():
        field_name = "".join(ch for ch in key if ch.isalnum() or ch == "_")
        if isinstance(value, bool):
            st += f",\n{indent}.{field_name} = {str(value).lower()}"
        elif isinstance(value, dict):
            val = generateStruct(value, indentlevel + 1)
            st += f",\n{indent}.{field_name} = {val}"
        elif isinstance(value, list) and all(isinstance(item, str) for item in value):
            escaped_items = [item.replace('\\', '\\\\').replace('"', '\\"') for item in value]
            items_str = ', '.join(f'"{item}"' for item in escaped_items)
            val = f"(const char *[]){{ {items_str} }}"
            st += f",\n{indent}.{field_name}_count = {len(value)}"
            st += f",\n{indent}.{field_name} = {val}"
        elif isinstance(value, list) and all(isinstance(item, dict) for item in value):
            subtype = field_name
            s = ", \n\t" + indent
            inner = s.join(generateStruct(d, indentlevel + 2) for d in value)
            val = f"({subtype}{typeSuffix}[]) {{\n{indent}\t{inner}\n{indent}}}"
            st += f",\n{indent}.{field_name}_count = {len(value)}"
            st += f",\n{indent}.{field_name} = {val}"
        elif isinstance(value, str):
            val = value.replace("\n", " ")
            val = val.replace('"', '\\"')
            st += f",\n{indent}.{field_name} = \"{val}\""
        elif isinstance(value, (int, float)):
            st += f",\n{indent}.{field_name} = {value}"
        else:
            st += f",\n{indent}.{field_name} = \"{value}\""
    if st[0] == ',':
        st = st[1:]
    lessindent = '\t' * (len(indent) - 1)
    st = "{" + st + f"\n{lessindent}}}"
    return st

def generate_header(doc):
    typedef_str = generateTypedef(doc, "", "instruction", set())
    header_str = "#ifndef INSTRUCTION_DEFS_H\n#define INSTRUCTION_DEFS_H\n\n#include <stdbool.h>\n\n" + typedef_str + "#endif\n"
    name = doc["name"].upper()
    struct_str = f"static const instruction{typeSuffix} instr_{doc["name"]} = " + generateStruct(doc, 1) + ";"
    header_str += "\n\n#ifndef INSTRUCTION_" + name + "_H\n#define INSTRUCTION_" + name + "_H\n\n" + struct_str + "\n#endif\n"
    return header_str 

def load_yaml(path):
    with open(path, "r") as file:
        docs = list(yaml.safe_load_all(file))
    return docs

if len(sys.argv) < 2:
    print("Usage: yaml_parser.py <input-yaml>")
    sys.exit(1)

input_yaml = sys.argv[1]

if not os.path.exists(input_yaml):
    print("Error: input file does not exist:", input_yaml, file=sys.stderr)
    sys.exit(2)

docs = load_yaml(input_yaml)

for doc in docs:
    output = generate_header(doc)
    output_header = f"./outputheader.h"
    with open(output_header, "w") as file:
        file.write(output)

    

