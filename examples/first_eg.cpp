/* A  -> MA'
 * A' -> +MA' | -MA' | EPSILON
 * M  -> EM'
 * M' -> %EM' | /EM' | *EM' | EPSILON
 * E  -> (A) | iden
 *
 * input: a*((b+c)*d)/e
 *
 * to build:
 * $ export LD_LIBRARY_PATH=path/to/parsegen$LD_LIBRARY_PATH       # needed just once per terminal session
 * $ g++ first_eg.cpp -o first_eg -Lpath/to/parsegen -lparsegen
 */

#include <iostream>
#include "../include/parsegen.h"

enum TokenType : int{
    ADD, SUB, MUL, DIV, MOD,
    OPA, CPA,
    IDEN,
    EPSILON, DOLLAR
};
enum NonTerminal : int {
    A = numeric_limits<int>::min(), Adash, M, Mdash, E
};

int main(int argc, char** argv)
{
    map<NonTerminal, vector<vector<int>>> rules = {
        {A    , {{M, Adash}}},
        {Adash, {{ADD, M, Adash}, {SUB, M, Adash}, {EPSILON}}},
        {M    , {{E, Mdash}}},
        {Mdash, {{MOD, E, Mdash}, {DIV, E, Mdash}, {MUL, E, Mdash}, {EPSILON}}},
        {E    , {{OPA, A, CPA}, {IDEN}}}
    };
    std::vector<Token> tokens = {
        Token(IDEN, "a", 1),
        Token(MUL , "*", 1),
        Token(OPA , "(", 1),
        Token(OPA , "(", 1),
        Token(IDEN, "b", 1),
        Token(ADD , "+", 1),
        Token(IDEN, "c", 1),
        Token(CPA , ")", 1),
        Token(MUL , "*", 1),
        Token(IDEN, "d", 1),
        Token(CPA , ")", 1),
        Token(DIV , "/", 1),
        Token(IDEN, "e", 1)
    };

    ParseGen p1;
    p1.set_rules(rules);
    p1.set_epsilon(EPSILON);
    p1.set_dollar(DOLLAR);
    p1.set_start_symbol(A);
    p1.calc_first();
    p1.calc_follow();

    p1.print_rules();  cout<<"\n";
    p1.print_first();  cout<<"\n";
    p1.print_follow(); cout<<"\n";
    p1.print_table();  cout<<"\n";


    p1.set_tokens(tokens);
    p1.print_tokens(); cout<<"\n";

    ParseTreeNode *t = p1.get_tree();
    t->preorder();
    delete t;
    return 0;
}