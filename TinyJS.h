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
    string file;
    Lex *lex;
    vector<Var *> scopes;

    void eval(STATE& state);
    void block(STATE& state);
    VarLink* ternary(STATE& state);
    VarLink* logic(STATE& state);
    VarLink* condition(STATE& state);
    VarLink* shift(STATE& state);
    VarLink* expression(STATE& state);
    shared_ptr<VarLink> term(STATE& state);
    shared_ptr<VarLink> unary(STATE& state);
    shared_ptr<VarLink> factor(STATE& state);

    shared_ptr<VarLink> findVar(const string &varName);

public:
    TinyJS(string& file){
        this->file=file;
    }
    Var *root;

    bool run();
    bool init();
    Scope* getVarDef(vector<Token>& tokens, const string& scopeName);
    bool execute(Lex& lex, vector<Scope*>& scope);
    Var* statement(Var* var,vector<Token*>& tokens ,vector<Scope*>& currentAR);
    Var* findVar(const string& name,vector<Scope*>& currentAR);
};


#endif //TINYJS_TINYJS_H
