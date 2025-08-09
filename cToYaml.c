#include <stdio.h>
#include "outputheader.h"

void print_indent(FILE *fp, int level) {
    for (int i = 0; i < level; i++) {
        fprintf(fp, "  ");
    }
}

void print_escaped_string(FILE *fp, const char *s) {
    fprintf(fp, "\"");
    while (*s) {
        if (*s == '\"') {
            fprintf(fp, "\\\"");
        } else if (*s == '\\') {
            fprintf(fp, "\\\\");
        } else {
            fprintf(fp, "%c", *s);
        }
        s++;
    }
    fprintf(fp, "\"");
}

void print_string_field(FILE *fp, const char *key, const char *value, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "%s: ", key);
    print_escaped_string(fp, value);
    fprintf(fp, "\n");
}

void print_bool_field(FILE *fp, const char *key, bool value, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "%s: %s\n", key, value ? "true" : "false");
}

void print_list_strings(FILE *fp, const char *key, const char **list, int count, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "%s:\n", key);
    for (int i = 0; i < count; i++) {
        print_indent(fp, indent + 1);
        fprintf(fp, "- ");
        print_escaped_string(fp, list[i]);
        fprintf(fp, "\n");
    }
}

void print_access(FILE *fp, const access_t *acc, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "access:\n");
    print_string_field(fp, "s", acc->s, indent + 1);
    print_string_field(fp, "u", acc->u, indent + 1);
    print_string_field(fp, "vs", acc->vs, indent + 1);
    print_string_field(fp, "vu", acc->vu, indent + 1);
}

void print_variable(FILE *fp, const variables_t *var, int indent) {
    print_string_field(fp, "name", var->name, indent - 4);
    print_string_field(fp, "location", var->location, indent);
}

void print_encoding(FILE *fp, const encoding_t *enc, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "encoding:\n");
    print_string_field(fp, "match", enc->match, indent + 1);
    print_indent(fp, indent + 1);
    fprintf(fp, "variables:\n");
    for (int i = 0; i < enc->variables_count; i++) {
        print_indent(fp, indent + 2);
        fprintf(fp, "- ");
        print_variable(fp, &enc->variables[i], indent + 3);
    }
}

void print_cert_normative_rule(FILE *fp, const cert_normative_rules_t *rule, int indent) {
    print_string_field(fp, "id", rule->id, indent - 4);
    print_string_field(fp, "name", rule->name, indent);
    print_string_field(fp, "description", rule->description, indent);
    print_list_strings(fp, "doc_links", rule->doc_links, rule->doc_links_count, indent);
}

void print_cert_normative_rules(FILE *fp, const cert_normative_rules_t *rules, int count, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "cert_normative_rules:\n");
    for (int i = 0; i < count; i++) {
        print_indent(fp, indent + 1);
        fprintf(fp, "- ");
        print_cert_normative_rule(fp, &rules[i], indent + 2);
    }
}

void print_cert_test_procedure(FILE *fp, const cert_test_procedures_t *proc, int indent) {
    print_string_field(fp, "id", proc->id, indent - 4);
    print_string_field(fp, "description", proc->description, indent);
    print_list_strings(fp, "normative_rules", proc->normative_rules, proc->normative_rules_count, indent);
    print_string_field(fp, "steps", proc->steps, indent);
}

void print_cert_test_procedures(FILE *fp, const cert_test_procedures_t *procs, int count, int indent) {
    print_indent(fp, indent);
    fprintf(fp, "cert_test_procedures:\n");
    for (int i = 0; i < count; i++) {
        print_indent(fp, indent + 1);
        fprintf(fp, "- ");
        print_cert_test_procedure(fp, &procs[i], indent + 2);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_yaml_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "w");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    print_string_field(fp, "$schema", instr_mock.schema, 0);
    print_string_field(fp, "kind", instr_mock.kind, 0);
    print_string_field(fp, "name", instr_mock.name, 0);
    print_string_field(fp, "long_name", instr_mock.long_name, 0);
    print_string_field(fp, "description", instr_mock.description, 0);
    print_string_field(fp, "definedBy", instr_mock.definedBy, 0);
    print_string_field(fp, "assembly", instr_mock.assembly, 0);
    print_encoding(fp, &instr_mock.encoding, 0);
    print_access(fp, &instr_mock.access, 0);
    print_bool_field(fp, "data_independent_timing", instr_mock.data_independent_timing, 0);
    print_string_field(fp, "operation()", instr_mock.operation, 0);
    print_string_field(fp, "sail()", instr_mock.sail, 0);
    print_cert_normative_rules(fp, instr_mock.cert_normative_rules, instr_mock.cert_normative_rules_count, 0);
    print_cert_test_procedures(fp, instr_mock.cert_test_procedures, instr_mock.cert_test_procedures_count, 0);

    fclose(fp);
    return 0;
}