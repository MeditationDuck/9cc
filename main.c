#include "9cc.h"

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "invalid argument\n");
        return 1;
    }

    //tokenize and perse
    user_input = argv[1];
    token = tokenize();
    Node *node = expr();

    // The biginning of assembly language
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}