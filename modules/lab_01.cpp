//
// Created by vldmr on 13.09.19.
//
#include "lab_01.h"
#include <vector>
#include "Variable.h"
#include "Lexemes.h"
#include "Error_handler.h"


/**
 * Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <your_other_operations>
 * <assign>      ::= <id> := <exp> ;
 * <exp>         ::= <id> | <constant>
 * <type>        ::= integer
 * <id>          ::= a-z
 * <constant>    ::= 0-9
 */

struct synt_t {
    Error_handler handler;

    std::vector<Lexem> lexems; //table of lexemeS
    int tableCount = 0; //Our position when reading lexem table
    int LexemTableSize();
    void PrintLexemTable();

    std::vector<Variable> variables; //var table
    bool varExists(std::string varName);
    void setVarType(std::string varType);

    void addId(Lexem id); // Adding Id to id list
    Lexem getId(int index); // We will need this to check id existence
    int idCount = 0; // size of id list

    std::ifstream code;         // input stream of Pascal code

    char  GetCurrentCurs();     // get current character in stream
    Lexem GetLexem();             // get next lexem
    Lexem GetLex(); //get next lexem from lexem table

    char  PeekChar(int n);      // peek on N symbols in stream
    Lexem PeekLex(int n);       // peek N lexem;
private:
    int lineNumber = 1; //location of a lexem in the stream, is use to notify about error location
    std::vector<Lexem> idList;  // List of identificators

    char cursor { -1 };         // cursor of stream

    char getChar();            // get next character in stream

};

void synt_t::setVarType(std::string varType) {
    for(int i = 0; i < variables.size(); i++)
    {
        variables[i].setType(varType);
    }
}

bool synt_t::varExists(std::string varName) {
    for(int i = 0; i < variables.size(); i++)
    {
        if(variables[i].getName() == varName)
        {
            return true;
        }
    }
    return false;
}

int synt_t::LexemTableSize() {
    return lexems.size();
}
Lexem synt_t::PeekLex(int n) {
    return lexems[tableCount+n-1];
}

Lexem synt_t::GetLex() {
    return lexems[tableCount++];
}

void synt_t::PrintLexemTable() {
    std::cout << "Lexem table content:\n";
    for(int i = 0; i < lexems.size(); i++)
    {
        std::cout << lexems[i].getName() << " line:" << lexems[i].getLine() << std::endl;
    }
    std::cout << "It's all !!\n";
}


void synt_t::addId(Lexem id)
{
    if(id.getToken() == id_tk)
    {
        variables.push_back(Variable(id.getName()));
    }
    idList.push_back(id);
    idCount++;
}

Lexem synt_t::getId(int index)
{
    return idList[index];
}

/**
 * @brief Get next character (symbol)
 * @param none
 *
 * @return current character
 * @note If stream is broken (eof or something), throw exception
 */
