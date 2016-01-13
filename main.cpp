#include <fstream>
#include "Lex.h"

#include "Interpreter.h"

using namespace std;


int main() {
//    string file="./Test4JS/var.js";
//    string file="./Test4JS/json.js";
//    string file="./Test4JS/closure.js";
//    string file="./Test4JS/array.js";
//    string file="./Test4JS/recursion.js";
//    string file="./Test4JS/this_and_new.js";
    string file="./Test4JS/eval.js";

    Interpreter interpreter(file);
    interpreter.execute();

    cout << interpreter.root->findChild("result")->var->getString() << endl;
    return 0;
}

