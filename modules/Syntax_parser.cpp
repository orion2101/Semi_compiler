//
// Created by rngou on 19.03.2021.
//

#include "Syntax_parser.h"

Syntax_parser::Syntax_parser(std::vector<Lexem> && table) {
    if (table.empty())
        throw std::runtime_error("<E> Syntax: Lexemes table is empty");
    /*if (table.at(0).getToken() == eof_tk)
        throw std::runtime_error("<E> Syntax: Code file is empty");*/
    lex_table = table;
    cursor = lex_table.begin();

    operations.emplace(":=", 0);

    operations.emplace("=", 1);
    operations.emplace("<>", 1);
    operations.emplace("<", 1);
    operations.emplace(">", 1);
    operations.emplace("<=", 1);
    operations.emplace(">=", 1);

    operations.emplace("+", 2);
    operations.emplace("-", 2);
    operations.emplace("or", 2);
    operations.emplace("xor", 2);

    operations.emplace("*", 3);
    operations.emplace("div", 3);

    operations.emplace("(", 4);
    operations.emplace(")", 4);

    operations.emplace("goto", 5);
}

Syntax_parser::~Syntax_parser() {
    Tree::FreeTree(root_tree);
}

Tree *Syntax_parser::build() {
    std::cout << "Code contains " << lex_table.size() << " lexemes" << std::endl;
    auto& it = cursor;
    if (programParse(it) != 0)
        return nullptr;

    while (it != lex_table.end() && it->getToken() != eof_tk)
        blockParse(it);

    if (!error) {
        std::cout << std::endl;
        std::cout << std::setfill('*') << std::setw(50);
        std::cout << "\r\n";

        root_tree->PrintTree(0);
    }

    std::cout << "EOF" << std::endl;

    return root_tree;
}

Syntax_parser::lex_it Syntax_parser::getNextLex(lex_it& iter) {
    try {
        if (iter != lex_table.end())
            iter++;
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }

    return iter;
}

Syntax_parser::lex_it Syntax_parser::getPrevLex(lex_it& iter) {
    try {
        if (iter != lex_table.begin())
            iter--;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }

    return iter;
}

Syntax_parser::lex_it Syntax_parser::peekLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;
        while (iter != lex_table.end()) {
            if (N == 0) return iter;
            iter++; N--;
        }

        return iter;
    }
    catch (const std::exception &) {
        std::cerr << "<E> Syntax: Can't peek so forward" << std::endl;
        return t_iter;
    }
}

Syntax_parser::lex_it Syntax_parser::peekPrevLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;
        while (iter != lex_table.end()) {
            if (N == 0) return iter;
            iter--; N--;
        }

        return iter;
    }
    catch (const std::exception&) {
        std::cerr << "<E> Syntax: Can't peek so backward" << std::endl;
        return t_iter;
    }
}

int Syntax_parser::programParse(lex_it &t_iter) {
    if (!checkLexem(t_iter, program_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_PROG_NAME);
        return -EXIT_FAILURE;
    }

    auto iter = getNextLex(t_iter);

    if (!checkLexem(iter, id_tk)) {
        if (iter->getToken() == eof_tk) {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(EOF_ERR);
            return -EXIT_FAILURE;
        }
        else {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_ID);
            return -EXIT_FAILURE;
        }
    }

    auto root_name = iter->getName(); // save the name of program

    iter = getNextLex(t_iter);

    if (!checkLexem(iter, semi_tk)) {
        if (iter->getToken() == eof_tk) {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(EOF_ERR);
            return -EXIT_FAILURE;
        }
        else {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_SEMI);
            return -EXIT_FAILURE;
        }
    }
    root_tree = Tree::CreateNode(root_name);

    return EXIT_SUCCESS;
}

