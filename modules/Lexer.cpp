//
// Created by rngou on 19.03.2021.
//

#include "Lexer.h"

char Lexer::getChar() {
    if (code.fail()) {
        std::cerr << "<E> Can't read from the file" << std::endl;
        throw std::runtime_error("File doesn't available");
    }

    if (!code.eof()) {
        code >> std::noskipws >> cursor;
    } else {
        std::cerr << "<E> File is EOF early" << std::endl;
        throw std::runtime_error("File is EOF early");
    }

    return cursor;
}

char Lexer::getCurrentCurs() {
    return cursor;
}

char Lexer::peekChar(int n) {
    try {
        char ch = -1;
        ch = code.peek();
        return ch;
    } catch (std::exception &exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what()
                  << std::endl;
        return -1;
    }
}

Lexem Lexer::getLexem() {
    try {
        auto ch = getCurrentCurs();

        while (ch == -1 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
            //update line number
            if(ch == '\n')
                line_number++;
            ch = getChar();

            if (code.eof()) return Lexem("EOF", eof_tk, line_number); // if end of file
        }

        auto isId = [](char ch) {
            return std::isalpha(static_cast<unsigned char>(ch)) ||
                   std::isdigit(static_cast<unsigned char>(ch));
        };


        std::string lex;
        if (std::isdigit(static_cast<unsigned char>(ch))) { // Constants (Numbers)
            while (std::isdigit(static_cast<unsigned char>(ch))) {
                lex += ch;
                ch = getChar();
            }

            return Lexem(lex, constant_tk, line_number);
        } else if (std::isalpha(static_cast<unsigned char>(ch))) { // Identificators
            while(isId(ch)) {
                lex += ch;
                ch = getChar();
            }


            if (lex == "program")      { return Lexem(lex, program_tk, line_number); }
            else if (lex == "label")   { return Lexem(lex, label_tk, line_number);   }
            else if (lex == "const")   { return Lexem(lex, const_tk, line_number);   }
            else if (lex == "var")     { return Lexem(lex, var_tk, line_number);     }
            else if (lex == "begin")   { return Lexem(lex, begin_tk, line_number);   }
            else if (lex == "integer") { return Lexem(lex, type_tk, line_number);    }
            else if (lex == "real")    { return Lexem(lex, type_tk, line_number);    }
            else if (lex == "case")    { return Lexem(lex, case_tk, line_number);    }
            else if (lex == "of")      { return Lexem(lex, of_tk, line_number);      }
            else if (lex == "if")      { return Lexem(lex, if_tk, line_number);      }
            else if (lex == "else")    { return Lexem(lex, else_tk, line_number);    }
            else if (lex == "end")     { return Lexem(lex, end_tk, line_number);     }
            else if (lex == "then")     { return Lexem(lex, then_tk, line_number);     }
            else if (lex == "goto")    { return Lexem(lex, goto_tk, line_number);    }
            else { // it is ID
                return Lexem(lex, id_tk, line_number);
            }
        } else if (std::ispunct(static_cast<unsigned char>(ch))) { // Other symbols
            tokens tok;
            switch (ch) {
                case ',' : tok = comma_tk; break;
                case '.' : tok = dot_tk;   break;
                case ':' : tok = ddt_tk;   break;
                case ';' : tok = semi_tk;  break;
                case '=' : tok = eqv_tk;   break;
                case '+' : tok = add_tk; break;
                case '-' : tok = sub_tk; break;
                case '*' : tok = mul_tk; break;
                case '/' : tok = div_tk; break;
                default:
                    std::cerr << "<E> Unknown token " << ch << std::endl; break;
            }
            lex += ch;

            if (tok == ddt_tk) {
                if (peekChar(1) == '=') {
                    ch = getChar();
                    lex += ch;
                    tok = ass_tk;
                }
            }

            getChar(); // some kind of k o s t y l; here we look on \n
            return Lexem(lex, tok, line_number);
        } else {
            std::cerr << "<E> Unknown token " << ch << std::endl;
        }

        return Lexem("", unknown_tk, line_number);
    } catch (const std::exception &exp) {
        return Lexem("", unknown_tk, line_number);
    }
}


Lexer::Lexer(const char *file_path) {
    try {
        code.open(file_path);
    }
    catch (const std::exception& exp) {
        std::string lel(exp.what());
        std::string what = "<E> Lexer: Catch exception in constructor:";
        throw std::runtime_error(what + lel);
    }
}

std::vector<Lexem> Lexer::parseCode() {
    try {
        if (!code.is_open()) {
            std::cerr << "<E> Can't open file" << std::endl;
            return lexem_table;
        }

        while (!code.eof()) {
            lexem_table.emplace_back(getLexem());
        }

        return lexem_table;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what() << std::endl;
        return lexem_table;
    }
}

Lexer::~Lexer() {
    code.close();
}

void Lexer::printLexem() {
    std::cout << "Lexem table: \n\n";
    for(int i = 0; i < lexem_table.size(); i++) {
        std::cout << lexem_table[i].getName() << " "
        << lexem_table[i].getToken() << std::endl;
    }
}
