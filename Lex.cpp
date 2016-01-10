//
//  Lex.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//

#include "Lex.h"
#include <regex>
#include <sstream>

//the tokens that can appear before numbers with '+/-' prefix
set<TOKEN_TYPES> tokenBeforePrefix{
    TK_L_BRACKET,
    TK_ASSIGN,
    TK_PLUS_EQUAL,
    TK_MINUS_EQUAL,
    TK_MULTI_EQUAL,
    TK_DIV_EQUAL,
    TK_MOD_EQUAL,
    TK_AND_EQUAL,
    TK_OR_EQUAL,
    TK_L_EQUAL,
    TK_G_EQUAL, };

Lex::Lex(){
    initialTokenMap();
    
    token.type = TK_NOT_VALID;
    lastTk.type = TK_NOT_VALID;
    
    tokenStart = 0;
    tokenEnd = 0;
    tokenLastEnd = 0;
    posNow = 0;
}

Lex::Lex(const string &str) : originalStr(str) {
    initialTokenMap();
    token.type = TK_NOT_VALID;
    lastTk.type = TK_NOT_VALID;
    
    tokenLastEnd = 0;
    
    tokenStart = 0;
    tokenEnd = (int)str.length();
    posNow = 0;
    //        getLex();
};

void Lex::reset(){
    token.type = TK_NOT_VALID;
    lastTk.type = TK_NOT_VALID;
    
    tokenLastEnd = 0;
    
    posNow = 0;
}



int Lex::getNextTokenInner(string &str, int startPos, Token &tk, Token &lastTk) {
    regex floatExp("^[\\+-]?((([1-9]\\d*)?\\.\\d+|[1-9]\\d*(\\.\\d*)?)[eE][\\+-]?[1-9]\\d*|([1-9]\\d*)?\\.\\d+)");
    regex decExp("^([\\+-]?[1-9]\\d*|0)");
    regex octalExp("^0[1-7][0-7]*");
    regex hexExp("^0[xX][1-9a-fA-F][0-9a-fA-F]*");

    while (isspace(str.at(startPos))) {
        startPos++;
        if (startPos == str.length()) {
            tk.type = TK_NOT_VALID;
            return startPos;
        }
    }

    int i = startPos;
    bool maybeANum = false;

    if (str.at(i) == '.' || isdigit(str.at(i))) {
        maybeANum = true;
    } else if (str.at(i) == '+' || str.at(i) == '-') {
        if (tokenBeforePrefix.find(lastTk.type) != tokenBeforePrefix.end()) {
            maybeANum = true;
        }
    }

    if (maybeANum) {
//        if ( (str.at(i) != '+' && str.at(i) != '-') || (lastTk.type != TK_START && lastTk.type != TK_IDENTIFIER && lastTk.type != TK_DEC_INT && lastTk.type != TK_OCTAL_INT && lastTk.type != TK_HEX_INT && lastTk.type != TK_FLOAT)) {
        smatch m;
        string s = str.substr(i);
        if (regex_search(s, m, floatExp)) {
            tk.setToken(TK_FLOAT, m[0]);
            i += tk.value.length();
            return i;
        } else if (regex_search(s, m, octalExp)) {
            tk.setToken(TK_OCTAL_INT, m[0]);
            i += tk.value.length();
            return i;
        } else if (regex_search(s, m, hexExp)) {
            tk.setToken(TK_HEX_INT, m[0]);
            i += tk.value.length();
            return i;
        } else if (regex_search(s, m, decExp)) {
            tk.setToken(TK_DEC_INT, m[0]);
            i += tk.value.length();
            return i;
        }
//    }
    }

    if (!isalpha(str.at(i)) && str.at(i) != '_') {
        //token is of types other than num and identifier
        switch (str.at(i)) {
            case '/': {
                if (i + 1 == str.length() || (str.at(i + 1) != '=' && str.at(i + 1) != '/' && str.at(i + 1) != '*')) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                } else if (str.at(i + 1) == '=') {
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }
                    //remove comment. '/' 出现在这里，只能是注释
                    // '//', '/* */'
                else if (str.at(i + 1) == '/') {
                    while (i != str.length() && str.at(i) != '\n') {
                        i++;
                    }
                    tk.type = TK_NOT_VALID;
                    return i;
                } else if (str.at(i + 1) == '*') {
                    i++;
                    while (i != str.length() && str.at(i) != '/') {
                        i++;
                    }
                    if (i != str.length() && str.at(i - 1) == '*' && i - startPos >= 3) {
                        tk.type = TK_NOT_VALID;
                        return i + 1;
                    } else {
                        cout << "In comment /*...*/, grammar error" << endl;
                        return -1;
                    }
                }
                break;
            }
            case '\'':
            case '"': {
                bool inBackslashChain = false;
                int backslashCount = 0;
                i++;
                //"...\"..."               "...\\"
                while (1) {
                    while (str.at(i) != str.at(startPos)) {
                        if (str.at(i) == '\\') {
                            inBackslashChain = true;
                            backslashCount++;
                        } else {
                            inBackslashChain = false;
                            backslashCount = 0;
                        }
                        i++;
                        if (i == str.length()) {
                            cout << "In string, quotation not match" << endl;
                            return -1;
                        }
                    }
                    if (backslashCount % 2 == 0) {
                        tk.setToken(TK_STRING, str.substr(startPos + 1, i - startPos));
                        return i + 1;
                        break;
                    }
                }
            }
            case '.':
            case ',':
            case ';':
            case ':':
            case '?':
            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case '~':
                tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                return i + 1;
                break;
            case '+':
            case '-':
            case '*':
            case '&':
            case '|':
            case '^':
                if (i + 1 == str.length() || (str.at(i + 1) != str.at(i) && str.at(i + 1) != '=')) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                } else {
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }
                break;
            case '%':
                if (i + 1 == str.length() || (str.at(i + 1) != '=')) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                } else {
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }
                break;
            case '<'://< <= << <<=
            case '>':
                if (i + 1 == str.length() || (str.at(i + 1) != str.at(i) && str.at(i + 1) != '=')) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                } else if (str.at(i + 1) == '=' ||
                           (str.at(i + 1) == str.at(i) && (i + 2 == str.length() || str.at(i + 2) != '='))) {
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                } else {
                    tk.setToken(tokenMap[str.substr(i, 3)], str.substr(i, 3));
                    return i + 3;
                }
                break;
            case '!': // ! != !==
            case '=': // = == ===
                if (i + 1 == str.length() || (str.at(i + 1) != '=')) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                } else if (i + 2 == str.length() || str.at(i + 2) != '=') {
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                } else {
                    tk.setToken(tokenMap[str.substr(i, 3)], str.substr(i, 3));
                    return i + 3;
                }
                break;

            default:
                cout << "No such token!" << endl;
                break;
        }

    } else {
        while (isalnum(str.at(i)) || str.at(i) == '_') {
            i++;
            if (i == str.length()) {
                break;
            }
        }

        tk.value = str.substr(startPos, i - startPos);
        if (tokenMap.find(tk.value) != tokenMap.end()) {
            tk.type = tokenMap[tk.value];
        } else {
            tk.type = TK_IDENTIFIER;
        }
        return i;
    }

    cout << "No such token" << endl;
    return -1;
}