int Syntax_parser::blockParse(lex_it& t_iter) {
    try {
        auto iter = getNextLex(t_iter);
        switch (iter->getToken()) {
            case label_tk: {
                if(root_tree->GetLeftNode() != nullptr) {
                    if(root_tree->GetLeftNode()->GetValue() == "var" || root_tree->GetLeftNode()->GetValue() == "const")
                    {
                        Tree* leftTree = root_tree->GetLeftNode();
                        while(leftTree->GetLeftNode() != nullptr)
                        {
                            leftTree = leftTree->GetLeftNode();
                        }
                        leftTree->AddLeftNode("label");
                    }
                }
                else root_tree->AddLeftNode("label");

                Tree* label_node = root_tree;
                while(label_node->GetValue() != "label")
                    label_node = label_node->GetLeftNode();
                labeldpParse(t_iter, label_node);
                break;
            }
            case const_tk: {
                if(root_tree->GetLeftNode() != nullptr){
                    if(root_tree->GetLeftNode()->GetValue() == "var" || root_tree->GetLeftNode()->GetValue() == "label") {
                        Tree *leftTree = root_tree->GetLeftNode();
                        while (leftTree->GetLeftNode() != nullptr) {
                            leftTree = leftTree->GetLeftNode();
                        }
                        leftTree->AddLeftNode("const");
                    }
                } else root_tree->AddLeftNode("const");

                Tree* const_node = root_tree;
                while(const_node->GetValue() != "const")
                    const_node = const_node->GetLeftNode();
                constdpParse(t_iter, const_node);
                break;
            }
            case var_tk: {
                if(root_tree->GetLeftNode() != nullptr){
                    if(root_tree->GetLeftNode()->GetValue() == "label" || root_tree->GetLeftNode()->GetValue() == "const") {
                        Tree *leftTree = root_tree->GetLeftNode();
                        while (leftTree->GetLeftNode() != nullptr) {
                            leftTree = leftTree->GetLeftNode();
                        }
                        leftTree->AddLeftNode("var");
                    }
                } else root_tree->AddLeftNode("var");

                Tree* var_node = root_tree;
                while(var_node->GetValue() != "var")
                    var_node = var_node->GetLeftNode();

                vardpParse(t_iter, var_node);
                break;
            }
            case begin_tk: {
                root_tree->AddRightTree(compoundParse(t_iter, 0));
                break;
            }
            case dot_tk: {
                if (!error) std::cout << "Program was parse successfully" << std::endl;
                break;
            }
            default: {
                break;
            }
        }
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::list<std::string> Syntax_parser::vardParse(lex_it& t_iter) {
    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_ID);
        return std::list<std::string>();
    }

    if (isVarExist(iter->getName())) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(DUPLICATE);
    }
    else id_map.emplace(iter->getName(), Variable("?", "?"));

    std::list<std::string> var_list;
    var_list.push_back(t_iter->getName());

    iter = getNextLex(t_iter);

    if (checkLexem(iter, comma_tk))
        var_list.splice(var_list.end(), vardParse(t_iter));

    return var_list;
}

int Syntax_parser::vardpParse(lex_it& t_iter, Tree *t_tree) {
    auto var_list = vardParse(t_iter);
    auto* tree_value = Tree::CreateNode("");
    bool isArray{ false };

    if (!checkLexem(t_iter, ddt_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_COL);
    }

    auto type_iter = getNextLex(t_iter);

    if (!checkLexem(t_iter, type_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_TYPE);
    }

    getNextLex(t_iter);
    if (!checkLexem(t_iter, semi_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_SEMI);
    }

    updateVarTypes(var_list, type_iter->getName());


    if (t_tree->GetValue() == "var") {
//        while (t_tree->GetLeftNode() != nullptr)
//            t_tree = t_tree->GetRightNode();
        //t_tree->AddRightNode("$");
        buildVarTree(var_list, t_tree);
    }
    else {
        if(t_tree->GetLeftNode() == nullptr && t_tree->GetRightNode() != nullptr)
            buildVarTree(var_list, t_tree);
        else throw;
    }
    Tree::FreeTree(tree_value);


    if (checkLexem(peekLex(1, t_iter), id_tk) || checkLexem(peekLex(1, t_iter), var_tk)) {
        if (checkLexem(peekLex(1, t_iter), var_tk))
            getNextLex(t_iter);
        vardpParse(t_iter, t_tree->GetRightNode());
    }

    return EXIT_SUCCESS;
}

