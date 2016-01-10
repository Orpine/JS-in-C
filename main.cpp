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


void lex_test(){
    string path = "/Users/laoreja/Downloads/tiny-js-master/tests/test";
    string filePath;
    
    for (int i = 35; i < 36; ++i) {
         wholeText = "";
         if (i < 10) {
             filePath = path + "00" + to_string(i) + ".js";
         }else{
             filePath = path + "0" + to_string(i) + ".js";
         }

         cout << endl << i << endl;
         ifstream input(filePath, std::ios::in);
         ofstream output(path+"res.txt", std::ios::out);

         while (input.getline(buffer, 1024)) {
             string line(buffer);
             wholeText += line + endline;
         }

//        cout<<wholeText<<endl;
        //
            
        Lex l(wholeText);

        while (l.token.type!=TK_EOF) {
        l.getNextToken();
        cout << l.getTokenStr(l.token.type) << " " << l.token.value << endl;
        }
    }
}

int main() {
    string file="./Test4JS/test1.js";
    TinyJS tinyJS(file);
//    tinyJS.root->addChild("result", new Var(0));
    tinyJS.execute();

    cout << tinyJS.root->findChild("result")->var->getInt() << endl;
    return 0;
}
