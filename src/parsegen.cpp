#include "parsegen.h"

#include <algorithm>

ParseGen::ParseGen(unsigned char flag){
    flags = flag;
}

set<int> ParseGen::get_first(NonTerminal tok){
    if(first[tok].empty()){
        for(vector<vector<int>>::iterator i=rules[tok].begin(); i!=rules[tok].end(); ++i){
            if(*(i->begin()) > -1){
                first[tok].insert( *(i->begin()) );
                if( *(i->begin())!=epsilon )
                    table[tok][(TokenType)*(i->begin())] = i;
            }
            else{
                set<int> t = get_first( (NonTerminal)*(i->begin()) );
                first[tok].insert(t.begin(), t.end());
                for(set<int>::iterator j=t.begin(); j!=t.end(); ++j){
                    if( *j!=epsilon )
                        table[tok][(TokenType)*j] = i;
                }

            }
        }
    }
    return first[tok];
}

void ParseGen::calc_first(){
    for(map<NonTerminal, vector<vector<int>>>::iterator i=rules.begin(); i!=rules.end(); ++i){
        first[i->first] = get_first(i->first);
    }
}

set<int> ParseGen::get_follow(NonTerminal tok){
    if(tok==(NonTerminal)numeric_limits<int>::min()){
        if (follow[tok].size()>1)
            return follow[tok];
    }
    else{
        if (!follow[tok].empty())
            return follow[tok];
    }

    for(map<NonTerminal, vector<vector<int>>>::iterator i=rules.begin(); i!=rules.end(); ++i){
        for(vector<vector<int>>::iterator k=i->second.begin(); k!=i->second.end(); ++k){
            for (vector<int>::iterator l=k->begin(); l!=k->end(); ++l){
                if(tok == *l){
                    vector<int>::iterator itr = l+1;
                    while(true){
                        if(itr==k->end()){
                            set<int> t = get_follow(i->first);
                            follow[tok].insert(t.begin(), t.end());
                            break;
                        }
                        else{
                            if(*itr > -1){
                                follow[tok].insert(*itr);
                                break;
                            }
                            else{
                                if(find(first[(NonTerminal)*itr].begin(), first[(NonTerminal)*itr].end(), (int)epsilon) == first[(NonTerminal)*itr].end()){
                                    follow[tok].insert(first[(NonTerminal)*itr].begin(), first[(NonTerminal)*itr].end());
                                    break;
                                }
                                else{
                                    follow[tok].insert(first[(NonTerminal)*itr].begin(), first[(NonTerminal)*itr].end());
                                    follow[tok].erase(epsilon);
                                    itr+=1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return follow[tok];
}
void ParseGen::calc_follow(){
    follow[(NonTerminal)numeric_limits<int>::min()].insert(dollar);

    for(map<NonTerminal, vector<vector<int>>>::iterator i=rules.begin(); i!=rules.end(); ++i){

        follow[i->first] = get_follow(i->first);

        // building table
        if(flags == 0x01)
            cout<<i->first<<": ";
        for(vector<vector<int>>::iterator j=rules[i->first].begin(); j!=rules[i->first].end(); ++j){
            if(flags == 0x01)
                cout<<*(j->begin())<<" | ";
            if( *(j->begin()) == epsilon ){
                for(set<int>::iterator k=follow[i->first].begin(); k!=follow[i->first].end(); ++k){
                    if(flags == 0x01)
                        cout<<*k<<":";
                    table[i->first][(TokenType)*k] = j;
                }
            }
        }
        if(flags == 0x01)
            cout<<"\n";
    }
}

ParseTreeNode* ParseGen::add_node(int node){
    ParseTreeNode *p=NULL;
    vector<vector<int>>::iterator r;

    if(node>=0){
        if(node==epsilon){
            return NULL;
        }
        if(node == itr->type){
            p = new ParseTreeNode(&(*itr));
            ++itr;
        }
        else{
            throw ParserException(string()
                +"Line "
                +to_string(itr->line)
                +": Expected "
                +to_string(node)
                +", got "
                +to_string(itr->type)
                +":"
                +itr->lexeme
                +"\n"
            );
        }
    }
    else{
        p = new ParseTreeNode((NonTerminal)node);
        r = table[(NonTerminal)node][itr->type];
        ParseTreeNode *x;
        for(vector<int>::iterator i=r->begin(); i!=r->end(); ++i){
            x = add_node(*i);
            if(x)
                p->add_child(x);
        }
        if (p->children.empty()){
            delete p;
            p = NULL;
        }
    }
    return p;
}


void ParseGen::set_epsilon(TokenType e){
    epsilon = e;
}
void ParseGen::set_dollar(TokenType d){
    dollar = d;
}
void ParseGen::set_start_symbol(NonTerminal s){
    start_symbol = s;
}

void ParseGen::set_rules(map<NonTerminal, vector<vector<int>>> r){
    rules = r;
}
void ParseGen::set_tokens(vector<Token> t){
    tokens = t;
    tokens.push_back(Token(dollar, "", -1));
    itr = tokens.begin();
}
ParseTreeNode* ParseGen::get_tree(){
    ParseTreeNode *p = NULL;
    p = add_node(start_symbol);

    if (itr->type != dollar){
        throw ParserException(string()
            +"Symbols still left after tree formed. First unused symbol: "
            +itr->lexeme+" at line "
            +to_string(itr->line)
            +"\n"
        );
    }

    return p;
}

void ParseGen::print_rules(){
    cout<<"Rules:\n";
    for(map<NonTerminal, vector<vector<int>>>::iterator i=rules.begin(); i!=rules.end(); ++i){
        cout<<i->first<<": ";
        for(vector<vector<int>>::iterator k=i->second.begin(); k!=i->second.end(); ++k){
            for (vector<int>::iterator l=k->begin(); l!=k->end(); ++l){
                cout<<*l<<" ";
            }
            cout<<" : ";
        }
        cout<<"\n";
    }
}

void ParseGen::print_first(){
    cout<<"First:\n";
    for(map<NonTerminal, set<int>>::iterator i=first.begin(); i!=first.end(); ++i){
        cout<<i->first<<":";
        for(set<int>::iterator j=i->second.begin(); j!=i->second.end(); ++j)
            cout<<" "<<*j;
        cout<<"\n";
    }
}

void ParseGen::print_follow(){
    cout<<"Follow:\n";
    for(map<NonTerminal, set<int>>::iterator i=follow.begin(); i!=follow.end(); ++i){
        cout<<i->first<<":";
        //for(set<int>::iterator j=i->second.begin(); j!=i->second.end(); ++j)
        for(auto const& j: i->second)
            cout<<" "<<j;
        cout<<"\n";
    }
}

void ParseGen::print_table(){
    cout<<"Table:\n";
    for(map<NonTerminal, map<TokenType, vector<vector<int>>::iterator>>::iterator i=table.begin(); i!=table.end(); ++i){
        cout<<i->first<<"\n";
        for(map<TokenType, vector<vector<int>>::iterator>::iterator j=i->second.begin(); j!=i->second.end(); ++j){
            cout<<"          "<<j->first<<":";
            for(vector<int>::iterator k=j->second->begin(); k!=j->second->end(); ++k){
                cout<<" "<<*k;
            }
            cout<<"\n";
        }
    }
}

void ParseGen::print_tokens(){
    cout<<"Input: ";
    for(vector<Token>::iterator i=tokens.begin(); i!=tokens.end(); ++i ){
        cout<<i->lexeme<<" ";
    }
    cout<<"\n";
}