Tree* Syntax_parser::stateParse(lex_it& t_iter, int c_count) {
    Tree *result_tree = nullptr;
    auto iter = getNextLex(t_iter);
    switch (iter->getToken()) {
        case id_tk: {
            if (!isVarExist(iter->getName())) {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(UNKNOWN);
                return nullptr;
            }

            auto var_iter = iter;
            getNextLex(t_iter);


            auto *tree_exp = Tree::CreateNode(t_iter->getName());

            //tree_exp = Tree::CreateNode(t_iter->getName());
            tree_exp->AddLeftNode(var_iter->getName(), 0);



            expressionParse(t_iter, tree_exp, 0);


            //tree_exp->PrintTree(0);
            //tree_exp->PrintTree_2();

            result_tree = tree_exp;
            break;
        }
        case begin_tk: {
            auto *tree_comp = compoundParse(t_iter, c_count);
            if (!checkLexem(peekLex(1, t_iter), semi_tk)) {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_SEMI);
                return nullptr;
            }
            else getNextLex(t_iter);

            if (tree_comp != nullptr)
                result_tree = tree_comp;
            break;
        }
        case goto_tk: {
            iter = getNextLex(t_iter);
            if(iter->getToken() != id_tk)
            {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_ID);
                return nullptr;
            }

            auto label_name = iter;

            auto *tree_exp = Tree::CreateNode("goto");

            tree_exp->AddLeftNode(label_name->getName(), 0);

            result_tree = tree_exp;
            break;
        }
        case case_tk:{
            iter = getNextLex(t_iter);

            if(iter->getToken() != id_tk)
            {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_ID);
                return nullptr;
            }

            if (!isVarExist(iter->getName())) {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(UNKNOWN);
                return nullptr;
            }

            iter = getNextLex(t_iter);
            if(!checkLexem(iter, of_tk))
            {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_OF);
                return nullptr;
            }

            Tree* case_tree = buildCaseTree(t_iter, c_count);
            getNextLex(t_iter);
            getNextLex(t_iter);
            result_tree = case_tree;
        }
        default: {
            break;
        }
    }
    return result_tree;
}

Tree* Syntax_parser::compoundParse(lex_it& t_iter, int c_count) {
    c_count++;
    int local_lvl = c_count; // save current compound level
    int sec_prm = 0;

    auto label = [&]() -> std::string {
        return "_op" + std::to_string(local_lvl) + "." +
               std::to_string(sec_prm);
    };

    auto is_end = [&]() -> bool {
        return (checkLexem(peekLex(1, t_iter), end_tk)
                || checkLexem(peekLex(1, t_iter), eof_tk));
    };

    Tree *tree = Tree::CreateNode(t_iter->getName()); // 'begin' node
    auto* root_compound_tree = tree; // save pointer of start of subtree

    bool was_label = false;
    std::string label_name;

    while (t_iter->getToken() != end_tk) {
        if (t_iter->getToken() == eof_tk) {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(EOF_ERR);
            return nullptr;
        }
        if (t_iter->getToken() != dot_tk)
        {
            //checking if label
            if(peekLex(1,t_iter)->getToken() == id_tk)
            {
                for(auto& el:id_map) {
                    if(el.first == peekLex(1,t_iter)->getName() && el.second.type == "label")
                    {
                        if(peekLex(2, t_iter)->getToken() != ddt_tk)
                        {
                            std::cout << "Line "<< t_iter->getLine() << ": ";
                            handler.handle(MISSING_COL);
                            return nullptr;
                        }
                        was_label = true;
                        label_name = peekLex(1,t_iter)->getName();
                        getNextLex(t_iter);
                        getNextLex(t_iter);
                    }
                }
            }

            auto *subTree = stateParse(t_iter, c_count);
            if (subTree != nullptr) {
                if(was_label)
                {
                    tree->AddRightNode(label_name);
                    was_label = false;
                }
                else
                    tree->AddRightNode(label());
                tree->GetRightNode()->AddLeftTree(subTree);
                tree = tree->GetRightNode();

                if (!is_end()) sec_prm++;
            }
        }
        else break;
    }

    if (c_count == 1) {
        if (checkLexem(peekLex(1, t_iter), unknown_tk) ||
            checkLexem(peekLex(1, t_iter), eof_tk) ||
            !checkLexem(peekLex(1, t_iter), dot_tk)) {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_DOT);
            return nullptr;
        }
        tree->AddRightNode(t_iter->getName() + ".");
    } else
        tree->AddRightNode(t_iter->getName());
    return root_compound_tree;
}

