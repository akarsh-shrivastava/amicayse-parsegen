#ifndef PARSEGEN_H_INCLUDED
    #define PARSEGEN_H_INCLUDED
    #include <map>
    #include <set>
    #include <vector>
    #include <string>
    #include <limits>
    #include <algorithm>
    #include "amicayse/utils.h"

    namespace amicayse
    {
        enum : unsigned char
        {
            PARSEGEN_VERBOSE_FOLLOW  = 0x1,
            PARSEGEN_VERBOSE_ADDNODE = 0x2,
        };
        class ParseGen
        {
            private:
            std::map<NonTerminal, std::vector<std::vector<int>>> rules;
            std::map<NonTerminal, std::set<int>> first, follow;
            std::map<NonTerminal, std::map<TokenType, std::vector<std::vector<int>>::iterator>> table;

            TokenType epsilon, dollar;
            NonTerminal start_symbol;

            std::vector<Token> tokens;
            std::vector<Token>::iterator itr;

            std::set<int> get_first(NonTerminal);
            std::set<int> get_follow(NonTerminal);
            ParseTreeNode* add_pt_node(int);

            unsigned char flags;

            public:
            ParseGen(unsigned char);

            void set_rules(std::map<NonTerminal, std::vector<std::vector<int>>>);
            void set_tokens(std::vector<Token>);

            void set_epsilon(TokenType);
            void set_dollar(TokenType);
            void set_start_symbol(NonTerminal);

            void calc_first();
            void calc_follow();

            ParseTreeNode* get_parse_tree();

            // utils
            void print_rules();
            void print_first();
            void print_follow();
            void print_table();
            void print_tokens();
        };
    }

#endif