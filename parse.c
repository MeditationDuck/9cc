#include "9cc.h"

static Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}


Node *expr(void);
static Node *equality(void);
static Node *relational(void);
static Node *add(void);
static Node *mul(void);
static Node *unary(void);
static Node *primary(void);


// expr = equality
Node *expr() {
    return equality();
}

// equality = relational ("==" relatinal | "!=" relational)*
static Node *equality() {
    Node *node = relational();

    for(;;) {
        if(consume("==")) {
            node = new_binary(ND_EQ, node, relational());
        } else if(consume("!=")) {
            node = new_binary(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational() {
    Node *node = add();
    for(;;){
        if(consume("<")) {
            node = new_binary(ND_LT, node, add());
        } else if(consume("<=")) {
            node = new_binary(ND_LE, node, add());
        } else if(consume(">")) {
            node = new_binary(ND_LT, add(), node);
        } else if(consume(">=")) {
            node = new_binary(ND_LE, add(), node);
        } else {
            return node;
        }
    }

}

// add = mul ("+" mul | "-" mul)*
static Node *add() {
    Node *node = mul();

    for(;;){
        if(consume("+")){  
            node = new_binary(ND_ADD, node, mul());
        } else if(consume("-")){
            node = new_binary(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

// mul = unary("*" unary | "/" unary)*
static Node *mul() {
    Node *node = unary();
    for(;;) {
        if(consume("*")) {
            node = new_binary(ND_MUL, node, unary());
        } else if(consume("/")) {
            node = new_binary(ND_DIV, node, unary());
        } else {
            return node;
        }
    }
}

// unary = ("+" | "-")? primary
static Node *unary() {
    if(consume("+")){
        return primary();
    }
    if(consume("-")){
        return new_binary(ND_SUB, new_num(0), primary());
    }
    return primary();
}
// primary = num | "(" expr ")"
static Node *primary() {

    if(consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    return new_num(expect_number());
}