int Syntax_parser::expressionParse(lex_it& t_iter, Tree *tree, int t_lvl) {
    lex_it var_iter;
    Tree *subTree;


    auto iter = getNextLex(t_iter);
    switch (iter->getToken()) {
        case id_tk: { // like a := b;
            if (!isVarExist(iter->getName()))
            {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(UNKNOWN);
            }
            var_iter = iter;
            getNextLex(iter);

            subTree = simplExprParse(var_iter, t_iter, tree, t_lvl);
            break;
        }
        case constant_tk: { // like a := 3 ...
            var_iter = iter;
            subTree = simplExprParse(var_iter, t_iter, tree, t_lvl);
            break;
        }
        case sub_tk: { // like a := -3;
            var_iter = t_iter;

            if (getNextLex(t_iter)->getToken() != constant_tk && !checkLexem(t_iter, id_tk)) {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_ID);
                return -EXIT_FAILURE;
            }

            tree->AddRightNode(var_iter->getName());
            tree->GetRightNode()->AddLeftNode("0");
            var_iter = t_iter;
            subTree = simplExprParse(var_iter, t_iter, tree->GetRightNode(), t_lvl);

            break;
        }
        case leftBracket_tk: {
            t_lvl += 3;
            expressionParse(t_iter, tree, t_lvl);
            break;
        }
        case rightBracket_tk: {
            if (getNextLex(t_iter)->getToken() != semi_tk) {
                t_lvl -= 3;;
                t_iter = getPrevLex(iter);
                lex_table.erase(getNextLex(iter));
                getPrevLex(t_iter);
                expressionParse(t_iter, tree, t_lvl);
            }
            else {
                t_lvl -= 3;;
                var_iter = getPrevLex(iter);
                t_iter = var_iter;
                getNextLex(iter);
                lex_table.erase(iter);
                simplExprParse(var_iter, t_iter, tree, t_lvl);
            }
            break;
        }
        case semi_tk: {
            if (t_lvl > 0) {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_RIGHT_BRACKET);
                return -EXIT_FAILURE;
            }
            if (t_lvl < 0) {
                std::cout << "Line "<< t_iter->getLine() << ": ";
                handler.handle(MISSING_LEFT_BRACKET);
                return -EXIT_FAILURE;
            }
            break;
        }
        case then_tk: {
            return EXIT_SUCCESS;
        }
        default: {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_ID);
            return -EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

