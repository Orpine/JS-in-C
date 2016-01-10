//
// Created by user on 2015/12/30.
//

#include "TinyJS.h"
#include <fstream>
#include <assert.h>

using namespace std;

void TinyJS::execute() {
    lex = new Lex(this->code);
    lex->getNextToken();
    scopes.clear();
    scopes.push_back(root);
    STATE state = RUNNING;
    while (lex->token.type != TK_EOF) {
        statement(state);
    }
}

void TinyJS::statement(STATE &state) {
    if (lex->token.type == TK_IDENTIFIER ||
        lex->token.type == TK_DEC_INT ||
        lex->token.type == TK_HEX_INT ||
        lex->token.type == TK_OCTAL_INT ||
        lex->token.type == TK_FLOAT ||
        lex->token.type == TK_STRING ||
        lex->token.type == TK_MINUS) {
        eval(state);
        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_L_LARGE_BRACKET) {
        block(state);
    } else if (lex->token.type == TK_SEMICOLON) {
        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_VAR) {
        lex->match(TK_VAR);
        string varName=lex->token.value;
        lex->match(TK_IDENTIFIER);

        auto scope=scopes.back();
        if(lex->token.type==TK_ASSIGN){
           lex->match(TK_ASSIGN);
           auto item=eval(state);
           scope->addUniqueChild(varName, item->var);
        }else if(lex->token.type==TK_SEMICOLON){
            scope->addUniqueChild(varName, new Var());
        }

        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_IF) {
        lex->match(TK_IF);
        lex->match(TK_L_BRACKET);
        auto cond = eval(state);
        lex->match(TK_R_BRACKET);
        STATE skipping = SKIPPING;
        statement(state == RUNNING && cond->var->getBool() ? state : skipping);
        if (lex->token.type == TK_ELSE) {
            lex->match(TK_ELSE);
            statement(state == RUNNING && cond->var->getBool() ? skipping : state);
        }
    } else if (lex->token.type == TK_WHILE) {
        lex->match(TK_WHILE);

        lex->match(TK_L_BRACKET);

        auto condStart = lex->posNow;
        auto cond = eval(state);
        auto condLex = lex->getSubLex(condStart);

        lex->match(TK_R_BRACKET);

        auto bodyStart = lex->posNow;
        STATE skipping = SKIPPING;
        statement(skipping);
        auto bodyLex = lex->getSubLex(bodyStart);

        if (state == RUNNING) {
            auto oriLex = lex;
            while (state == RUNNING && cond->var->getBool()) {
                lex = bodyLex;
                lex->reset();
                lex->getNextToken();
                statement(state);
                if (state == CONTINUE) {
                    state = RUNNING;
                }
                lex = condLex;
                lex->reset();
                lex->getNextToken();
                cond = eval(state);
            }
            if (state == BREAKING) {
                state = RUNNING;
            }
            lex = oriLex;
        }
        delete condLex;
        delete bodyLex;

    } else if (lex->token.type == TK_FOR) {
        lex->match(TK_FOR);
        lex->match(TK_L_BRACKET);

        statement(state);

        auto condStart = lex->posNow;
        auto cond = eval(state);
        auto condLex = lex->getSubLex(condStart);

        lex->match(TK_SEMICOLON);

        auto updateStart = lex->posNow;
        auto skipping = SKIPPING;
        // note here we can use eval since here must be a side-effect statement
        eval(skipping);
        auto updateLex = lex->getSubLex(updateStart);

        lex->match(TK_R_BRACKET);

        auto bodyStart = lex->posNow;
        statement(skipping);
        auto bodyLex = lex->getSubLex(bodyStart);


        if (state == RUNNING) {
            auto oriLex = lex;
            while (state == RUNNING && cond->var->getBool()) {
                lex = bodyLex;
                lex->reset();
                lex->getNextToken();
                statement(state);
                if (state == CONTINUE) {
                    state = RUNNING;
                }

                lex = updateLex;
                lex->reset();
                lex->getNextToken();
                eval(state);

                lex = condLex;
                lex->reset();
                lex->getNextToken();
                cond = eval(state);
            }
            if (state == BREAKING) {
                state = RUNNING;
            }
            lex = oriLex;
        }

        delete condLex;
        delete updateLex;
        delete bodyLex;

    } else if (lex->token.type == TK_RETURN) {
        lex->match(TK_RETURN);
        shared_ptr<VarLink> ret = nullptr;
        if (lex->token.type != TK_SEMICOLON) {
            ret = eval(state);
        }
        if (state == RUNNING) {
            auto retVar = scopes.back()->findChild(JS_RETURN_VAR);
            assert(retVar != nullptr);
            retVar->replaceWith(ret);
            state = SKIPPING;
        }
        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_FUNCTION) {
        lex->match(TK_FUNCTION);

        string name = lex->token.value;
        auto func = parseFuncDefinition(false);
        scopes.back()->addUniqueChild(name, func);

    } else if (lex->token.type == TK_EOF) {

    } else if (lex->token.type == TK_BREAK) {
        lex->match(TK_BREAK);
        if (state == RUNNING) {
            state = BREAKING;
        }
    } else if (lex->token.type == TK_CONTINUE) {
        lex->match(TK_CONTINUE);
        if (state == RUNNING) {
            state = CONTINUE;
        }
    } else {
        assert(0);
    }
}

