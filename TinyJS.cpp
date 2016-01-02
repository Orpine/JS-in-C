//
// Created by user on 2015/12/30.
//

#include "TinyJS.h"
#include <fstream>
using namespace std;

bool TinyJS:: run(){
    if(!init()){
        return false;
    }

    vector<Scope*> rootAR;
    Scope* rootScope=getVarDef(lex.tokens, "root");
    rootAR.push_back(rootScope);
    execute(lex, rootAR);

    VarLink* varLink=rootAR[0]->vars[0];
    cout<<varLink->name<<endl;
    cout<<varLink->var->getInt()<<endl;
}
Scope* TinyJS:: getVarDef(vector<Token>& tokens, const string& scopeName){
    Scope* scope=new Scope(scopeName);

    int index=0;
    while(index<tokens.size()){
        if(tokens[index].type == TK_L_BRACKET){
            int pair=1;
            while(pair>0){
                index++;
                if(tokens[index].type == TK_L_BRACKET) pair++;
                if(tokens[index].type == TK_R_BRACKET) pair--;
            }
        }
        if(tokens[index].type == TK_L_LARGE_BRACKET){
            int pair=1;
            while(pair>0){
                index++;
                if(tokens[index].type == TK_L_LARGE_BRACKET) pair++;
                if(tokens[index].type == TK_R_LARGE_BRACKET) pair--;
            }
        }
        if(tokens[index].type==TK_VAR){
            index++;
            int len=1;
            if(tokens[index].type==TK_IDENTIFIER){
                Var* var=new Var();
                VarLink* varLink=new VarLink(var, tokens[index].value);
                scope->vars.push_back(varLink);
                index++;
                len++;
                while(tokens[index].type!=TK_SEMICOLON){
                    var->valueTokens.push_back(&tokens[index]);
                    index++;
                    len++;
                }
                var->len=len;
            }
            else{
                cout<<"There should be a variable after symbol var instead of "<<tokens[index].value<<"."<<endl;
                return nullptr;
            }
        }
        if(tokens[index].type==TK_FUNCTION){
            index++;
            int len=1;
            if(tokens[index].type == TK_IDENTIFIER){
                Var* var = new Var();
                var->type=VAR_FUNCTION;
                VarLink* varLink=new VarLink(var, tokens[index].value);
                scope->vars.push_back(varLink);

                //get the arguments
                index++;
                len++;
                while(tokens[index].type!=TK_L_LARGE_BRACKET){
                    if(tokens[index].type==TK_IDENTIFIER){
                        VarLink* arg=new VarLink(nullptr, tokens[index].value);
                        var->args.push_back(arg);
                    }
                    index++;
                    len++;
                }
                //get the tokens of the function
                index++;
                len++;
                while(tokens[index].type!=TK_R_LARGE_BRACKET){
                    var->valueTokens.push_back(&tokens[index]);
                    index++;
                    len++;
                }
                var->len=len;
            }
            else{
                cout<<"Expect a function name."<<endl;
                return nullptr;
            }
        }
        index++;
    }

    return scope;
}
bool TinyJS:: execute(Lex& lex, vector<Scope*>& scope){
    int index=0;
    while(index<lex.tokens.size()){
        TOKEN_TYPES type=lex.tokens[index].type;

        if(type==TK_VAR){
            Var* var=findVar(lex.tokens[index+1].value, scope);
            index+=var->len;
            cout<<"length is "<<var->len<<endl;
            cout<<"size is "<<var->valueTokens.size()<<endl;
            if(var->valueTokens.size()!=0){
                statement(var, var->valueTokens, scope);
            }
        }

        index++;
    }
}
bool TinyJS:: init(){
    char buffer[1024];
    string wholeText;

    char endLineChar[2] = {'\n', '\0'};
    string endline(endLineChar);


    ifstream input(this->file, std::ios::in);
    ofstream output("E:\\CppWorkspace\\TinyJS\\Test4JS\\tokens.txt", std::ios::out);

    while (input.getline(buffer, 1024)) {
        string line(buffer);
        wholeText += line + endline;
    }

    Lex l(wholeText);
    for (int j = 0; j < l.tokens.size(); j++) {
            output << l.tokens[j].type;
            output << "  ";
    }
    lex=l;

    return lex.tkgot;
}
Var* TinyJS:: statement(Var* var, vector<Token*>& tokens ,vector<Scope*>& currentAR){
    cout<<tokens[0]->type<<endl;
    if(tokens.size()==1){
        const TOKEN_TYPES type=tokens[0]->type;
        if(type==TK_DEC_INT || type==TK_OCTAL_INT || type==TK_HEX_INT){
            cout<<"return "<<tokens[0]->getIntData()<<endl;
            return new Var(tokens[0]->getIntData());
        }
        if(type==TK_FLOAT){
            return new Var(tokens[0]->getFloatData());
        }
    }
    else if(tokens[0]->type==TK_ASSIGN){
        cout<<"assign"<<tokens.size()<<endl;
        vector<Token*> tempTokens;
        for(int index=1;index<tokens.size();index++) tempTokens.push_back(tokens[index]);
        var->copy(statement(nullptr, tempTokens, currentAR));
    }

    return nullptr;
}

Var* TinyJS:: findVar(const string& name,vector<Scope*>& currentAR){
    int index=currentAR.size()-1;
    while(index>=0){
        Scope* scope=currentAR[index];
        int scopeIndex=scope->vars.size()-1;
        while(scopeIndex>=0){
            if(scope->vars[scopeIndex]->name == name){
                cout<<"var found."<<endl;
                return scope->vars[scopeIndex]->var;
            }
            scopeIndex--;
        }
        index--;
    }

    return nullptr;
}