//
// Created by rngou on 19.03.2021.
//

#include "Error_handler.h"

void Error_handler::handle(Error error) {
    switch(error) {
        case MISSING_COL:
            std::cerr << "Here must be ':'" << std::endl;
            break;
        case MISSING_SEMI:
            std::cerr << "Here must be ';'" << std::endl;
            break;
        case MISSING_ID:
            std::cerr << "Here must be identificator" << std::endl;
            break;
        case MISSING_TYPE:
            std::cerr << "Here must be type" << std::endl;
            break;
        case MISSING_EQU:
            std::cerr << "Here must be '='" << std::endl;
            break;
        case MISSING_CONST:
            std::cerr << "Here must be a constant" << std::endl;
            break;
        case MISSING_END:
            std::cerr << "Each 'begin' must be close by a 'end'" << std::endl;
            break;
        case MISSING_DOT:
            std::cerr << "Here must be '.'" << std::endl;
            break;
        case MISSING_VAR_DEF:
            std::cerr << "Variable not defined" << std::endl;
            break;
        case MISSING_ASS:
            std::cerr << "Here must be ':='" << std::endl;
            break;
        case MISSING_COMMA:
            std::cerr << "Here must be ','" << std::endl;
            break;
        case DUPLICATE:
            std::cerr << "This item was already defined" << std::endl;
            break;
        case MISSING_LEFT_BRACKET:
            std::cerr << "Here must be '('" << std::endl;
            break;
        case MISSING_RIGHT_BRACKET:
            std::cerr << "Here must be ')'" << std::endl;
            break;
        case MISSING_PROG_NAME:
            std::cerr << "Here must be program name" << std::endl;
            break;
        case EOF_ERR:
            std::cerr << "Program is end of file early" << std::endl;
            break;
        case MISSING_OF:
            std::cerr << "Here must be 'of'" << std::endl;
            break;
        case MISSING_CONSTANT:
            std::cerr << "Here must be '0-9'" << std::endl;
            break;
        default:
            std::cerr << "Unknown error" << std::endl;
            break;
    }
}