shared_ptr<VarLink> TinyJS::eval(STATE &state) {
    auto lhs = ternary(state);
    if (lex->token.type == TK_ASSIGN || lex->token.type == TK_PLUS_EQUAL || lex->token.type == TK_MINUS_EQUAL) {
        if (state == RUNNING && !lhs->owned) {
            assert(lhs->name.length() != 0);
            lhs = root->addUniqueChild(lhs->name, lhs->var);
        }
        auto op = lex->token.type;
        lex->match(op);
        auto rhs = eval(state);
        if (state == RUNNING) {
            if (op == TK_ASSIGN) {
                lhs->replaceWith(rhs);
            } else {
                lhs->replaceWith(lhs->var->mathOp(rhs->var, op == TK_PLUS_EQUAL ? TK_PLUS : TK_MINUS));
            }
        }
    }
    return lhs;
}

shared_ptr<VarLink> TinyJS::ternary(STATE &state) {
    auto lhs = logic(state);
    while (lex->token.type == TK_QUESTION_MARK) {
        lex->match(TK_QUESTION_MARK);
        if (state == RUNNING) {
            STATE skipping = SKIPPING;
            if (lhs->var->getBool()) {
                lhs = logic(state);
                lex->match(TK_COLON);
                logic(skipping);
            } else {
                logic(skipping);
                lex->match(TK_COLON);
                lhs = logic(state);
            }
        } else {
            lhs = logic(state);
            lex->match(TK_COLON);
            lhs = logic(state);
        }
    }
    return lhs;
}

shared_ptr<VarLink> TinyJS::logic(STATE &state) {
    auto lhs = compare(state);
    while (lex->token.type == TK_BITWISE_AND || lex->token.type == TK_BITWISE_OR || lex->token.type == TK_BITWISE_XOR ||
           lex->token.type == TK_AND_AND || lex->token.type == TK_OR_OR) {
        lhs = make_shared<VarLink>(lhs->var->copyThis());
        auto op = lex->token.type;
        lex->match(op);
        bool getBool = false, shortCircuit = false;

        if (state == RUNNING) {
            if (op == TK_AND_AND) {
                shortCircuit = !lhs->var->getBool();
                getBool = true;
            } else if (op == TK_OR_OR) {
                shortCircuit = lhs->var->getBool();
                getBool = true;
            }
        }

        STATE skipping = SKIPPING;
        auto rhs = compare(shortCircuit ? skipping : state);

        if (state == RUNNING && !shortCircuit) {
            if (getBool) {
                lhs->replaceWith(new Var(lhs->var->getBool()));
                rhs->replaceWith(new Var(rhs->var->getBool()));
            }
            lhs->replaceWith(lhs->var->mathOp(rhs->var, op));
        }
    }
    return lhs;
}

