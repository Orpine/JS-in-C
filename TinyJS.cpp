//
// Created by user on 2015/12/30.
//

#include "TinyJS.h"
#include <fstream>
#include <assert.h>

using namespace std;
#define clean(x) {if (x && !x->owned) {delete x;}}
//bool TinyJS::run() {
//    if (!init()) {
//        return false;
//    }
//
//    vector<Scope *> rootAR;
//    Scope *rootScope = getVarDef(lex.tokens, "root");
//    rootAR.push_back(rootScope);
//    execute(lex, rootAR);
//
//    shared_ptr<VarLink> varLink = rootAR[0]->vars["a"];
//    cout << varLink->name << endl;
//    cout << varLink->var->getInt() << endl;
//}
////
////Scope *TinyJS::getVarDef(vector<Token> &tokens, const string &scopeName) {
////    Scope *scope = new Scope(scopeName);
////
////    int index = 0;
////    while (index < tokens.size()) {
////        if (tokens[index].type == TK_L_BRACKET) {
////            int pair = 1;
////            while (pair > 0) {
////                index++;
////                if (tokens[index].type == TK_L_BRACKET) pair++;
////                if (tokens[index].type == TK_R_BRACKET) pair--;
////            }
////        }
////        if (tokens[index].type == TK_L_LARGE_BRACKET) {
////            int pair = 1;
////            while (pair > 0) {
////                index++;
////                if (tokens[index].type == TK_L_LARGE_BRACKET) pair++;
////                if (tokens[index].type == TK_R_LARGE_BRACKET) pair--;
////            }
////        }
////        if (tokens[index].type == TK_VAR) {
////            index++;
////            int len = 1;
////            if (tokens[index].type == TK_IDENTIFIER) {
////                Var *var = new Var();
////                shared_ptr<VarLink> varLink;
////                varLink.reset(new VarLink(var, tokens[index].value));
////                scope->vars[varLink->name] = varLink;
//////                scope->vars.push_back(varLink);
////                index++;
////                len++;
////                while (tokens[index].type != TK_SEMICOLON) {
////                    var->valueTokens.push_back(&tokens[index]);
////                    index++;
////                    len++;
////                }
////                var->len = len;
////            }
////            else {
////                cout << "There should be a variable after symbol var instead of " << tokens[index].value << "." << endl;
////                return nullptr;
////            }
////        }
////        if (tokens[index].type == TK_FUNCTION) {
////            index++;
////            int len = 1;
////            if (tokens[index].type == TK_IDENTIFIER) {
////                Var *var = new Var();
////                var->type = VAR_FUNCTION;
////                shared_ptr<VarLink> varLink;
////                varLink.reset(new VarLink(var, tokens[index].value));
////                scope->vars[varLink->name] = varLink;
//////                scope->vars.push_back(varLink);
////
////                //get the arguments
////                index++;
////                len++;
////                while (tokens[index].type != TK_L_LARGE_BRACKET) {
////                    if (tokens[index].type == TK_IDENTIFIER) {
////                        VarLink *arg = new VarLink(nullptr, tokens[index].value);
////                        var->args.push_back(arg);
////                    }
////                    index++;
////                    len++;
////                }
////                //get the tokens of the function
////                index++;
////                len++;
////                while (tokens[index].type != TK_R_LARGE_BRACKET) {
////                    var->valueTokens.push_back(&tokens[index]);
////                    index++;
////                    len++;
////                }
////                var->len = len;
////            }
////            else {
////                cout << "Expect a function name." << endl;
////                return nullptr;
////            }
////        }
////        index++;
////    }
////
////    return scope;
////}
//
//bool TinyJS::execute(Lex &lex, vector<Scope *> &scope) {
//    int index = 0;
//    while (index < lex.tokens.size()) {
//        TOKEN_TYPES type = lex.tokens[index].type;
//
//        if (type == TK_VAR) {
//            Var *var = findVar(lex.tokens[index + 1].value, scope);
//            index += var->len;
//            cout << "length is " << var->len << endl;
//            cout << "size is " << var->valueTokens.size() << endl;
//            if (var->valueTokens.size() != 0) {
//                statement(var, var->valueTokens, scope);
//            }
//        }
//
//        index++;
//    }
//}

void TinyJS::execute() {
    lex = new Lex(this->file);
    scopes.clear();
    scopes.push_back(root);
    STATE state = RUNNING;
    while (lex->token.type != TK_EOF) {
        eval(state);
    }
}

