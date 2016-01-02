//
//  main.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//


#include <fstream>
#include "Lex.h"

//int main(int argc, const char * argv[]) {
//
//    string path = "E:\\CppWorkspace\\TinyJS\\Test4JS\\test1";
//    string filePath=path+".js";
//
//    char buffer[1024];
//    string wholeText;
//
//    char endLineChar[2] = {'\n', '\0'};
//    string endline(endLineChar);
//
//
//
//    for (int i = 1; i < 2; ++i) {
//        wholeText = "";
////        if (i < 10) {
////            filePath = path + "0" + to_string(i) + ".js";
////        }else{
////            filePath = path + "" + string(i) + ".js";
////        }
//
//        cout << endl << i << endl;
//        ifstream input(filePath, std::ios::in);
//        ofstream output(path+"res.txt", std::ios::out);
//
//        while (input.getline(buffer, 1024)) {
//            string line(buffer);
//            wholeText += line + endline;
//        }
//
//        cout<<wholeText<<endl;
//        Lex l(wholeText);
//        for (int j = 0; j < l.tokens.size(); j++) {
//            output << l.tokens[j].type;
//            output << "  ";
//        }
//    }
//    return 0;
//}

#include "TinyJS.h"

using namespace std;

int main() {
    string file="E:\\CppWorkspace\\TinyJS\\Test4JS\\test1.js";
    TinyJS tinyJS(file);
    tinyJS.run();

    return 0;
}