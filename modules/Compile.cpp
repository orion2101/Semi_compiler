//
// Created by rngou on 19.03.2021.
//
#include "Lexer.h"
#include "Syntax_parser.h"
#include "Compile.h"

int Compile(const std::string& file_path) {

    Lexer lex(file_path.c_str());
    auto table = lex.parseCode(); //table of lexemes
    lex.printLexem();

    Syntax_parser syntax(std::move(table));
    auto tree = syntax.build(); // syntax tree

    if (tree == nullptr) {
        std::cerr << "Error: Invalid syntax tree" << std::endl;
        return -EXIT_FAILURE;
    }
//
//    GenCode gencod(std::move(*tree));
//    gencod.GenerateAsm(); // final code file

    return EXIT_SUCCESS;
}