void TinyJS::eval(STATE &state) {
    if (lex->token.type == TK_IDENTIFIER ||
        lex->token.type == TK_DEC_INT ||
        lex->token.type == TK_HEX_INT ||
        lex->token.type == TK_OCTAL_INT ||
        lex->token.type == TK_FLOAT ||
        lex->token.type == TK_STRING ||
        lex->token.type == TK_MINUS) {
        base(state);
        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_L_LARGE_BRACKET) {
        lex->match(TK_L_LARGE_BRACKET);
//        bool executeDup = execute;
//        block(executeDup);
        lex->match(TK_R_LARGE_BRACKET);
    } else if (lex->token.type == TK_SEMICOLON) {
        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_VAR) {

    } else if (lex->token.type == TK_IF) {

    } else if (lex->token.type == TK_WHILE) {

    } else if (lex->token.type == TK_FOR) {

    } else if (lex->token.type == TK_RETURN) {

    } else if (lex->token.type == TK_FUNCTION) {

    } else if (lex->token.type == TK_EOF) {

    } else if (lex->token.type == TK_BREAK){
        state = BREAKING;
    } else {
        assert(0);
    }

}







//bool TinyJS::init() {
//    char buffer[1024];
//    string wholeText;
//
//    char endLineChar[2] = {'\n', '\0'};
//    string endline(endLineChar);
//
//
//    ifstream input(this->file, std::ios::in);
//    ofstream output("./Test4JS/tokens.txt", std::ios::out);
//
//    while (input.getline(buffer, 1024)) {
//        string line(buffer);
//        wholeText += line + endline;
//    }
//
//    Lex l(wholeText);
//    for (int j = 0; j < l.tokens.size(); j++) {
//        output << l.tokens[j].type;
//        output << "  ";
//    }
//    lex = l;
//
//    return lex.tkgot;
//}

//Var *TinyJS::statement(Var *var, vector<Token *> &tokens, vector<Scope *> &currentAR) {
//    cout << tokens[0]->type << endl;
//    if (tokens.size() == 1) {
//        const TOKEN_TYPES type = tokens[0]->type;
//        if (type == TK_DEC_INT || type == TK_OCTAL_INT || type == TK_HEX_INT) {
//            cout << "return " << tokens[0]->getIntData() << endl;
//            return new Var(tokens[0]->getIntData());
//        }
//        if (type == TK_FLOAT) {
//            return new Var(tokens[0]->getFloatData());
//        }
//    } else if (tokens[0]->type == TK_ASSIGN) {
//        cout << "assign " << tokens.size() << endl;
//        vector<Token *> tempTokens;
//        for (int index = 1; index < tokens.size(); index++) tempTokens.push_back(tokens[index]);
//        var->copy(statement(nullptr, tempTokens, currentAR));
//    }
//
//    return nullptr;
//}




shared_ptr<VarLink> TinyJS::term(STATE &state) {  // handle *, /, % operator
    auto lhs = unary(state);
    while (lex->token.type == TK_MULTIPLY || lex->token.type == TK_DIVIDE || lex->token.type == TK_MOD) {
        auto op = lex->token.type;
        lex->match(op);
        auto rhs = unary(state);
        if (state == RUNNING) {
            lhs->replaceWith(lhs->var->mathOp(rhs->var, op));
        }
    }
}

shared_ptr<VarLink> TinyJS::unary(STATE &state) { // handle ! operator
    if (lex->token.type == TK_NOT) {
        lex->match(TK_NOT);
        auto ret = factor(state);
        if (state == RUNNING) {
            ret->replaceWith(new Var(!(ret->var->getBool())));
        }
        return ret;
    } else {
        return factor(state);
    }
}

