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

enum amicayse::TokenType : int{
    ADD, SUB, MUL, DIV, MOD,
    OPA, CPA,
    IDEN,
    EPSILON, DOLLAR
};
enum amicayse::NonTerminal : int {
    A = numeric_limits<int>::min(), Adash, M, Mdash, E
};

int main(int argc, char** argv)
{
    std::map<amicayse::NonTerminal, std::vector<std::vector<int>>> rules = {
        {amicayse::A    , {{amicayse::M, amicayse::Adash}}},
        {amicayse::Adash, {{amicayse::ADD, amicayse::M, amicayse::Adash}, {amicayse::SUB, amicayse::M, amicayse::Adash}, {amicayse::EPSILON}}},
        {amicayse::M    , {{amicayse::E, amicayse::Mdash}}},
        {amicayse::Mdash, {{amicayse::MOD, amicayse::E, amicayse::Mdash}, {amicayse::DIV, amicayse::E, amicayse::Mdash}, {amicayse::MUL, amicayse::E, amicayse::Mdash}, {amicayse::EPSILON}}},
        {amicayse::E    , {{amicayse::OPA, amicayse::A, amicayse::CPA}, {amicayse::IDEN}}}
    };
    std::vector<amicayse::Token> tokens = {
        amicayse::Token(amicayse::IDEN, "a", 1),
        amicayse::Token(amicayse::MUL , "*", 1),
        amicayse::Token(amicayse::OPA , "(", 1),
        amicayse::Token(amicayse::OPA , "(", 1),
        amicayse::Token(amicayse::IDEN, "b", 1),
        amicayse::Token(amicayse::ADD , "+", 1),
        amicayse::Token(amicayse::IDEN, "c", 1),
        amicayse::Token(amicayse::CPA , ")", 1),
        amicayse::Token(amicayse::MUL , "*", 1),
        amicayse::Token(amicayse::IDEN, "d", 1),
        amicayse::Token(amicayse::CPA , ")", 1),
        amicayse::Token(amicayse::DIV , "/", 1),
        amicayse::Token(amicayse::IDEN, "e", 1)
    };

    amicayse::ParseGen p1(0);
    p1.set_rules(rules);
    p1.set_epsilon(amicayse::EPSILON);
    p1.set_dollar(amicayse::DOLLAR);
    p1.set_start_symbol(amicayse::A);
    p1.calc_first();
    p1.calc_follow();

    p1.print_rules();  std::cout<<"\n";
    p1.print_first();  std::cout<<"\n";
    p1.print_follow(); std::cout<<"\n";
    p1.print_table();  std::cout<<"\n";


    p1.set_tokens(tokens);
    p1.print_tokens(); std::cout<<"\n";

    amicayse::ParseTreeNode *t = p1.get_tree();
    t->preorder();
    delete t;
    return 0;
}