Tree* Syntax_parser::simplExprParse(const lex_it& var_iter, lex_it& t_iter, Tree* tree, int t_lvl)
{
    Tree* subTree;
    auto iter = getNextLex(t_iter);
    switch (iter->getToken()) {
        case add_tk:
        case sub_tk:
        case mul_tk:
        case div_tk:
        case bool_eqv_tk:
        case bool_noneqv_tk:
        case bool_bigger_tk:
        case bool_less_tk:
        case bool_bigeqv_tk:
        case bool_leseqv_tk:{
            if (operations.at(iter->getName()) + t_lvl <= (tree->GetPriority())) {    // Priority of current <=
                tree->AddRightNode(var_iter->getName());
                subTree = tree->GetParentNode();

                while (operations.at(iter->getName()) + t_lvl <= operations.at(subTree->GetValue())) // go through parents
                    subTree = subTree->GetParentNode();

                subTree = createLowestOpTree(subTree, iter->getName(), operations.at(iter->getName()) + t_lvl);
            }
            else { // if Priority of current >
                /******* Create a new node of subexpression ************/
                tree->AddRightNode(iter->getName(), operations.at(iter->getName()) + t_lvl);   //     <oper> <- subTree
                subTree = tree->GetRightNode();                                                //      /  /
                subTree->AddLeftNode(var_iter->getName());                                     //    val  nullptr
                /********************************************************/
            }
            expressionParse(t_iter, subTree, t_lvl);
            break;
        }
        default: { // any other lexem, expression is over
            if (iter->getToken() == rightBracket_tk) {
                getPrevLex(t_iter);
                expressionParse(t_iter, tree, t_lvl);
            }
            else {
                if (t_lvl != 0)
                {
                    getPrevLex(t_iter);
                    expressionParse(t_iter, tree, t_lvl);
                }
                tree->AddRightNode(var_iter->getName());
            }
            break;
        }
    }
    return tree;
}

Tree* Syntax_parser::simplExprParse(Tree* var_tree, lex_it& t_iter, Tree* tree, int t_lvl)
{
    Tree* subTree;
    auto iter = getNextLex(t_iter);
    switch (iter->getToken()) {
        case add_tk:
        case sub_tk:
        case mul_tk:
        case div_tk:
        case comp_tk: {
            if (operations.at(iter->getName()) + t_lvl <= (tree->GetPriority())) {    // Priority of current <=
                tree->AddRightTree(var_tree);
                subTree = tree->GetParentNode();

                while (operations.at(iter->getName()) + t_lvl <= operations.at(subTree->GetValue())) // go through parents
                    subTree = subTree->GetParentNode();

                subTree = createLowestOpTree(subTree, iter->getName(), operations.at(iter->getName()) + t_lvl);
            }
            else { // if Priority of current >
                /******* Create a new node of subexpression ************/
                tree->AddRightNode(iter->getName(), operations.at(iter->getName()) + t_lvl);   //     <oper> <- subTree
                subTree = tree->GetRightNode();                                                //      /  /
                subTree->AddLeftTree(var_tree);                                     //    val  nullptr
                /********************************************************/
            }
            expressionParse(t_iter, subTree, t_lvl);
            break;
        }
        default: { // any other lexem, expression is over
            if (iter->getToken() == rightBracket_tk) {
                getPrevLex(t_iter);
                expressionParse(t_iter, tree, t_lvl);
            }
            else {
                if (t_lvl != 0)
                {
                    getPrevLex(t_iter);
                    expressionParse(t_iter, tree, t_lvl);
                }
                tree->AddRightTree(var_tree);
            }
            break;
        }
    }
    return tree;
}

bool Syntax_parser::checkLexem(const lex_it& t_iter, const tokens& t_tok) {
    if (t_iter == lex_table.end())   return false;
    if (t_iter->getToken() != t_tok) return false;

    return true;
}

bool Syntax_parser::isVarExist(const std::string& t_var_name) {
    auto map_iter = id_map.find(t_var_name);
    return !(map_iter == id_map.end());
}

