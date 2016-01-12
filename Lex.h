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
#include <string>
#include <unordered_map>
#include <assert.h>

using namespace std;

//不能识别的运算符有： >>>, >>>=
enum TOKEN_TYPES {
//    TK_START,
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
    TK_N_TYPEEQUAL, //===
    TK_L_SHIFT,
    TK_L_SHIFT_EQUAL,
    TK_R_SHIFT,
    TK_R_SHIFT_EQUAL,
    TK_L_EQUAL,
    TK_G_EQUAL, //<=, >=
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
    TK_DOT, TK_SEMICOLON, TK_COLON, TK_COMMA, TK_QUESTION_MARK,
    TK_EOF
};


class Token {
public:
    Token() { };

    Token(TOKEN_TYPES _type, const string &_value) : type(_type), value(_value) { };

    void setToken(TOKEN_TYPES _type, const string &_value) {
        type = _type;
        value = _value;
    }

    int getIntData() {
        if (type == TK_DEC_INT) {
            return stoi(value, 0, 10);
        } else if (type == TK_OCTAL_INT) {
            return stoi(value, 0, 8);
        } else if (type == TK_HEX_INT) {
            return stoi(value, 0, 16);
        } else {
            assert(0);
            return 0;
        }
    }

    double getFloatData() {
        return atof(value.c_str());
    }

    TOKEN_TYPES type;
    string value;
};

class Lex {
private:
    map<string, TOKEN_TYPES> tokenMap;
    map<TOKEN_TYPES, string> invTokenMap;

    int getNextTokenInner(string &str, int startPos, Token &tk, Token &lastTk);
    //return this token's endpos + 1

public:
    string originalStr = "";

    Token lastTk;
    Token token;

    int tokenLastEnd;
    int posNow = 0;


    Lex();

    Lex(const string &str);

    void initialTokenMap();

    void setString(const string &str) {
        originalStr = str;
    }


    void match(TOKEN_TYPES expected_token);

    string getTokenStr(TOKEN_TYPES token);

    void reset();

    string getFunctionBody();

    Lex *getSubLex(int lastPosition);


    void getNextToken() {

        lastTk = token;
        if (posNow > 0) {
            tokenLastEnd = posNow - 1;
        }

        token.type = TK_NOT_VALID;
        while (posNow < originalStr.length() && token.type == TK_NOT_VALID) {
            posNow = getNextTokenInner(originalStr, posNow, token, lastTk);
        }
        if (posNow == originalStr.length() && token.type == TK_NOT_VALID) {
            token.type = TK_EOF;
        }

    }

};


#endif /* Lex_hpp */
