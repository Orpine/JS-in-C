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

class TinyJS {
private:
    string file;
    Lex lex;
public:
    TinyJS(string& file){
        this->file=file;
    }
    bool run();
    bool init();
    Scope* getVarDef(vector<Token>& tokens, const string& scopeName);
    bool execute(Lex& lex, vector<Scope*>& scope);
    Var* statement(Var* var,vector<Token*>& tokens ,vector<Scope*>& currentAR);
    Var* findVar(const string& name,vector<Scope*>& currentAR);
};


#endif //TINYJS_TINYJS_H