void Syntax_parser::updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name) {
    try {
        for (auto& el : t_var_list)
            id_map.at(el).type = t_type_name;
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Syntax_parser::updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name,
                            const std::pair<int, int>& range) {
    try {
        for (auto& el : t_var_list) {
            id_map.at(el).type = t_type_name;
            id_map.at(el).isarray = true;
            id_map.at(el).range.first = range.first;
            id_map.at(el).range.second = range.second;
        }
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Syntax_parser::buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree) {
    try {
        auto i = 0;
        for (auto& el : t_var_list) {
            auto* tmp_tree = Tree::CreateNode("$");
            tmp_tree->AddRightNode(el);
            tmp_tree->GetRightNode()->AddRightNode(id_map.at(el).type);
            createVarTree(t_tree, tmp_tree, i++);
        }
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Syntax_parser::buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree, Tree* array_tree) {
    try {
        auto i = 0;

        for (auto& el : t_var_list) {
            auto* tmp_tree = Tree::CreateNode(el);
            tmp_tree->AddRightTree(array_tree);
            array_tree->AddRightNode(id_map.at(el).type, 0);
            createVarTree(t_tree, tmp_tree, i++);
        }
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}



void Syntax_parser::createVarTree(Tree* t_tree, Tree* t_donor_tree, int lvl) {
//    if (lvl > 0) {
//        lvl--;
//        createVarTree(t_tree->GetRightNode(), t_donor_tree, lvl);
//    }
//    else {
//        t_tree->AddRightTree(t_donor_tree);
//        t_tree->AddLeftNode("$");
//    }
    if(t_tree->GetValue() == "var")
    {
        t_tree->AddRightTree(t_donor_tree);
    } else {
        t_tree->AddLeftTree(t_donor_tree);
    }
}

Tree* Syntax_parser::createLowestOpTree(Tree* t_parent_tree, std::string value, int priority_) {
    auto* lowest_tree = Tree::CreateNode(t_parent_tree, value, priority_);
    lowest_tree->AddLeftTree(t_parent_tree->GetRightNode());
    t_parent_tree->AddRightTree(lowest_tree);

    return lowest_tree;
}

std::list<std::string> Syntax_parser::constdParse(Syntax_parser::lex_it &t_iter) {
    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_ID);
        return std::list<std::string>();
    }

    if (isConstExist(iter->getName())) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(DUPLICATE);
    }
    else {
        if(peekLex(1, t_iter)->getToken() != eqv_tk)
        {
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_EQU);
        }
        std::string const_value = peekLex(2, t_iter)->getName();
        id_map.emplace(iter->getName(), Variable("?", const_value));
    }

    std::list<std::string> const_list;
    const_list.push_back(t_iter->getName());

    return const_list;

}

std::list<std::string> Syntax_parser::labeldParse(Syntax_parser::lex_it &t_iter) {
    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_ID);
        return std::list<std::string>();
    }

    if (isLabelExist(iter->getName())){
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(DUPLICATE);
    }
    else id_map.emplace(iter->getName(), Variable("label", "-1"));

    std::list<std::string> label_list;
    label_list.push_back(iter->getName());

    iter = getNextLex(t_iter);

    if (checkLexem(iter, comma_tk))
        label_list.splice(label_list.end(), labeldParse(t_iter));

    return label_list;
}

int Syntax_parser::constdpParse(Syntax_parser::lex_it &t_iter, Tree *t_tree) {
    auto const_list = constdParse(t_iter);
    auto* tree_value = Tree::CreateNode("");
    //bool isArray{ false };

    getNextLex(t_iter);
    if (!checkLexem(t_iter, eqv_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_EQU);
    }

    getNextLex(t_iter);
    if (!checkLexem(t_iter, constant_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_CONST);
    }

    //TODO: save const value

    getNextLex(t_iter);
    if (!checkLexem(t_iter, semi_tk)) {
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_SEMI);
    }

    if(t_tree->GetValue() == "const")
        buildConstTree(const_list, t_tree);
    else {
        if(t_tree->GetLeftNode() == nullptr && t_tree->GetRightNode() != nullptr)
            buildConstTree(const_list, t_tree);
        else throw;
    }
    Tree::FreeTree(tree_value);


    if (checkLexem(peekLex(1, t_iter), id_tk) || checkLexem(peekLex(1, t_iter), const_tk)) {
        if (checkLexem(peekLex(1, t_iter), const_tk))
            getNextLex(t_iter);
        constdpParse(t_iter, t_tree->GetRightNode());
    }

    return -EXIT_SUCCESS;
}