shared_ptr<VarLink> TinyJS::factor(STATE &state) {
    if (lex->token.type == TK_L_BRACKET) {
        lex->match(TK_L_BRACKET);
        auto ret = base(state);
        lex->match(TK_R_BRACKET);
        return ret;
    } else if (lex->token.type == TK_DEC_INT || lex->token.type == TK_HEX_INT || lex->token.type == TK_OCTAL_INT || lex->token.type == TK_FLOAT) {
        auto ret = make_shared<VarLink>(new Var(lex->token.type == TK_FLOAT ? lex->token.getFloatData() : lex->token.getIntData()));
        lex->match(lex->token.type);
        return ret;
    } else if (lex->token.type == TK_STRING) {
        auto ret = make_shared<VarLink>(new Var(lex->token.value));
        lex->match(TK_STRING);
        return ret;
    } else if (lex->token.type == TK_L_LARGE_BRACKET) {
        //  TODO: json data
        return nullptr;
    } else if (lex->token.type == TK_TRUE) {
        lex->match(TK_TRUE);
        return make_shared<VarLink>(new Var(true));
    } else if (lex->token.type == TK_FALSE) {
        lex->match(TK_FALSE);
        return make_shared<VarLink>(new Var(false));
    } else if (lex->token.type == TK_NULL) {
        lex->match(TK_NULL);
        return make_shared<VarLink>(new Var("null", VAR_NULL));
    } else if (lex->token.type == TK_UNDEIFNED) {
        lex->match(TK_UNDEIFNED);
        return make_shared<VarLink>(new Var("undefined", VAR_UNDEFINED));
    } else if (lex->token.type == TK_IDENTIFIER) {
        auto ret = state == RUNNING ? findVar(lex->token.value) : make_shared<VarLink>(new Var());
        if (state == RUNNING && !ret) {
            ret = make_shared<VarLink>(new Var(), lex->token.value);
        }
        auto parent = nullptr;
        lex->match(TK_IDENTIFIER);
        while (lex->token.type == TK_L_BRACKET || lex->token.type == TK_DOT) {
            if (lex->token.type == TK_L_BRACKET) { // ( means a function call
                ret = callFunction(state, ret, parent);
            } else if (lex->token.type == TK_DOT) { // . means record access
                // TODO: record access
//                lex->match(TK_DOT);
//                if (state == RUNNING) {
//                    auto varName = lex->token.value;
//                    auto child = ret->var->findChild(varName);
//
//                }
            }
        }
        if (lex->token.type == TK_L_SQUARE_BRACKET) { // [ means array access
            lex->match(TK_L_SQUARE_BRACKET);
            auto idx = base(state);
            lex->match(TK_R_SQUARE_BRACKET);
            if (state == RUNNING) {
                ret = ret->var->findChildOrCreate(idx->var->getString());
            }
            clean(idx);
        }
        return ret;
    } else if (lex->token.type == TK_L_SQUARE_BRACKET) { // [ means array declaration
        // TODO: array declaration
    } else if (lex->token.type == TK_FUNCTION) { // function declaration
        auto func = parseFuncDefinition();
        assert(func->name == ANONYMOUS);
        return func;
    } else if (lex->token.type == TK_NEW) { // new an object
        // TODO: new an object
    }
}



void TinyJS::block(STATE &state) {
    lex->match(TK_L_LARGE_BRACKET);
    while (lex->token.type != TK_EOF && lex->token.type != TK_R_LARGE_BRACKET && state == RUNNING) {
        eval(state);
    }
    if (state != RUNNING) {
        int bracket = 1;
        while (lex->token.type != TK_EOF && bracket) {
            if (lex->token.type == TK_L_LARGE_BRACKET) {
                bracket++;
            } else if (lex->token.type == TK_R_LARGE_BRACKET) {
                bracket--;
            }
            lex->match(lex->token.type);
        }
    }
}

//VarLink* TinyJS::statement(Var *var, vector<Token *> &tokens, vector<Scope *> &currentAR) {
//
//
//}





//Var *TinyJS::findVar(const string &name, vector<Scope *> &currentAR) {
//    int index = (int) currentAR.size() - 1;
//    while (index >= 0) {
//        Scope *scope = currentAR[index];
//        auto findVar = scope->vars.find(name);
//        if (findVar != scope->vars.end()) {
//            cout << "var found." << endl;
//            return findVar->second->var;
//        }
////        int scopeIndex = (int) scope->vars.size() - 1;
////        while (scopeIndex >= 0) {
////            if (scope->vars[scopeIndex]->name == name) {
////                return scope->vars[scopeIndex]->var;
////            }
////            scopeIndex--;
////        }
//        index--;
//    }
//
//    return nullptr;
//}

shared_ptr<VarLink> TinyJS::findVar(const string &varName) {
    for (int i = (int)scopes.size() - 1; i >= 0; i--) {
        auto var = scopes[i]->findChild(varName);
        if (var) {
            return var;
        }
    }
    return nullptr;
}
