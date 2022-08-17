#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    //Kinds of token.
typedef enum {
    TK_RESERVED,    // reserved symbol
    TK_NUM,     //integer token
    TK_EOF,     // token to indecate end of input
} TokenKind;


// token structure 
typedef struct Token Token;
struct Token {
    TokenKind kind; // kind of token
    Token *next;    //next token 
    int val;        // the value when the kind is TK_NUM 
    char *str;      // token text
    int len;
};

//tokenize.c
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *tokenize(void);

// current token 
extern char *user_input;
extern Token *token;

//
//perse.c
//
// valuable of abstruct syntax tree
typedef enum {
    ND_ADD, //+
    ND_SUB, //-
    ND_MUL, //*
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT, // <
    ND_LE, //<=
    ND_NUM, // number
} NodeKind;

// node type of abstruct syntax tree
typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

//codegen.c

void gen(Node *node);

