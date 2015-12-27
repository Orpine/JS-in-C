//
//  main.cpp
//  js_in_c_lexer
//
//  Created by 顾秀烨 on 12/27/15.
//  Copyright © 2015 laoreja. All rights reserved.
//


#include <fstream>
#include "Lex.hpp"

int main(int argc, const char * argv[]) {
    
    string path = "/Users/laoreja/Downloads/tiny-js-master/tests/test0";
    string filePath;
    
    char buffer[1024];
    string wholeText;
    
    char endLineChar[2] = {'\n', '\0'};
    string endline(endLineChar);
    
    
    
    for (int i = 1; i < 37; ++i) {
        wholeText = "";
        if (i < 10) {
            filePath = path + "0" + to_string(i) + ".js";
        }else{
            filePath = path + "" + to_string(i) + ".js";
        }
        
        cout << endl << i << endl;
        ifstream input(filePath, std::ios::in);
        ofstream output(path+to_string(i)+"res.js", std::ios::out);
        
        while (input.getline(buffer, 1024)) {
            string line(buffer);
            wholeText += line + endline;
        }

        Lex l(wholeText);
        for (int j = 0; j < l.tokens.size(); j++) {
            output << l.tokens[j].value;
            output << "  ";
        }
    }
    return 0;
}
