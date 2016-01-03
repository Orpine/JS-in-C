//
// Created by user on 2015/12/30.
//

#ifndef TINYJS_TINYJS_H
#define TINYJS_TINYJS_H
#include "Lex.h"
#include "var.h"
#include <string>
#include <vector>

using namespace std;

enum STATE {
    RUNNING,
    SKIPPING,
    BREAKING
};

class TinyJS {
private:
    string code;
    Lex *lex;
    vector<Var *> scopes;

    void eval(STATE& state);
    void block(STATE& state);
    shared_ptr<VarLink> base(STATE& state);
    shared_ptr<VarLink> ternary(STATE& state);
    shared_ptr<VarLink> logic(STATE& state);
    shared_ptr<VarLink> compare(STATE& state);
    shared_ptr<VarLink> shift(STATE& state);
    shared_ptr<VarLink> expression(STATE& state);
    shared_ptr<VarLink> term(STATE& state);
    shared_ptr<VarLink> unary(STATE& state);
    shared_ptr<VarLink> factor(STATE& state);

    shared_ptr<VarLink> findVar(const string &varName);

public:
    TinyJS(const string& file){
        const int maxSize = 1000000; // support 1MB code;
        char code[maxSize];
        FILE *fin = fopen(file.c_str(), "r");
        fread(code, 1, maxSize, fin);
        this->code=string(code);
        root = (new Var(VAR_BLANK, VAR_OBJECT))->ref();
    }
    Var *root;

    void execute();

//    bool run();
//    bool init();
//    Scope* getVarDef(vector<Token>& tokens, const string& scopeName);
//    bool execute(Lex& lex, vector<Scope*>& scope);
//    Var* statement(Var* var,vector<Token*>& tokens ,vector<Scope*>& currentAR);
//    Var* findVar(const string& name,vector<Scope*>& currentAR);
};


#endif //TINYJS_TINYJS_H
