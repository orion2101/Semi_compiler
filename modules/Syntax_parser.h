//
// Created by rngou on 19.03.2021.
//

#ifndef LEXER_SYNTAX_PARSER_H
#define LEXER_SYNTAX_PARSER_H

#include <vector>
#include <map>
#include <list>
#include "Error_handler.h"
#include "Variable.h"
#include "Lexemes.h"
#include "Tree.h"

class Syntax_parser {
public:
    Syntax_parser(std::vector<Lexem>&&);
    Tree* build();
    ~Syntax_parser();
private:
    using lex_it = std::vector<Lexem>::iterator;
    lex_it							cursor;
    std::vector<Lexem>				lex_table;	// table of lexemes
    std::map<std::string, Variable> id_map;		// table of identifiers
    Tree							*root_tree;
    bool							error{ false };
    std::string						breakpoint;

    std::map<std::string, int> operations;

    lex_it		getNextLex(lex_it& iter);
    lex_it		getPrevLex(lex_it& iter);

    lex_it		peekLex(int N, lex_it t_iter);
    lex_it		peekPrevLex(int N, lex_it t_iter);

    int						programParse(lex_it &t_iter);
    int						blockParse(lex_it& t_iter);

    std::list<std::string>	vardParse(lex_it& t_iter);
    int						vardpParse(lex_it& t_iter, Tree *t_tree);

    std::list<std::string> constdParse(lex_it& t_iter);
    int                    constdpParse(lex_it& t_iter, Tree *t_tree);
    void                   buildConstTree(const std::list<std::string>& t_var_list, Tree* t_tree);
    void                   createConstTree(Tree* t_tree, Tree* t_donor_tree, int lvl);

    std::list <std::string> labeldParse(lex_it& t_iter);
    int                     labeldpParse(lex_it& t_iter, Tree *t_tree);
    void                    buildLabelTree(const std::list<std::string>& t_label_list, Tree* t_tree);
    void                    createLabelTree(Tree* t_tree, Tree* t_donor_tree, int lvl);

    int                     declParse(lex_it& t_iter, Tree *t_tree);

    Tree*					stateParse(lex_it& t_iter, int c_count);
    Tree*					compoundParse(lex_it& t_iter, int c_count);

    int						expressionParse(lex_it& t_iter, Tree* tree, int t_lvl);
    Tree					*simplExprParse(const lex_it& var_iter, lex_it& t_iter, Tree* tree, int t_lvl);
    Tree					*simplExprParse(Tree* var_tree, lex_it& t_iter, Tree* tree, int t_lvl);


    Error_handler handler;
    bool	checkLexem(const lex_it& t_iter, const tokens& t_tok);
    bool	isVarExist(const std::string& t_var_name);
    bool    isConstExist(const std::string& t_const_name);
    int     defConstType(lex_it& t_iter);
    bool    isLabelExist(const std::string& t_label_name);
    void	updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name);
    void	updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name, const std::pair<int, int>& range);

    Tree*   buildCaseTree(lex_it& t_iter, int c_count);
    void	buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree);
    void    buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree, Tree* array_tree);
    void	createVarTree(Tree* t_tree, Tree* t_donor_tree, int lvl);
    Tree*	createLowestOpTree(Tree* t_parent_tree, std::string value, int priority);
};


#endif //LEXER_SYNTAX_PARSER_H