shared_ptr<VarLink> TinyJS::compare(STATE &state) {
    auto lhs = shift(state);
    while (lex->token.type == TK_EQUAL || lex->token.type == TK_N_EQUAL ||
           lex->token.type == TK_TYPEEQUAL || lex->token.type == TK_N_TYPEEQUAL ||
           lex->token.type == TK_LESS || lex->token.type == TK_L_EQUAL ||
           lex->token.type == TK_GREATER || lex->token.type == TK_G_EQUAL) {
        lhs = make_shared<VarLink>(lhs->var->copyThis());
        auto op = lex->token.type;
        lex->match(op);
        auto rhs = shift(state);
        if (state == RUNNING) {
            lhs->replaceWith(lhs->var->mathOp(rhs->var, op));
        }
    }
    return lhs;
}

shared_ptr<VarLink> TinyJS::shift(STATE &state) {
    auto ret = expression(state);
    if (lex->token.type == TK_L_SHIFT || lex->token.type == TK_R_SHIFT) {
        ret = make_shared<VarLink>(ret->var->copyThis());
        auto op = lex->token.type;
        lex->match(op);
        auto opNum = expression(state);
        if (state == RUNNING) {
            if (op == TK_L_SHIFT) {
                ret->var->setInt(ret->var->getInt() << opNum->var->getInt());
            } else if (op == TK_R_SHIFT) {
                ret->var->setInt(ret->var->getInt() >> opNum->var->getInt());
            }
        }
    }
    return ret;
}

shared_ptr<VarLink> TinyJS::expression(STATE &state) {
    bool negative = false;
    if (lex->token.type == TK_MINUS) {
        lex->match(TK_MINUS);
        negative = true;
    }
    auto lhs = term(state);
    if (state == RUNNING && negative) {
        Var zero(0);
        lhs = make_shared<VarLink>(lhs->var->copyThis());
        lhs->replaceWith(zero.mathOp(lhs->var, TK_MINUS));
    }
    while (lex->token.type == TK_PLUS || lex->token.type == TK_MINUS || lex->token.type == TK_PLUS_PLUS ||
           lex->token.type == TK_MINUS_MINUS) {
        auto op = lex->token.type;
        lex->match(lex->token.type);
        if (op == TK_PLUS || op == TK_MINUS) {
            lhs = make_shared<VarLink>(lhs->var->copyThis());
            auto rhs = term(state);
            if (state == RUNNING) {
                lhs->replaceWith(lhs->var->mathOp(rhs->var, op));
            }
        } else {
            if (state == RUNNING) {
                Var one(1);
                lhs->replaceWith(lhs->var->mathOp(&one, op == TK_PLUS_PLUS ? TK_PLUS : TK_MINUS));
            }
        }
    }
    return lhs;
}

shared_ptr<VarLink> TinyJS::term(STATE &state) {  // handle *, /, % operator
    auto lhs = unary(state);
    while (lex->token.type == TK_MULTIPLY || lex->token.type == TK_DIVIDE || lex->token.type == TK_MOD) {
        lhs = make_shared<VarLink>(lhs->var->copyThis());
        auto op = lex->token.type;
        lex->match(op);
        auto rhs = unary(state);
        if (state == RUNNING) {
            lhs->replaceWith(lhs->var->mathOp(rhs->var, op));
        }
    }
    return lhs;
}

