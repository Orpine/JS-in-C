//
//  Lex.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//

#include "Lex.hpp"
#include <regex>

//the tokens that can appear before numbers with '+/-' prefix
set<TOKEN_TYPES> tokenBeforePrefix{TK_L_BRACKET, TK_ASSIGN, TK_PLUS_EQUAL, TK_MINUS_EQUAL, TK_MULTI_EQUAL, TK_DIV_EQUAL, TK_MOD_EQUAL, TK_AND_EQUAL,
    TK_OR_EQUAL,
    TK_L_EQUAL,
    TK_G_EQUAL, };

int Lex::getNextToken(string& str, int startPos, Token& tk, Token& lastTk){
    
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
    }else if(str.at(i) == '+' || str.at(i) == '-'){
        if (tokenBeforePrefix.find(lastTk.type) != tokenBeforePrefix.end()) {
            maybeANum = true;
        }
    }
    
    if (maybeANum) {
        if ( (str.at(i) != '+' && str.at(i) != '-') || (lastTk.type != TK_START && lastTk.type != TK_IDENTIFIER && lastTk.type != TK_DEC_INT && lastTk.type != TK_OCTAL_INT && lastTk.type != TK_HEX_INT && lastTk.type != TK_FLOAT)) {
            smatch m;
            string s = str.substr(i);
            if (regex_search(s, m, floatExp)) {
                tk.setToken(TK_FLOAT, m[0]);
                i += tk.value.length();
                return i;
            }else if(regex_search(s, m, octalExp)){
                tk.setToken(TK_OCTAL_INT, m[0]);
                i += tk.value.length();
                return i;
            }else if (regex_search(s, m, hexExp)){
                tk.setToken(TK_HEX_INT, m[0]);
                i += tk.value.length();
                return i;
            }else if(regex_search(s, m, decExp)){
                tk.setToken(TK_DEC_INT, m[0]);
                i += tk.value.length();
                return i;
            }
        }
        
    }
    
    if(!isalpha(str.at(i)) && str.at(i) != '_'){
        //token is of types other than num and identifier
        switch (str.at(i)) {
            case '/':
            {
                if (i+1 == str.length() || (str.at(i+1) != '=' && str.at(i+1) != '/' && str.at(i+1) != '*') ) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                }else if(str.at(i+1) == '='){
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }
                //remove comment. '/' 出现在这里，只能是注释
                // '//', '/* */'
                else if (str.at(i+1) == '/') {
                    while (i != str.length() && str.at(i) != '\n') {
                        i++;
                    }
                    tk.type = TK_NOT_VALID;
                    return i;
                }else if(str.at(i+1) == '*'){
                    i++;
                    while(i != str.length() && str.at(i) != '/'){
                        i++;
                    }
                    if ( i != str.length() && str.at(i-1) == '*' && i - startPos >= 3) {
                        tk.type = TK_NOT_VALID;
                        return i+1;
                    }else{
                        cout << "In comment /*...*/, grammar error" << endl;
                        return -1;
                    }
                }
                break;
            }
            case '\'':
            case '"':
            {
                bool inBackslashChain = false;
                int backslashCount = 0;
                i++;
                //"...\"..."               "...\\"
                while(1){
                    while (str.at(i) != str.at(startPos) ) {
                        if (str.at(i) == '\\') {
                            inBackslashChain = true;
                            backslashCount++;
                        }else{
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
                        tk.setToken(TK_STRING, str.substr(startPos, i-startPos+1));
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
                if (i+1 == str.length() || (str.at(i+1) != str.at(i) && str.at(i+1) != '=') ) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                }else{
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }
                break;
            case '%':
                if (i+1 == str.length() || (str.at(i+1) != '=') ) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                }else{
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }
                break;
            case '<'://< <= << <<=
            case '>':
                if (i+1 == str.length() || (str.at(i+1) != str.at(i) && str.at(i+1) != '=') ) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                }else if( str.at(i+1) == '=' || (str.at(i+1) == str.at(i) && (i+2==str.length() || str.at(i+2) != '=')) ){
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }else{
                    tk.setToken(tokenMap[str.substr(i, 3)], str.substr(i, 3));
                    return i + 3;
                }
                break;
            case '!': // ! != !==
            case '=': // = == ===
                if (i+1 == str.length() || (str.at(i+1) != '=') ) {
                    tk.setToken(tokenMap[str.substr(i, 1)], str.substr(i, 1));
                    return i + 1;
                }else if( i+2 == str.length() || str.at(i+2) != '=' ){
                    tk.setToken(tokenMap[str.substr(i, 2)], str.substr(i, 2));
                    return i + 2;
                }else{
                    tk.setToken(tokenMap[str.substr(i, 3)], str.substr(i, 3));
                    return i + 3;
                }
                break;
                
            default:
                cout << "No such token!" << endl;
                break;
        }
        
    }else{
        while ( isalnum(str.at(i)) || str.at(i) == '_' ) {
            i++;
            if (i == str.length()) {
                break;
            }
        }
        
        tk.value = str.substr(startPos, i - startPos);
        if ( tokenMap.find(tk.value) != tokenMap.end() ) {
            tk.type = tokenMap[tk.value];
        }else{
            tk.type = TK_IDENTIFIER;
        }
        return i;
    }
    
    cout << "No such token" << endl;
    return -1;
}


void Lex::initialTokenMap(){
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
    tokenMap["}"] = TK_R_BRACKET;
    tokenMap["."] = TK_DOT;
    tokenMap[";"] = TK_SEMICOLON;
    tokenMap[":"] = TK_COLON;
    tokenMap[","] = TK_COMMA;
    tokenMap["?"] = TK_QUESTION_MARK;
}
