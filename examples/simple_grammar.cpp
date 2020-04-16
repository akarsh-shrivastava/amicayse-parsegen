/* Grammar:
 * S -> aAaBC
 * A -> a
 * B -> b | EPSILON
 * C -> c
 *
 * input 1: aaabc
 * input 2: aaac
 *
 * to build:
 * $ export LD_LIBRARY_PATH=path/to/parsegen$LD_LIBRARY_PATH       # needed just once per terminal session
 * $ g++ simple_grammar.cpp -o simple_grammar -Lpath/to/parsegen -lparsegen
 */

#include <iostream>
#include "../include/parsegen.h"

enum TokenType : int{
    a, b, c, d,
    EPSILON, DOLLAR
};
enum NonTerminal : int {
    S = numeric_limits<int>::min(), A, B, C 
};
int main(int argc, char** argv)
{

    map<NonTerminal, vector<vector<int>>> rules = {
        {S    , {{a, A, a, B, C}}},
        {A    , {{a}}},
        {B    , {{b}, {EPSILON}}},
        {C    , {{c}}}
    };
    std::vector<Token> tokens1 = {
        Token(a , "a", 1),
        Token(a , "a", 1),
        Token(a , "a", 1),
        Token(b , "b", 1),
        Token(c , "c", 1),
    };
    std::vector<Token> tokens2 = {
        Token(a , "a", 1),
        Token(a , "a", 1),
        Token(a , "a", 1),
        Token(c , "c", 1),
    };

    ParseGen p1;
    p1.set_rules(rules);
    p1.set_epsilon(EPSILON);
    p1.set_dollar(DOLLAR);
    p1.set_start_symbol(S);
    p1.calc_first();
    p1.calc_follow();

    p1.print_rules();  cout<<"\n";
    p1.print_first();  cout<<"\n";
    p1.print_follow(); cout<<"\n";
    p1.print_table();  cout<<"\n";

    ParseTreeNode *t;

    p1.set_tokens(tokens1);
    p1.print_tokens(); cout<<"\n";
    t = p1.get_tree();
    t->preorder();
    delete t;

    p1.set_tokens(tokens2);
    p1.print_tokens(); cout<<"\n";
    t = p1.get_tree();
    t->preorder();
    delete t;

    return 0;
}