shared_ptr<VarLink> TinyJS::unary(STATE &state) { // handle ! operator
    if (lex->token.type == TK_NOT) {
        lex->match(TK_NOT);
        auto ret = make_shared<VarLink>(factor(state)->var->copyThis());
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
        auto ret = eval(state);
        lex->match(TK_R_BRACKET);
        return ret;
    } else if (lex->token.type == TK_DEC_INT || lex->token.type == TK_HEX_INT || lex->token.type == TK_OCTAL_INT ||
               lex->token.type == TK_FLOAT) {
        shared_ptr<VarLink> ret;
        if (lex->token.type == TK_FLOAT) {
            auto tmp = lex->token.getFloatData();
            ret = make_shared<VarLink>(new Var(tmp));
        } else {
            auto tmp = lex->token.getIntData();
            ret = make_shared<VarLink>(new Var(tmp));
        }
        lex->match(lex->token.type);
        return ret;
    } else if (lex->token.type == TK_STRING) {
        auto ret = make_shared<VarLink>(new Var(lex->token.value.substr(1, lex->token.value.length() - 2)));
        lex->match(TK_STRING);
        return ret;
    } else if (lex->token.type == TK_L_LARGE_BRACKET) {
        auto ret=parseJSON(state);
        return ret;
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

        bool child=false;
        lex->match(TK_IDENTIFIER);
        while (lex->token.type == TK_L_BRACKET || lex->token.type == TK_DOT) {
            if (lex->token.type == TK_L_BRACKET) { // ( means a function call
                shared_ptr<VarLink> func;
                if(child){
                    func=ret;
                }
                else {
                    func = findVar(lex->lastTk.value);
                }

                lex->match(TK_L_BRACKET);
                Var* args = parseArguments(state);

                auto originLex = lex;
                auto originScopes = scopes;

                ret = make_shared<VarLink>(callFunction(state, func, args, scopes));

                lex=originLex;
                scopes = originScopes;

                return ret;
            } else if (lex->token.type == TK_DOT) { // . means record access
                child=true;
                lex->match(TK_DOT);
                if (state == RUNNING) {
                    auto varName = lex->token.value;

                    ret = ret->var->findChildOrCreate(varName);
                    lex->match(TK_IDENTIFIER);
                }
            }
        }
        if (lex->token.type == TK_L_SQUARE_BRACKET) { // [ means array access
            lex->match(TK_L_SQUARE_BRACKET);

            auto idx = eval(state);
            if (state == RUNNING) {
                ret = ret->var->findChildOrCreate(idx->var->getString());
            }
            lex->match(TK_R_SQUARE_BRACKET);
        }
        return ret;
    } else if (lex->token.type == TK_L_SQUARE_BRACKET) { // [ means array declaration
        lex->match(TK_L_SQUARE_BRACKET);

        auto ret = state == RUNNING ? findVar(lex->token.value) : make_shared<VarLink>(new Var());
        if (state == RUNNING && !ret) {
            ret = make_shared<VarLink>(new Var("", VAR_ARRAY), lex->token.value);
        }
        Var* var=ret->var;

        int index=0;
        shared_ptr<VarLink> item;
        while (true){
            item=eval(state);

            if(item== nullptr){
                lex->match(TK_R_SQUARE_BRACKET);
                break;
            }
            stringstream ss;
            ss << index;
            var->addChild(ss.str(), item->var);
            if(lex->token.type == TK_R_SQUARE_BRACKET){
                lex->match(TK_R_SQUARE_BRACKET);
                break;
            }

            lex->match(TK_COMMA);
            index++;
        }

        return ret;
    } else if (lex->token.type == TK_FUNCTION) { // function declaration
        lex->match(TK_FUNCTION);

        Var* func = parseFuncDefinition(true);

        return make_shared<VarLink>(func);
    } else if (lex->token.type == TK_NEW) { // new an object
        // TODO: new an object
        return nullptr;
    }
    return nullptr;
}
shared_ptr<VarLink> TinyJS:: parseJSON(STATE& state){
    auto result = make_shared<VarLink>(new Var());
    auto var = result->var;

    lex->match(TK_L_LARGE_BRACKET);
    while(true){
        if(lex->token.type == TK_R_LARGE_BRACKET){
            lex->match(TK_R_LARGE_BRACKET);
            break;
        }

        string name = lex->token.value;
        lex->match(TK_IDENTIFIER);
        lex->match(TK_COLON);
        if(lex->token.type == TK_L_LARGE_BRACKET){//the child also has a json
            var->addUniqueChild(name, parseJSON(state)->var);
        }
        else if(lex->token.type == TK_FUNCTION){
            lex->match(TK_FUNCTION);
            var->addUniqueChild(name, parseFuncDefinition(true));
        }
        else{
            var->addUniqueChild(name, eval(state)->var);
        }

        if(lex->token.type == TK_COMMA){
            lex->match(TK_COMMA);
        }
    }

    return result;
}

