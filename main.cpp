//
//  main.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//


#include <fstream>
#include "Lex.h"

#include "Interpreter.h"

using namespace std;

char buffer[1024];
string wholeText;

char endLineChar[2] = {'\n', '\0'};
string endline(endLineChar);


int main() {
    string file="./Test4JS/2.js";
    Interpreter interpreter(file);
    interpreter.execute();

    cout << interpreter.root->findChild("result")->var->getBool() << endl;
    return 0;
}
