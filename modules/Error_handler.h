//
// Created by rngou on 19.03.2021.
//

#ifndef LEXER_ERROR_HANDLER_H
#define LEXER_ERROR_HANDLER_H

#include <iostream>


enum Error {
    EOF_ERR,
    MISSING_ID,
    MISSING_SEMI,
    MISSING_COL,
    MISSING_TYPE,
    MISSING_EQU,
    MISSING_CONST,
    MISSING_END,
    MISSING_DOT,
    MISSING_VAR_DEF,
    MISSING_ASS,
    MISSING_COMMA,
    MISSING_LEFT_BRACKET,
    MISSING_RIGHT_BRACKET,
    MISSING_PROG_NAME,
    MISSING_OF,
    MISSING_CONSTANT,
    DUPLICATE,
    UNKNOWN
};

class Error_handler {
public:
    void handle(Error error);
};


#endif //LEXER_ERROR_HANDLER_H