int Syntax_parser::labeldpParse(lex_it &t_iter, Tree *t_tree) {
    auto label_list = labeldParse(t_iter);
    auto* tree_value = Tree::CreateNode("");
    //bool isArray{ false };

    if(!checkLexem(t_iter, semi_tk)){
        std::cout << "Line "<< t_iter->getLine() << ": ";
        handler.handle(MISSING_SEMI);
    }

    if (t_tree->GetValue() == "label")
        buildLabelTree(label_list, t_tree);
    else {
        if(t_tree->GetLeftNode() == nullptr && t_tree->GetRightNode() != nullptr)
            buildLabelTree(label_list, t_tree);
        else throw;
    }

    if (checkLexem(peekLex(1, t_iter), id_tk) || checkLexem(peekLex(1, t_iter), label_tk)) {
        if (checkLexem(peekLex(1, t_iter), label_tk))
            getNextLex(t_iter);
        labeldpParse(t_iter, t_tree->GetRightNode());
    }

    return -EXIT_SUCCESS;
}

void Syntax_parser::buildConstTree(const std::list<std::string>& t_const_list, Tree* t_tree) {
    try{
        int i = 0;
        for(auto& el : t_const_list) {
            auto* tmp_tree = Tree::CreateNode("$");
            tmp_tree->AddRightNode(el);
            tmp_tree->GetRightNode()->AddRightNode(id_map.at(el).value);
            createConstTree(t_tree, tmp_tree, i++);
        }
    } catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Syntax_parser::createConstTree(Tree* t_tree, Tree* t_donor_tree, int lvl) {
    if(t_tree->GetValue() == "const")
    {
        t_tree->AddRightTree(t_donor_tree);
    } else {
        t_tree->AddLeftTree(t_donor_tree);
    }
}

bool Syntax_parser::isConstExist(const string &t_const_name) {
    auto map_iter = id_map.find(t_const_name);
    return !(map_iter == id_map.end());
}

bool Syntax_parser::isLabelExist(const std::string &t_label_name) {
    auto map_iter = id_map.find(t_label_name);
    return !(map_iter == id_map.end());
}

void Syntax_parser::buildLabelTree(const list<std::string> &t_label_list, Tree *t_tree) {
    try{
        int i = 0;
        for(auto& el : t_label_list) {
            auto* tmp_tree = Tree::CreateNode("$");
            tmp_tree->AddRightNode(el);
            createLabelTree(t_tree, tmp_tree, i++);
        }
    } catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
                  << exp.what() << std::endl;
    }
}

void Syntax_parser::createLabelTree(Tree *t_tree, Tree *t_donor_tree, int lvl) {
    if(t_tree->GetValue() == "label")
    {
        if(t_tree->GetRightNode()!= nullptr){
            t_tree = t_tree->GetRightNode();
            while(t_tree->GetLeftNode()!= nullptr)
                t_tree = t_tree->GetLeftNode();

            t_tree->AddLeftTree(t_donor_tree);
        } else t_tree->AddRightTree(t_donor_tree);
    }
}

Tree *Syntax_parser::buildCaseTree(Syntax_parser::lex_it &t_iter, int c_count) {
    //Tree* case_tree = nullptr;
    auto *case_tree = Tree::CreateNode("case");
    //Tree *sub_tree = nullptr;
    while(peekLex(1, t_iter)->getToken() != end_tk && peekLex(2, t_iter)->getToken() != semi_tk)
    {
        if(getNextLex(t_iter)->getToken() != constant_tk){
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_CONSTANT);
            return nullptr;
        }
        auto *sub_tree = Tree::CreateNode(t_iter->getName());
//        case_tree->AddRightNode(t_iter->getName()); //case label node

        if(getNextLex(t_iter)->getToken() != ddt_tk){
            std::cout << "Line "<< t_iter->getLine() << ": ";
            handler.handle(MISSING_COL);
            return nullptr;
        }

        sub_tree->AddLeftTree(stateParse(t_iter, c_count));
        if(case_tree->GetRightNode() != nullptr){
            Tree* tree_node = case_tree;
            while(tree_node->GetRightNode()!= nullptr)
                tree_node = tree_node->GetRightNode();
            tree_node->AddRightTree(sub_tree);
        }
        else
            case_tree->AddRightTree(sub_tree);
    }


    return case_tree;
}
