//
// Created by rngou on 19.03.2021.
//

#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H

#include <iostream>
#include <fstream>
#include "Lexemes.h"
#include <vector>

class Lexer {
private:
    std::ifstream code;
    std::vector<Lexem> lexem_table;

    Lexem getLexem();

    int line_number = 1;
    char cursor { -1 };         // cursor of stream

    char getChar();            // get next character in stream
    char peekChar(int n);

    char getCurrentCurs();

public:
    explicit Lexer(const char* file_path);
    ~Lexer();

    std::vector<Lexem> parseCode();

    void printLexem();
};


#endif //LEXER_LEXER_H
