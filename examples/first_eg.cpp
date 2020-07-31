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
#include "parsegen.h"

using namespace amicayse;

enum amicayse::TokenType : int{
    ADD, SUB, MUL, DIV, MOD,
    OPA, CPA,
    IDEN,
    EPSILON, DOLLAR
};
enum amicayse::NonTerminal : int {
    A = std::numeric_limits<int>::min(), Adash, M, Mdash, E
};

int main(int argc, char** argv)
{
    ParseTreeNode *t;
    std::map<NonTerminal, std::vector<std::vector<int>>> rules = {
        {A    , {{M, Adash}}},
        {Adash, {{ADD, M, Adash}, {SUB, M, Adash}, {EPSILON}}},
        {M    , {{E, Mdash}}},
        {Mdash, {{MOD, E, Mdash}, {DIV, E, Mdash}, {MUL, E, Mdash}, {EPSILON}}},
        {E    , {{OPA, A, CPA}, {IDEN}}}
    };
    std::vector<Token> tokens1 = {
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

    ParseGen p1(0);
    p1.set_rules(rules);
    p1.set_epsilon(EPSILON);
    p1.set_dollar(DOLLAR);
    p1.set_start_symbol(A);
    p1.calc_first();
    p1.calc_follow();

    p1.print_rules();  std::cout<<"\n";
    p1.print_first();  std::cout<<"\n";
    p1.print_follow(); std::cout<<"\n";
    p1.print_table();  std::cout<<"\n";
    
    p1.set_tokens(tokens1);
    p1.print_tokens(); std::cout<<"\n";

    t = p1.get_parse_tree();
    t->preorder();
    delete t;

    std::cout<<"\n\n\n";
    std::vector<Token> tokens2 = {
        Token(IDEN, "a", 1),
        Token(MUL , "*", 1),
        Token(IDEN, "e", 1)
    };

    p1.set_tokens(tokens2);
    p1.print_tokens(); std::cout<<"\n";

    try{
        t = p1.get_parse_tree();
        t->preorder();
        delete t;
    }
    catch(ParserException ex){
        std::cout<<ex.what()<<std::endl;
    }
    return 0;
}