void Lex::match(TOKEN_TYPES expected_token) {
//    while(token.type == TK_NOT_VALID)
    if (token.type != expected_token) {
        cout << "Got " << getTokenStr(token.type) << " expected " << getTokenStr(expected_token) << endl;
    }
    getNextToken();
}

string Lex::getTokenStr(TOKEN_TYPES tkType) {
    if (invTokenMap.find(tkType) != invTokenMap.end()) {
        return invTokenMap[tkType];
    }

    stringstream ss;
    ss << (int)tkType;
    return string("?[" + ss.str() + "]");
}

string Lex::getSubString() {
    int count=1;
    int prePos=posNow;
    int start=posNow-1;
    int len=1;


    while(count>0){
        this->getNextToken();
        if(token.value == "{"){
            count++;
        }
        if(token.value == "}"){
            count--;
        }
    };

    len=posNow-prePos;
    string result = originalStr.substr(start, len+1);
    this->match(TK_R_LARGE_BRACKET);

    return result;
}

Lex *Lex::getSubLex(int lastPosition) {
    if (lastPosition >= tokenLastEnd) {
        cout << "getSubLex error: lastPositin >= tokenLastEnd" << endl;
        return nullptr;
    }
    return new Lex(originalStr.substr(lastPosition - 1, tokenLastEnd - lastPosition + 2));
    
}

//string Lex::getPosition(int pos) {
//    return __1::basic_string<char, char_traits<char>, allocator<char>>();
//}

