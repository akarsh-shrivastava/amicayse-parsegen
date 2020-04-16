#ifndef PARSEGEN_H_INCLUDED
    #define PARSEGEN_H_INCLUDED
    #include <map>
    #include <set>
    #include <vector>
    #include <string>
    #include <limits>
    #include "/home/akarsh/zzz/cdtools/utils.h"

    using namespace std;
    class ParseGen
    {
        private:
        map<NonTerminal, vector<vector<int>>> rules;
        map<NonTerminal, set<int>> first, follow;
        map<NonTerminal, map<TokenType, vector<vector<int>>::iterator>> table;

        TokenType epsilon, dollar;
        NonTerminal start_symbol;

        vector<Token> tokens;
        vector<Token>::iterator itr;

        set<int> get_first(NonTerminal);
        set<int> get_follow(NonTerminal);
        ParseTreeNode* add_node(int);

        public:
        void set_rules(map<NonTerminal, vector<vector<int>>>);
        void set_tokens(vector<Token>);

        void set_epsilon(TokenType);
        void set_dollar(TokenType);
        void set_start_symbol(NonTerminal);

        void calc_first();
        void calc_follow();

        ParseTreeNode* get_tree();

        // utils
        void print_rules();
        void print_first();
        void print_follow();
        void print_table();
        void print_tokens();
    };

#endif