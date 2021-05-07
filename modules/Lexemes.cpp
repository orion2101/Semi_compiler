//
// Created by rngou on 25.01.2021.
//

#include <iostream>
#include "Lexemes.h"

std::string Lexem::getName() {
    return name;
}

tokens Lexem::getToken() {
    return token;
}

int Lexem::getLine() {
    return lineNumber;
}

Lexem::Lexem(std::string value, tokens token) {
    this->name = value;
    this->token = token;
}

Lexem::Lexem(std::string value, tokens token, int lineNumber) {
    this->name = value;
    this->token = token;
    this->lineNumber = lineNumber;
}