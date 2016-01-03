//
//  main.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//


#include <fstream>
#include "Lex.h"

int main(int argc, const char * argv[]) {

    string path = "/Users/laoreja/Downloads/tiny-js-master/tests/test003";
    string filePath=path+".js";

    char buffer[1024];
    string wholeText;

    char endLineChar[2] = {'\n', '\0'};
    string endline(endLineChar);



    for (int i = 1; i < 2; ++i) {
        wholeText = "";
//        if (i < 10) {
//            filePath = path + "0" + to_string(i) + ".js";
//        }else{
//            filePath = path + "" + string(i) + ".js";
//        }

        cout << endl << i << endl;
        ifstream input(filePath, std::ios::in);
        ofstream output(path+"res.txt", std::ios::out);

        while (input.getline(buffer, 1024)) {
            string line(buffer);
            wholeText += line + endline;
        }

        cout<<wholeText<<endl;
        
        Lex l(wholeText);
        l.getNextToken();
        l.getNextToken();
        l.match(TK_ASSIGN);
        l.match(TK_DEC_INT);
        l.match(TK_SEMICOLON);
        l.match(TK_VAR);
        l.match(TK_IDENTIFIER);
        l.getNextToken();
        l.match(TK_FOR);
        l.getNextToken();
        l.match(TK_IDENTIFIER);
        l.getNextToken();
        l.match(TK_DEC_INT);
        l.getNextToken();
        l.match(TK_IDENTIFIER);
        l.match(TK_L_EQUAL);
        l.match(TK_DEC_INT);
        l.getNextToken();
        l.match(TK_IDENTIFIER);
        l.match(TK_PLUS_PLUS);
        l.getNextToken();
        int lastPosition = l.posNow + 1;
        
        l.match(TK_IDENTIFIER);
        l.getNextToken();
        l.getNextToken();
        l.match(TK_PLUS);
        l.match(TK_IDENTIFIER);
        l.getNextToken();
        Lex* subLex;
        subLex = l.getSubLex(lastPosition);
        subLex -> getNextToken();
        cout << subLex->token.value;
        
//        for (int j = 0; j < l.tokens.size(); j++) {
//            output << l.tokens[j].type;
//            output << "  ";
//        }
    }
    return 0;
}

//#include "TinyJS.h"
//
//using namespace std;
//
//int main() {
//    string file="./Test4JS/test1.js";
//    TinyJS tinyJS(file);
//    tinyJS.run();
//
//    return 0;
//}
