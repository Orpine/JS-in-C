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


int main() {
    string file="D:\\GitHub\\JS-in-C\\Test4JS\\test1.js";
    TinyJS tinyJS(file);
//    tinyJS.root->addChild("result", new Var(0));
    tinyJS.execute();

    cout << tinyJS.root->findChild("result")->var->getInt() << endl;
    return 0;
}