Var* TinyJS:: parseArguments(STATE& state){
    Var* args = new Var();
    args->addChild(JS_ARGS_VAR,new Var());
    auto params = args->findChild(JS_ARGS_VAR);

    int index=0;
    while(true){
        if(lex->token.type == TK_R_BRACKET){
            lex->match(TK_R_BRACKET);
            break;
        }
        if(lex->token.type == TK_COMMA){
            lex->match(TK_COMMA);
        }

        auto arg = eval(state);
        stringstream ss;
        ss << index;
        params->var->addChild(ss.str(), arg->var);
        index++;
    }

    args->addChild(JS_PARAMETER_VAR, new Var(index));
    return args;
}

Var* TinyJS:: parseFuncDefinition(bool assign){
    auto func = new Var();
    func->type = VAR_FUNCTION;

    auto funcScopes = new Var();
    func->addChild(JS_SCOPE, funcScopes);
    int index=0;
    for(auto x: scopes){
        stringstream ss;
        ss << index;
        funcScopes->addChild(ss.str(), x);
        index++;
    }
    func->addChild(JS_SCOPE_NUM, new Var(index));

    if(!assign)
        lex->match(TK_IDENTIFIER);
    lex->match(TK_L_BRACKET);

    auto args = new Var();
    int count=0;
    while(true){
        if(lex->token.type == TK_R_BRACKET){
            lex->match(TK_R_BRACKET);
            break;
        }
        if(lex->token.type == TK_COMMA){
            lex->match(TK_COMMA);
        }

        stringstream ss;
        ss << count;
        args->addChild(ss.str(), new Var(lex->token.value));
        count++;
        lex->match(TK_IDENTIFIER);
    }

    func->addChild(JS_PARAMETER_VAR, new Var(count));
    func->addChild(JS_ARGS_VAR, args);
    func->addChild(JS_FUNCBODY_VAR, new Var(lex->getSubString()));
    return func;
}

Var* TinyJS:: callFunction(STATE& state, shared_ptr<VarLink> func,Var* args, vector<Var*> parent){
    auto num = args->findChild(JS_PARAMETER_VAR);
    auto funcNum = func->var->findChild(JS_PARAMETER_VAR);
    if(num->var->getInt() != funcNum->var->getInt()){
        cout<<"error: expected number of arguments is "<<funcNum->var->getInt()<<".But it's "<<num->var->getInt()<<" now."<<endl;
        return nullptr;
    }

    scopes.clear();
    auto funcScope = func->var->findChild(JS_SCOPE)->var;
    int number=func->var->findChild(JS_SCOPE_NUM)->var->getInt();
    for(int i=0;i<number;i++){
        stringstream ss;
        ss << i;
        scopes.push_back(funcScope->findChild(ss.str())->var);
    }

    Var* scope=new Var();
    scopes.push_back(scope);
    scope->addChild(JS_RETURN_VAR, new Var());

    int n = num->var->getInt();
    auto inArgus = args->findChild(JS_ARGS_VAR);
    auto outArgus = func->var->findChild(JS_ARGS_VAR);

    for(int i=0;i<n;i++){
        stringstream ss;
        ss << i;

        auto tmp = inArgus->var->findChild(ss.str())->var;
        if(tmp->isInt() || tmp->isBoolean() || tmp->isDouble()){
            scope->addChild( outArgus->var->findChild(ss.str())->var->getString(), tmp->copyThis());
        }else{
            scope->addChild( outArgus->var->findChild(ss.str())->var->getString(), tmp);
        }

    }

    string code = func->var->findChild(JS_FUNCBODY_VAR)->var->getString();
    lex = new Lex(code);
    lex->getNextToken();

    auto oriState = state;
    while (lex->token.type != TK_EOF) {
        statement(state);
    }
    state = oriState;

    auto ret = scope->findChild(JS_RETURN_VAR)->var;

    return ret;

}

void TinyJS::block(STATE &state) {
    lex->match(TK_L_LARGE_BRACKET);
    if (state == RUNNING) {
        while (lex->token.type != TK_EOF && lex->token.type != TK_R_LARGE_BRACKET) {
            statement(state);
        }
        lex->match(TK_R_LARGE_BRACKET);
    } else {
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

shared_ptr<VarLink> TinyJS::findVar(const string &varName) {
    for (int i = (int) scopes.size() - 1; i >= 0; i--) {
        auto var = scopes[i]->findChild(varName);
        if (var) {
            return var;
        }
    }
    return nullptr;
}
