//
//  main.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//


#include <fstream>
#include "Lex.h"

#include "TinyJS.h"

using namespace std;


    char buffer[1024];
    string wholeText;

    char endLineChar[2] = {'\n', '\0'};
    string endline(endLineChar);



//     for (int i = 1; i < 2; ++i) {
//         wholeText = "";
// //        if (i < 10) {
// //            filePath = path + "0" + to_string(i) + ".js";
// //        }else{
// //            filePath = path + "" + string(i) + ".js";
// //        }

//         cout << endl << i << endl;
//         ifstream input(filePath, std::ios::in);
//         ofstream output(path+"res.txt", std::ios::out);

//         while (input.getline(buffer, 1024)) {
//             string line(buffer);
//             wholeText += line + endline;
//         }

// //        cout<<wholeText<<endl;
// //        
//         Lex l(wholeText);
        
//         while (l.token.type!=TK_EOF) {
//             l.getNextToken();
//             cout << l.getTokenStr(l.token.type) << " " << l.token.value << endl;
//         }
//        l.getNextToken();
//        l.getNextToken();
//        l.match(TK_ASSIGN);
//        l.match(TK_DEC_INT);
//        l.match(TK_SEMICOLON);
//        l.match(TK_VAR);
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        l.match(TK_FOR);
//        l.getNextToken();
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        l.match(TK_DEC_INT);
//        l.getNextToken();
//        l.match(TK_IDENTIFIER);
//        l.match(TK_L_EQUAL);
//        l.match(TK_DEC_INT);
//        l.getNextToken();
//        l.match(TK_IDENTIFIER);
//        l.match(TK_PLUS_PLUS);
//        l.getNextToken();
//        int lastPosition = l.posNow + 1;
//        
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        l.getNextToken();
//        l.match(TK_PLUS);
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        Lex* subLex;
//        subLex = l.getSubLex(lastPosition);
//        subLex -> getNextToken();
//        cout << subLex->token.value;
//        while (l.token.type != TK_EOF) {
//            l.getNextToken();
//            cout << l.token.value << " ";
//        }
//        
//        l.reset();
//        l.getNextToken();
//        l.getNextToken();
//        l.match(TK_ASSIGN);
//        l.match(TK_DEC_INT);
//        l.match(TK_SEMICOLON);
//        l.match(TK_VAR);
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        l.match(TK_FOR);
//        l.getNextToken();
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        l.match(TK_DEC_INT);
//        l.getNextToken();
//        l.match(TK_IDENTIFIER);
//        l.match(TK_L_EQUAL);
//        l.match(TK_DEC_INT);
//        l.getNextToken();
//        l.match(TK_IDENTIFIER);
//        l.match(TK_PLUS_PLUS);
//        l.getNextToken();
//        lastPosition = l.posNow + 1;
//        
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        l.getNextToken();
//        l.match(TK_PLUS);
//        l.match(TK_IDENTIFIER);
//        l.getNextToken();
//        Lex* subLex2;
//        subLex2 = l.getSubLex(lastPosition);
//        subLex2 -> getNextToken();
//        cout << subLex2->token.value;

        
//        for (int j = 0; j < l.tokens.size(); j++) {
//            output << l.tokens[j].type;
//            output << "  ";
//        }
    // }

int main() {
    string file="./Test4JS/test1.js";
    TinyJS tinyJS(file);
//    tinyJS.root->addChild("result", new Var(0));
    tinyJS.execute();
    cout << tinyJS.root->findChild("a")->var->getInt() << endl;
//    cout << tinyJS.root->findChild("result")->var->getInt() << endl;
    return 0;
}
