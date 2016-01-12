//
// Created by user on 2015/12/30.
//

#ifndef TINYJS_TINYJS_H
#define TINYJS_TINYJS_H

#include "Lex.h"
#include "Var.h"
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

enum STATE {
    RUNNING,
    SKIPPING,
    BREAKING,
    CONTINUE
};

class TinyJS {
private:
    string code;
    Lex *lex;
    vector<Var *> scopes;

    void statement(STATE &state);

    void block(STATE &state);

    shared_ptr<VarLink> eval(STATE &state);

    shared_ptr<VarLink> ternary(STATE &state);

    shared_ptr<VarLink> logic(STATE &state);

    shared_ptr<VarLink> compare(STATE &state);

    shared_ptr<VarLink> shift(STATE &state);

    shared_ptr<VarLink> expression(STATE &state);

    shared_ptr<VarLink> term(STATE &state);

    shared_ptr<VarLink> unary(STATE &state);

    shared_ptr<VarLink> factor(STATE &state);

    shared_ptr<VarLink> findVar(const string &varName);

    shared_ptr<VarLink> parseJSON(STATE &state);


public:
    TinyJS(const string &file) {
        const int maxSize = 1000000; // support 1MB code;
        char code[maxSize];
        FILE *fin = fopen(file.c_str(), "r");
        fread(code, 1, maxSize, fin);
        this->code = string(code);
        root = (new Var(VAR_BLANK, VAR_OBJECT))->ref();
    }

    Var *root;

    void execute();

    Var *parseFuncDefinition(bool assign);

    Var *parseArguments(STATE &state);

    Var *callFunction(STATE &, shared_ptr<VarLink> func, Var *args);

    Var *newObject(STATE &, shared_ptr<VarLink> func, Var *args);
};


#endif //TINYJS_TINYJS_H
