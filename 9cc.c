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

typedef struct Token Token;

// token structure 
struct Token {
    TokenKind kind; // kind of token
    Token *next;    //next token address
    int val;        // the value when the kind is TK_NUM 
    char *str;      // token text
};
// current token 
Token *token;

// valuable of abstruct syntax tree
typedef enum {
    ND_ADD, //+
    ND_SUB, //-
    ND_MUL, //*
    ND_DIV, // /
    ND_NUM, // number
} NodeKind;

typedef struct Node Node;

// node type of abstruct syntax tree
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token->next;
    return true;
}

Node *expr();
Node *mul();
Node *primary();
Node *unary();

void expect(char op);
int expect_number();

Node *primary() {
    if(consume('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    return new_node_num(expect_number());
}

// mul = unary("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();
    for(;;) {
        if(consume('*')) {
            node = new_node(ND_MUL, node, unary());
        } else if(consume('/')) {
            node = new_node(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}
Node *expr() {
    Node *node = mul();

    for (;;) {
        if (consume('+')) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume('-')) {
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}
Node *unary() {
    if(consume('+')){
        return primary();
    }
    if(consume('-')){
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    return primary();
}








void gen(Node *node) {
    if(node->kind == ND_NUM) {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
    }
    printf("    push rax\n");

}





char *user_input;
//the function to notify the error
// same argument with printf
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}




void expect(char op) {
    if(token->kind != TK_RESERVED || token->str[0] != op){
        error_at(token->str, "it is not '%c'", op);
    } 
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM){
        error_at(token->str, "not number.");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

//make new token 
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

Token *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        // Punctuator
        if (strchr("+-*/()", *p)) {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        //Integer literal
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(token->str, "Can not tokenize");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}


int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "invalid argument\n");
        return 1;
    }

    //tokenize and perse
    user_input = argv[1];
    token = tokenize();
    Node *node =expr();

    // The biginning of assembly language
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}