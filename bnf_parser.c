#include "bnf_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
 * BNF notation
 * <symbol> ::= __expression__
 * where:
 *  <symbol> is a nonterminal and the expression contains one or more sequences of either terminal
 *  or nonterminal symbols;
 *  ::= means that the symbol on the left must be replaced by the expression on the right
 *  more sequences of symbols are separated by the vertical bar "|", indicating a choice of possible
 *  subsitions
 *  
*/

void read_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file! Please make sure the file exists in the provided location\n");
        exit(-1);
    } else {
        char buffer[4096];
        fread(buffer, 4096, 4096, file);
        parse(buffer);
    }
}

typedef unsigned short ushort;

typedef enum {
    T_TERMINAL,
    T_NONTERMINAL,
    T_H_BAR,
    T_DEFINITION_SYM
} token_type_e;

typedef struct {
    ushort begin;
    ushort end;
} short_range_t;

typedef struct {
    token_type_e token_type;
    short_range_t slice_indices;
} token_t;

ushort seek_stream(const char **stream, const char delimiter) {
    ushort len = 0;
    for (; **stream != delimiter; (*stream)++, len++);
    return len;
}

void parse(const char *code) {
    token_t token_stream[256];
    ushort begin = 0, end = 0, count = 0;
    for (const char *iter = code; *iter != '\0'; iter++) {
        const char c = *iter;
        if (c == ' ') continue;
        else if (c == '<') {
            end = seek_stream(&iter, '>');
            if (*iter == '\0') exit(-1);
            token_t nonterminal = {
                .token_type = T_NONTERMINAL,
                .slice_indices = {
                    .begin = begin,
                    .end = end - 1
                }
            };
            token_stream[count++] = nonterminal;
        } else if (c == '"') {
            end = seek_stream(&iter, '"');
            if (*iter == '\0') exit(-1);
            token_t terminal = {
                .token_type = T_TERMINAL,
                .slice_indices = {
                    .begin = begin,
                    .end = end - 1
                }
            };
            token_stream[count++] = terminal;
        } else if (c == '|') {
            token_t hbar = {
                .token_type = T_H_BAR
            };
            token_stream[count++] = hbar;
        } else if (c == ':') {
            if (*(iter++) == ':' && *(iter++) == '=') {
                token_t def = {
                    .token_type = T_DEFINITION_SYM
                };
                token_stream[count++] = def;
            }
        } else {
            exit(-1);
        }
    }
}   
