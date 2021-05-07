//
// Created by rngou on 25.01.2021.
//

#ifndef LAB_02_LEXEMES_H
#define LAB_02_LEXEMES_H

enum tokens {
    unknown_tk = -1,    // we get unknown token
    program_tk = 0,     // 'program'
    var_tk,             // 'var'
    begin_tk,           // 'begin'
    end_tk,             // 'end'
    type_tk,            // 'type'
    id_tk       = 8,    // any [aA-zZ][0-9]
    constant_tk = 9,    // 0-9
    dot_tk,             // '.'
    comma_tk,           // ','
    ddt_tk,             // ':'
    semi_tk,            // ';'
    eqv_tk,             // '='
    ass_tk,             // ':='
    add_tk,             // '+'
    sub_tk,             // '-'
    mul_tk,             // '*'
    div_tk,             // '/'
    if_tk,              // 'if'
    else_tk,            // 'else'
    case_tk,            // 'case'
    of_tk,              // 'of'
    then_tk,            // 'then'
    goto_tk,            // 'goto'
    label_tk,           // 'label'
    const_tk,           // 'const'
    leftBracket_tk,     // ')'
    rightBracket_tk,    // ')'
    bool_eqv_tk,        // '=='
    bool_noneqv_tk,     // '<>'
    bool_bigger_tk,     // '>'
    bool_less_tk,       // '<'
    bool_bigeqv_tk,     // '>='
    bool_leseqv_tk,     // '<='
    comp_tk,            // 'comparison signs'
    eof_tk
};

class Lexem {
private:
    std::string name;
    tokens token;
    int lineNumber;
public:
    Lexem() = default;
    Lexem(std::string, tokens);
    Lexem(std::string, tokens, int);
    std::string getName();
    tokens getToken();
    int getLine();
};

#endif //LAB_02_LEXEMES_H