char synt_t::getChar() {
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


/**
 * @brief Peek to forward in stream on @n symbols
 * @param[in] n - the number of symbols for peek
 *
 * @return symbol from stream
 * @return -1 - can't peek forward
 * @note Peek only forward
 */
char synt_t::PeekChar(int n) {
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


/**
 * @brief Get current value of cursor
 * @param none
 *
 * @return value of cursor
 */
char synt_t::GetCurrentCurs() {
    return cursor;
}


/**
 * @brief Get next lexem
 * @param none
 *
 * @return lexem value (token)
 */
Lexem synt_t::GetLexem() {
    try {
        auto ch = GetCurrentCurs();
        //update line number
        if(ch == '\n')
            lineNumber++;

        while (ch == -1 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
            ch = getChar();
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

            return Lexem(lex, constant_tk, lineNumber);
        } else if (std::isalpha(static_cast<unsigned char>(ch))) { // Identificators
            while(isId(ch)) {
                lex += ch;
                ch = getChar();
            }


            if (lex == "program")      { return Lexem(lex, program_tk, lineNumber); }
            else if (lex == "label")   { return Lexem(lex, label_tk, lineNumber);   }
            else if (lex == "const")   { return Lexem(lex, const_tk, lineNumber);   }
            else if (lex == "var")     { return Lexem(lex, var_tk, lineNumber);     }
            else if (lex == "begin")   { return Lexem(lex, begin_tk, lineNumber);   }
            else if (lex == "integer") { return Lexem(lex, type_tk, lineNumber);    }
            else if (lex == "real")    { return Lexem(lex, type_tk, lineNumber);    }
            else if (lex == "case")    { return Lexem(lex, case_tk, lineNumber);    }
            else if (lex == "of")      { return Lexem(lex, of_tk, lineNumber);      }
            else if (lex == "if")      { return Lexem(lex, if_tk, lineNumber);      }
            else if (lex == "else")    { return Lexem(lex, else_tk, lineNumber);    }
            else if (lex == "end")     { return Lexem(lex, end_tk, lineNumber);     }
            else if (lex == "then")     { return Lexem(lex, then_tk, lineNumber);     }
            else if (lex == "goto")    { return Lexem(lex, goto_tk, lineNumber);    }
            else { // it is ID
                return Lexem(lex, id_tk, lineNumber);
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
                if (PeekChar(1) == '=') {
                    ch = getChar();
                    lex += ch;
                    tok = ass_tk;
                }
            }

            getChar(); // some kind of k o s t y l; here we look on \n
            return Lexem(lex, tok, lineNumber);
        } else {
            std::cerr << "<E> Unknown token " << ch << std::endl;
        }

        return Lexem("", unknown_tk, lineNumber);
    } catch (const std::exception &exp) {
        return Lexem("", unknown_tk, lineNumber);
    }
}


/**
 * @brief Peek to forward on the N lexem
 * @param[in] n - the number of skipped tokens
 *
 * @return N-th lexem (token)
 * @note Return cursor of stream to previous position
 */
/*lexem synt_t::PeekLex(int n) {
    int curr_pos   = code.tellg(); // get current position in stream
    auto curr_curs = GetCurrentCurs();
    try {
        lexem res;
        for (int i = 0; i < n; i++) {
            res = GetLex();
        }
        code.seekg(curr_pos, code.beg);
        cursor = curr_curs;

        return res;
    } catch (const std::exception &exp) {
        std::cerr << "<E> You try to peek too much forward, get back" << std::endl;
        code.seekg(curr_pos, code.beg);
        cursor = curr_curs;

        return lexem("", unknown_tk);
    }
}*/

/********** prototypes of functions ***************/
void  buildTreeStub  (Lexem lex);
int   expressionParse(Lexem lex, synt_t &parser);
int   stateParse     (Lexem &lex, synt_t &parser);
int   compoundParse  (Lexem lex, synt_t &parser);
Lexem vardParse      (Lexem lex, synt_t &parser);
Lexem constParse     (Lexem lex, synt_t &parser);
Lexem labelParse     (Lexem lex, synt_t &parser);
int   blockParse     (Lexem lex, synt_t &parser);
int   programParse   (synt_t &parser);
/*************************************************/


/**
 * @brief Stub for building tree
 * @param[in] lex - current lexem
 *
 * @return none
 * @note: in future here you will be building tree node
 */
void buildTreeStub(Lexem lex) {
    /*std::cout << "<D> stub, get lexem " << lex.first << " (" << lex.second << ")"
              << std::endl;*/
}


/**
 * @brief Parse expression part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if expression part is matched to grammar
 * @return -EXIT_FAILURE - if expression part doesn't matched to grammar
 */
int expressionParse(Lexem lex, synt_t &parser) {
    lex = parser.GetLex();
    switch(lex.getToken()) {
        case id_tk: {

        }
        case constant_tk: {
            /**
             * TODO: Here you have to check existence of operations (+/-/etc)
             */
            buildTreeStub(lex); // Here is your future Tree realization

            tokens op_token = parser.PeekLex(1).getToken();

            if (op_token == add_tk || op_token == sub_tk)
            {
                Lexem operation = Lexem("", unknown_tk);
                while(parser.PeekLex(1).getToken() != semi_tk)
                {
                    operation = parser.GetLex();
                    buildTreeStub(operation);
                }
            }

            break;
        }
        default: {
            std::cerr << "Line " << lex.getLine() << ": ";
            throw MISSING_ID;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse state part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if state part is matched to grammar
 * @return -EXIT_FAILURE - if state part doesn't matched to grammar
 */
int stateParse(Lexem &lex, synt_t &parser) {
    lex = parser.GetLex();
    switch(lex.getToken()) {
        case id_tk: { // a := b (assign part)
            /**
             * TODO: Here you have to check existence of variable
             */

             if(!(parser.varExists(lex.getName())))
             {
                 std::cerr << lex.getName() << " ";
                 throw MISSING_VAR_DEF;
             }

            lex = parser.GetLex();
            if (lex.getToken() != ass_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                throw MISSING_ASS;
            }

            try{
                expressionParse(lex, parser);
            } catch (Error& e) {
                parser.handler.handle(e);
            }

            lex = parser.GetLex();
            if (lex.getToken() != semi_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                throw MISSING_SEMI;
            }
            break;
        }
        case begin_tk: { // compound statements
            compoundParse(lex, parser);
            lex = parser.GetLex();
            if (lex.getToken() != semi_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                throw MISSING_SEMI;
            }
            break;
        }
        //TODO: add case goto_tk
        default: {

            break;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse compound part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if compound part is matched to grammar
 * @return -EXIT_FAILURE - if compound part doesn't matched to grammar
 */
int compoundParse(Lexem lex, synt_t &parser) {
    static int compound_count = 0;
    compound_count++;
    while (lex.getToken() != end_tk) {
        buildTreeStub(lex); // Here is your Tree realization
        if (parser.tableCount == (parser.lexems.size())) {
            throw MISSING_END;
        }

        try{
            stateParse(lex, parser);
        } catch (Error& e)
        {
            parser.handler.handle(e);
        }
    }

    if (compound_count == 1) {
        if (parser.PeekLex(1).getToken() == unknown_tk) {
            std::cerr << "Line " << parser.PeekLex(1).getLine() << ": ";
            throw MISSING_DOT;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Parse variable declaration part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return lexem - last parsed lexem (will actually return ':')
 */
Lexem vardParse(Lexem lex, synt_t &parser) {
    lex = parser.GetLex();
    if (lex.getToken() != id_tk) {
        std::cerr << "Line " << lex.getLine() << ": ";
        throw MISSING_ID;
    }

    /**
     *  TODO: Here save list of identificators
     */
    parser.addId(lex); // saving identificators

    lex = parser.GetLex();
    if (lex.getToken() == comma_tk)
        return vardParse(lex, parser); // Раскручивая стек обратно,
                                       // будет возвращено последнее значение

    return lex;
}


/**
 * @brief Parse block part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if block part is matched to grammar
 * @return -EXIT_FAILURE - if block part doesn't matched to grammar
 * @return 1 - we get last lexem ('.'), end of parse
 */
int blockParse(Lexem lex, synt_t &parser) {
    lex = parser.GetLex();
    switch(lex.getToken()) { // var / begin
        case var_tk: {   // var a, b: integer;
            try {
                lex = vardParse(lex, parser);
            } catch (Error &e) {
                parser.handler.handle(e);
            }

            if (lex.getToken() != ddt_tk)
            {
                std::cerr << "Line " << lex.getLine() << ": ";
                parser.handler.handle(MISSING_COL);
            }

            lex = parser.GetLex();
            if (lex.getToken() != type_tk)
            {
                std::cerr << "Line " << lex.getLine() << ": ";
                parser.handler.handle(MISSING_TYPE);
            }

            /*setting the type of the previous saved variables*/
            for(int i = 0; i < parser.variables.size(); i++)
            {
                //TODO: добавить проверка присуствия поля тип переменной
                parser.setVarType(lex.getName());
            }

            lex = parser.GetLex();
            if (lex.getToken() != semi_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                parser.handler.handle(MISSING_SEMI);
                return -EXIT_FAILURE;
            }

            buildTreeStub(lex); // Here is your Tree realization

            break;
        }
        case begin_tk: {
            try {
                compoundParse(lex, parser);
            } catch (Error& e)
            {
                parser.handler.handle(e);
            }
            break;
        }
        case dot_tk: {
            std::cout << "Program was parse successfully" << std::endl;
            return 1;
        }
        case const_tk: {
            try{
                lex = constParse(lex, parser);
            } catch (Error &e) {
                parser.handler.handle(e);
            }

            if (lex.getToken() != semi_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                parser.handler.handle(MISSING_SEMI);
                return -EXIT_FAILURE;
            }

            break;
        }
        case label_tk: {
            try{
                lex = labelParse(lex, parser);
            } catch (Error &e) {
                parser.handler.handle(e);
            }

            if (lex.getToken() != semi_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                parser.handler.handle(MISSING_SEMI);
                return -EXIT_FAILURE;
            }

            break;
        }
        default: {
            std::cerr << "Line " << lex.getLine() << ": ";
            parser.handler.handle(UNKNOWN);
            return -EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Entry point in grammar
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if input program part is correct
 * @return -EXIT_FAILURE - if input program part is incorrect
 * @note Wait input like: program <id_tk> ;
 */
int programParse(synt_t &parser) {
    auto lex = parser.GetLex();
    if (lex.getToken() == program_tk) {
        lex = parser.GetLex();
        if (lex.getToken() != id_tk) {
            std::cerr << "Line " << lex.getLine() << ": ";
            parser.handler.handle(MISSING_ID);
            if (lex.getToken() != semi_tk) {
                std::cerr << "Line " << lex.getLine() << ": ";
                throw MISSING_SEMI;
            }
        }

        lex = parser.GetLex();
        if (lex.getToken() != semi_tk) {
            std::cerr << "Line " << lex.getLine() << ": ";
            throw MISSING_SEMI;
        }

        return EXIT_SUCCESS;
    } else {
        std::cerr << "Line " << lex.getLine() << ": ";
        throw MISSING_SEMI;
    }
}

Lexem labelParse(Lexem lex, synt_t &parser) {
    lex = parser.GetLex();
    if (lex.getToken() != id_tk)
    {
        std::cerr << "Line " << lex.getLine() << ": ";
        throw MISSING_ID;
    }

    parser.addId(lex);

    lex = parser.GetLex();
    if (lex.getToken() == comma_tk)
        return labelParse(lex, parser);

    return lex;
}

Lexem constParse(Lexem lex, synt_t &parser) {
    lex = parser.GetLex();
    if (lex.getToken() != id_tk)
    {
        std::cerr << "Line " << lex.getLine() << ": ";
        throw MISSING_ID;
    }

    parser.addId(lex);

    lex = parser.GetLex();
    if(lex.getToken() != eqv_tk)
    {
        std::cerr << "Line " << lex.getLine() << ": ";
        throw MISSING_EQU;
    }

    lex = parser.GetLex();
    if(lex.getToken() != constant_tk)
    {
        std::cerr << "Line " << lex.getLine() << ": ";
        throw MISSING_CONST;
    }

    lex = parser.GetLex();
    return lex;
}


/**
 * @brief Start parse incoming pascal file
 * @param[in] file_path - path to a pascal file (code.p)
 *
 * @return  EXIT_SUCCESS - if file was successful parsed
 * @return -EXIT_FAILURE - if can't parse incoming file
 */
int Parse2(const std::string &file_path) {
    try {
        synt_t example_synt;

        example_synt.code.open(file_path);
        if (!example_synt.code.is_open()) {
            std::cerr << "<E> Can't open file" << std::endl;
            return -EXIT_FAILURE;
        }

       // example_synt.code.seekg(0, example_synt.code.beg);

        //Filling up lexem table
        while (!example_synt.code.eof() && !example_synt.code.fail())
        {
            example_synt.lexems.push_back(example_synt.GetLexem());
        }

        example_synt.PrintLexemTable();
        example_synt.code.close();
        if (programParse(example_synt) != 0) {
            return -EXIT_FAILURE;
        }

        Lexem lex;
        try{
            while (example_synt.tableCount <= example_synt.LexemTableSize()) {
                if (blockParse(lex, example_synt) == 1)
                    break;
            }
        } catch (Error &e) {
            example_synt.handler.handle(e);
        }


        for(int i = 0; i < example_synt.variables.size(); i++)
        {
            std::cout << example_synt.variables[i].getName() << " " << example_synt.variables[i].getType() << std::endl;
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what()
                  << std::endl;
        return -EXIT_FAILURE;
    }
}


/**
 * What you have to do:
 *  - rewrite synt structure (set more encapsulated);
 *  - add struct for work with variables;
 *  - add variable check (check of existen);
 *  - add + and - operations (with variadic number of values);
 *  - add function of error information (must pass the number line of code
 *      with error, type of error);
 *  - rewrite work with table of lexemes (get lexemes directly from table);
 *  - implement all TODO tips (start with 'TODO: Here you have to ...')
 */

