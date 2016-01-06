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
            if (retVar->var->type == VAR_FUNCTION) {
                retVar->var->addChilds(scopes.back());
            }
        }
        lex->match(TK_SEMICOLON);
    } else if (lex->token.type == TK_FUNCTION) {

    } else if (lex->token.type == TK_EOF) {

    } else if (lex->token.type == TK_BREAK) {
        state = BREAKING;
    } else if (lex->token.type == TK_CONTINUE) {
        state = CONTINUE;
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
        bool getBool = false, shortCircuit = false;

        if (state == RUNNING) {
            if (lex->token.type == TK_AND_AND) {
                shortCircuit = !lhs->var->getBool();
                getBool = true;
            } else if (lex->token.type == TK_OR_OR) {
                shortCircuit = lhs->var->getBool();
                getBool = true;
            }
        }

        STATE skipping = SKIPPING;
        auto rhs = compare(shortCircuit ? state : skipping);

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
                // TODO: function call
//                ret = callFunction(state, ret, parent);
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
            // TODO: array access
            auto idx = eval(state);
            if (state == RUNNING) {
                ret = ret->var->findChildOrCreate(idx->var->getString());
            }
            lex->match(TK_R_SQUARE_BRACKET);
        }
        return ret;
    } else if (lex->token.type == TK_L_SQUARE_BRACKET) { // [ means array declaration
        // TODO: array declaration
        cout<<"array declaration"<<endl;
        lex->match(TK_L_SQUARE_BRACKET);
        auto ret = state == RUNNING ? findVar(lex->token.value) : make_shared<VarLink>(new Var());
        if (state == RUNNING && !ret) {
            ret = make_shared<VarLink>(new Var(), lex->token.value);
        }
        Var* var=ret->var;

        int index=0;
        shared_ptr<VarLink> item;
        while (true){
            item=eval(state);

            if(item== nullptr){
                break;
            }
            stringstream ss;
            ss << index;
            var->addChild(ss.str(), item->var);
            if(lex->token.type == TK_R_SQUARE_BRACKET){
                break;
            }

            lex->match(TK_COMMA);
            index++;
        }

        lex->match(TK_SEMICOLON);
        return ret;
    } else if (lex->token.type == TK_FUNCTION) { // function declaration
        // TODO: function parse
        auto func = parseFuncDefinition();
//        assert(func->name == ANONYMOUS);
        return func;
    } else if (lex->token.type == TK_NEW) { // new an object
        // TODO: new an object
        return nullptr;
    }
    return nullptr;
}

shared_ptr<VarLink> TinyJS:: parseFuncDefinition(){
    auto func = make_shared<VarLink>(new Var() ,lex->token.value);

    lex->match(TK_IDENTIFIER);
    lex->match(TK_L_BRACKET);

    while(true){

    }

    return func;
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