void Lex::initialTokenMap() {
    // value properties
    tokenMap["Infinity"] = TK_INFINITY;
    tokenMap["NaN"] = TK_NAN;
    tokenMap["undefined"] = TK_UNDEIFNED;
    tokenMap["null"] = TK_NULL;
    //fundamental objects
    tokenMap["Object"] = TK_OBJECT;
    //control flow
    tokenMap["break"] = TK_BREAK;
    tokenMap["continue"] = TK_CONTINUE;
    tokenMap["Empty"] = TK_EMPTY;
    tokenMap["if"] = TK_IF;
    tokenMap["else"] = TK_ELSE;
    tokenMap["switch"] = TK_SWITCH;
    tokenMap["do"] = TK_DO;
    tokenMap["while"] = TK_WHILE;
    tokenMap["for"] = TK_FOR;
    //declarations
    tokenMap["var"] = TK_VAR;
    tokenMap["let"] = TK_LET;
    tokenMap["const"] = TK_CONST;
    //functions and classes
    tokenMap["function"] = TK_FUNCTION;
    tokenMap["return"] = TK_RETURN;
    tokenMap["class"] = TK_CLASS;
    //expressions
    tokenMap["this"] = TK_THIS;
    tokenMap["new"] = TK_NEW;
    tokenMap["super"] = TK_SUPER;
    tokenMap["delete"] = TK_DELETE;
    tokenMap["typeof"] = TK_TYPEOF;
    
    //done
    tokenMap["++"] = TK_PLUS_PLUS;
    tokenMap["--"] = TK_MINUS_MINUS;
    tokenMap["+"] = TK_PLUS;
    tokenMap["-"] = TK_MINUS;
    tokenMap["~"] = TK_BITWISE_NOT;
    
    tokenMap["!"] = TK_NOT;
    tokenMap["*"] = TK_MULTIPLY;
    tokenMap["/"] = TK_DIVIDE;
    tokenMap["%"] = TK_MOD;
    
    tokenMap["**"] = TK_EXPONENT;
    tokenMap["<"] = TK_LESS;
    tokenMap[">"] = TK_GREATER;
    tokenMap["<="] = TK_L_EQUAL;
    tokenMap[">="] = TK_G_EQUAL;
    
    tokenMap["=="] = TK_EQUAL;
    tokenMap["!="] = TK_N_EQUAL;
    tokenMap["==="] = TK_TYPEEQUAL;
    tokenMap["!=="] = TK_N_TYPEEQUAL;
    tokenMap["<<"] = TK_L_SHIFT;
    tokenMap[">>"] = TK_R_SHIFT;
    tokenMap["&"] = TK_BITWISE_AND;
    tokenMap["|"] = TK_BITWISE_OR;
    tokenMap["^"] = TK_BITWISE_XOR;
    tokenMap["&&"] = TK_AND_AND;
    tokenMap["||"] = TK_OR_OR;
    tokenMap["="] = TK_ASSIGN;
    tokenMap["*="] = TK_MULTI_EQUAL;
    tokenMap["/="] = TK_DIV_EQUAL;
    tokenMap["%="] = TK_MOD_EQUAL;
    tokenMap["+="] = TK_PLUS_EQUAL;
    tokenMap["-="] = TK_MINUS_EQUAL;
    tokenMap["<<="] = TK_L_SHIFT_EQUAL;
    tokenMap[">>="] = TK_R_SHIFT_EQUAL;
    tokenMap["&="] = TK_AND_EQUAL;
    tokenMap["^="] = TK_XOR_EQUAL;
    tokenMap["|="] = TK_OR_EQUAL;
    
    tokenMap["true"] = TK_TRUE;
    tokenMap["false"] = TK_FALSE;
    //done
    tokenMap["["] = TK_L_SQUARE_BRACKET;
    tokenMap["]"] = TK_R_SQUARE_BRACKET;
    tokenMap["("] = TK_L_BRACKET;
    tokenMap[")"] = TK_R_BRACKET;
    tokenMap["{"] = TK_L_LARGE_BRACKET;
    tokenMap["}"] = TK_R_LARGE_BRACKET;
    tokenMap["."] = TK_DOT;
    tokenMap[";"] = TK_SEMICOLON;
    tokenMap[":"] = TK_COLON;
    tokenMap[","] = TK_COMMA;
    tokenMap["?"] = TK_QUESTION_MARK;
    
    // value properties
    invTokenMap[TK_EOF] = "eof";
    invTokenMap[TK_IDENTIFIER] = "ID";
    invTokenMap[TK_NOT_VALID] = "TK_NOT_VALID";
    invTokenMap[TK_DEC_INT] = "dec int";
    invTokenMap[TK_OCTAL_INT] = "oct int";
    invTokenMap[TK_HEX_INT] = "hex int";
    invTokenMap[TK_FLOAT] = "float";
    invTokenMap[TK_STRING] = "string";
    
    
    invTokenMap[TK_INFINITY] = "Infinity";
    invTokenMap[TK_NAN] = "NaN";
    invTokenMap[TK_UNDEIFNED] = "undefined";
    invTokenMap[TK_NULL] = "null";
    //fundamental objects
    invTokenMap[TK_OBJECT] = "Object";
    //control flow
    invTokenMap[TK_BREAK] = "break";
    invTokenMap[TK_CONTINUE] = "continue";
    invTokenMap[TK_EMPTY] = "Empty";
    invTokenMap[TK_IF] = "if";
    invTokenMap[TK_ELSE] = "else";
    invTokenMap[TK_SWITCH] = "switch";
    invTokenMap[TK_DO] = "do";
    invTokenMap[TK_WHILE] = "while";
    invTokenMap[TK_FOR] = "for";
    //declarations
    invTokenMap[TK_VAR] = "var";
    invTokenMap[TK_LET] = "let";
    invTokenMap[TK_CONST] = "const";
    //functions and classes
    invTokenMap[TK_FUNCTION] = "function";
    invTokenMap[TK_RETURN] = "return";
    invTokenMap[TK_CLASS] = "class";
    //expressions
    invTokenMap[TK_THIS] = "this";
    invTokenMap[TK_NEW] = "new";
    invTokenMap[TK_SUPER] = "super";
    invTokenMap[TK_DELETE] = "delete";
    invTokenMap[TK_TYPEOF] = "typeof";
    
    //done
    invTokenMap[TK_PLUS_PLUS] = "++";
    invTokenMap[TK_MINUS_MINUS] = "--";
    invTokenMap[TK_PLUS] = "+";
    invTokenMap[TK_MINUS] = "-";
    invTokenMap[TK_BITWISE_NOT] = "~";
    
    invTokenMap[TK_NOT] = "!";
    invTokenMap[TK_MULTIPLY] = "*";
    invTokenMap[TK_DIVIDE] = "/";
    invTokenMap[TK_MOD] = "%";
    
    invTokenMap[TK_EXPONENT] = "**";
    invTokenMap[TK_LESS] = "<";
    invTokenMap[TK_GREATER] = ">";
    invTokenMap[TK_L_EQUAL] = "<=";
    invTokenMap[TK_G_EQUAL] = ">=";
    
    invTokenMap[TK_EQUAL] = "==";
    invTokenMap[TK_N_EQUAL] = "!=";
    invTokenMap[TK_TYPEEQUAL] = "===";
    invTokenMap[TK_N_TYPEEQUAL] = "!==";
    invTokenMap[TK_L_SHIFT] = "<<";
    invTokenMap[TK_R_SHIFT] = ">>";
    invTokenMap[TK_BITWISE_AND] = "&";
    invTokenMap[TK_BITWISE_OR] = "|";
    invTokenMap[TK_BITWISE_XOR] = "^";
    invTokenMap[TK_AND_AND] = "&&";
    invTokenMap[TK_OR_OR] = "||";
    invTokenMap[TK_ASSIGN] = "=";
    invTokenMap[TK_MULTI_EQUAL] = "*=";
    invTokenMap[TK_DIV_EQUAL] = "/=";
    invTokenMap[TK_MOD_EQUAL] = "%=";
    invTokenMap[TK_PLUS_EQUAL] = "+=";
    invTokenMap[TK_MINUS_EQUAL] = "-=";
    invTokenMap[TK_L_SHIFT_EQUAL] = "<<=";
    invTokenMap[TK_R_SHIFT_EQUAL] = ">>=";
    invTokenMap[TK_AND_EQUAL] = "&=";
    invTokenMap[TK_XOR_EQUAL] = "^=";
    invTokenMap[TK_OR_EQUAL] = "|=";
    
    invTokenMap[TK_TRUE] = "true";
    invTokenMap[TK_FALSE] = "false";
    //done
    invTokenMap[TK_L_SQUARE_BRACKET] = "[";
    invTokenMap[TK_R_SQUARE_BRACKET] = "]";
    invTokenMap[TK_L_BRACKET] = "(";
    invTokenMap[TK_R_BRACKET] = ")";
    invTokenMap[TK_L_LARGE_BRACKET] = "{";
    invTokenMap[TK_R_LARGE_BRACKET] = "}";
    invTokenMap[TK_DOT] = ".";
    invTokenMap[TK_SEMICOLON] = ";";
    invTokenMap[TK_COLON] = ":";
    invTokenMap[TK_COMMA] = ",";
    invTokenMap[TK_QUESTION_MARK] = "?";

}

