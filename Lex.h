//
//  Lex.hpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//

#ifndef Lex_hpp
#define Lex_hpp

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

//不能识别的运算符有： >>>, >>>=
enum TOKEN_TYPES{
    TK_START,
    TK_NOT_VALID,

    TK_IDENTIFIER = 256,

    TK_LET,
    TK_VAR, TK_CONST,
    TK_DEC_INT, TK_OCTAL_INT, TK_HEX_INT,
    TK_FLOAT, TK_STRING, TK_OBJECT,
    TK_NULL, TK_UNDEIFNED,
    TK_TRUE, TK_FALSE,
    TK_INFINITY, TK_NAN,

    TK_N_EQUAL,
    TK_EQUAL,
    TK_TYPEEQUAL,
    TK_N_TYPEEQUAL,//===
            TK_L_SHIFT,
    TK_L_SHIFT_EQUAL,
    TK_R_SHIFT,
    TK_R_SHIFT_EQUAL,
    TK_L_EQUAL,
    TK_G_EQUAL,//<=, >=
            TK_PLUS_EQUAL,
    TK_MINUS_EQUAL,
    TK_MULTI_EQUAL,
    TK_DIV_EQUAL,
    TK_MOD_EQUAL,
    TK_AND_EQUAL,
    TK_OR_EQUAL,
    TK_XOR_EQUAL,
    TK_AND_AND,
    TK_OR_OR,
    TK_PLUS_PLUS,
    TK_MINUS_MINUS,

    TK_IF,
    TK_ELSE,
    TK_SWITCH,
    TK_FOR,
    TK_WHILE,
    TK_DO,
    TK_BREAK,
    TK_CONTINUE,
    TK_EMPTY,

    TK_TYPEOF,

    TK_FUNCTION,
    TK_RETURN,
    TK_CLASS,
    TK_SUPER,
    TK_NEW,
    TK_DELETE,
    TK_THIS,


    //单目
            TK_L_SQUARE_BRACKET, TK_R_SQUARE_BRACKET,
    TK_L_BRACKET, TK_R_BRACKET,
    TK_L_LARGE_BRACKET, TK_R_LARGE_BRACKET,
    TK_ASSIGN,
    TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE, TK_MOD, TK_EXPONENT,
    TK_NOT,
    TK_BITWISE_NOT, TK_BITWISE_AND, TK_BITWISE_OR, TK_BITWISE_XOR,

    TK_LESS, TK_GREATER,

    //标点符号
            TK_DOT, TK_SEMICOLON, TK_COLON, TK_COMMA, TK_QUESTION_MARK
};



class Token{
public:
    Token() {};
    Token(TOKEN_TYPES _type, const string& _value): type(_type), value(_value) {};
    void setToken(TOKEN_TYPES _type, const string& _value){
        type = _type;
        value = _value;
    }
    int getIntData(){
        return atoi(value.c_str());
    }
    double getFloatData(){
        return atof(value.c_str());
    }
    TOKEN_TYPES type;
    string value;
};

class Lex{
private:
    map<string, TOKEN_TYPES> tokenMap;
    //return this token's endpos + 1
    int getNextToken(string& str, int startPos, Token& tk, Token& lastTk);

public:
    bool tkgot=false;
    string originalStr = "";
    vector<Token> tokens;

    Lex(){initialTokenMap();};
    Lex(const string& str): originalStr(str){
        initialTokenMap();
        getLex();
    };

    void initialTokenMap();
    void setString(const string& str){
        originalStr = str;
    }
    void getLex(){
        if (originalStr == "") {
            cout << "The lex has no string, use setString and try again" << endl;
            return;
        }

        int oldPos = 0;
        int pos = 0;
        Token tk;
        tk.type = TK_START;
        Token lastTk;
        while (pos != originalStr.length()) {
            oldPos = pos;
            lastTk = tk;
            pos = getNextToken(originalStr, pos, tk, lastTk);
            if (pos == -1) {
                cout << "Lex error: " << originalStr.substr(oldPos) << endl;
                return;
            }
            if (tk.type != TK_NOT_VALID) {
                tokens.push_back(tk);
            }
        }
        tkgot=true;
    }
};

#endif /* Lex_hpp */
