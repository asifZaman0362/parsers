#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* 
 *
 * E -> T E'
 * E' -> + E | #
 * T -> F T'
 * T' -> * T | #
 * F -> (E) | a | b | c
 *
 */

#define nullptr 0

typedef enum {
    A, B, C, PLUS, STAR, L_PAREN, R_PAREN
} token_type_e;

typedef struct {
    char lexeme;
    token_type_e type;
} token_t;

typedef struct node {
    token_t token;
    struct node *left;
    struct node *right;
} node_t;

token_t token_stream[256];
unsigned long pos = 0;

node_t *expression();
node_t *term();
node_t *factor();

void delete_tree(node_t *node) {
    if (node == nullptr) return;
    delete_tree(node->left);
    delete_tree(node->right);
    free(node);
}

node_t *factor() {
    if (token_stream[pos].type == A || token_stream[pos].type == B || token_stream[pos].type == C) {
        node_t *parent = (node_t*) malloc(sizeof(node_t));
        parent->token.lexeme = token_stream[pos].lexeme;
        parent->token.type = token_stream[pos].type;
        parent->left = nullptr;
        parent->right = nullptr;
        pos++;
        return parent;
    } else if (token_stream[pos].type == L_PAREN) {
        pos++;
        node_t *new_node = expression();
        if (new_node != nullptr && token_stream[pos++].type == R_PAREN) {
            return new_node;
        } else {
            fprintf(stdout, "Unmatched '('\n");
            delete_tree(new_node);
            return nullptr;
        }
    } else return nullptr;
}

node_t *term() {
    node_t *new_node = factor();
    if (new_node != nullptr) {
        if (token_stream[pos].type == STAR) {
            pos++;
            node_t *right = term();
            if (right != nullptr) {
                node_t *parent = (node_t*) malloc(sizeof(node_t));
                parent->token.type = PLUS;
                parent->token.lexeme = '*';
                parent->left = new_node;
                parent->right = right;
                return parent;
            } else {
                fprintf(stdout, "Expected a term after *!\n");
                delete_tree(new_node);
                return nullptr;
            }
        }
        return new_node;
    }
    return nullptr;
}

node_t *expression() {
    node_t *new_node = term();
    if (new_node != nullptr) {
        if (token_stream[pos].type == PLUS) {
            pos++;
            node_t *right = expression();
            if (right != nullptr) {
                node_t *parent = (node_t*) malloc(sizeof(node_t));
                parent->token.type = PLUS;
                parent->token.lexeme = '+';
                parent->left = new_node;
                parent->right = right;
                return parent;
            } else {
                fprintf(stdout, "Expected an expression after +!\n");
                return nullptr;
            }
        }
        return new_node;
    } return nullptr;
}

void lex(const char *buffer) {
    unsigned long long pos = 0, count = 0;
    for (const char *c = buffer; *c != '\0'; c++) {
        token_t token;
        if (*c == 'a') {
            token.lexeme = 'a';
            token.type = A;
        } else if (*c == 'b') {
            token.lexeme = 'b';
            token.type = B;
        } else if (*c == 'c') {
            token.lexeme = 'c';
            token.type = C;
        } else if (*c == '*') {
            token.lexeme = '*';
            token.type = STAR;
        } else if (*c == '+') {
            token.lexeme = '+';
            token.type = PLUS;
        } else if (*c == '(') {
            token.lexeme = '(';
            token.type = L_PAREN;
        } else if (*c == ')') {
            token.lexeme = ')';
            token.type = R_PAREN;
        } else if (*c == '\r' || *c == ' ' || *c == '\t' || *c == '\n') {
            continue;
        } else {
            fprintf(stdout, "Unexpected token %c at position: %llu", *c, pos);
            exit(-1);
        } if (count >= sizeof(token_stream) / sizeof(token_t*)) {
            fprintf(stdout, "Program produces too many tokens than the compiler can handle!\nHalting...\n");
            exit(-1);
        } else token_stream[count++] = token;
    }
}

void print_tree(node_t *root, int depth) {
    fprintf(stdout, "\n");
    for (unsigned int i = 0; i < depth; i++) {
        fprintf(stdout, "\t");
    }
    if (root != nullptr)
        fprintf(stdout, "%c", root->token.lexeme);
    if (root->left != nullptr)
        print_tree(root->left, depth + 1);
    if (root->right != nullptr)
        print_tree(root->right, depth + 1);
}

node_t *parse() {
    node_t *root = expression();
    if (root != nullptr) {
        fprintf(stdout, "Parsed expression successfully!\n");
        print_tree(root, 0);
    } else {
        fprintf(stdout, "Parser error!\nExiting...\n");
        exit(-1);
    }
}

int main (void) {
    char input_buffer[2048];
    fprintf(stdout, "Enter code:\n");
    fgets(input_buffer, 2048, stdin);
    lex(input_buffer);
    node_t *root = parse();
    delete_tree(root);
    //print_tree(root);
    return 0;
}
