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

int main() {
    string file="./Test4JS/test1.js";
    TinyJS tinyJS(file);
    tinyJS.root->addChild("result", new Var(0));
    tinyJS.execute();
    cout << tinyJS.root->findChild("result")->var->getInt() << endl;
    return 0;
}
