#include <map>
#include <set>
#include <stack>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <iterator>
#include "../utils.h"
using namespace std;

void dumpstack(stack<int> st){
    cout<<"\n";
    while(!st.empty()){
        int x = st.top();
        if (x<0)
            cout<<"|"<<x<<"|\n";
        else
            cout<<"|     "<<x<<"     |\n";
        st.pop();
    }
    cout<<"|-----------|\n";
}

enum TokenType : int{
    ADD, SUB, MUL, DIV, MOD,
    OPA, CPA,
    IDEN,
    EPSILON, DOLLAR
};
enum NonTerminal : int {
    A = numeric_limits<int>::min(), Adash, M, Mdash, E
};


map<
    NonTerminal,
    vector<vector<int>>
> rules;
map<
    NonTerminal,
    set<int>
> first, follow;

map<NonTerminal, map<TokenType, vector<vector<int>>::iterator>> table;

vector<Token> tokens;
vector<Token>::iterator itr;
void initRules(){
    rules[A]     = {{M, Adash}};
    rules[Adash] = {{ADD, M, Adash}, {SUB, M, Adash}, {EPSILON}};
    rules[M]     = {{E, Mdash}};
    rules[Mdash] = {{MOD, E, Mdash}, {DIV, E, Mdash}, {MUL, E, Mdash}, {EPSILON}};
    rules[E]     = {{OPA, A, CPA}, {IDEN}};
}/*
void initRules(){
    rules[A]     = {{M, Adash}};
    rules[Adash] = {{ADD, M, Adash}, {EPSILON}};
    rules[M]     = {{E, Mdash}};
    rules[Mdash] = {{MUL, E, Mdash}, {EPSILON}};
    rules[E]     = {{OPA, A, CPA}, {IDEN}};
}*/
void init_tokens(){
    tokens = {
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
        Token(IDEN, "e", 1),
        Token(DOLLAR, "$", 1)
    };
    itr = tokens.begin();
}
set<int> get_first(NonTerminal tok){
    if(first[tok].empty()){
        for(vector<vector<int>>::iterator i=rules[tok].begin(); i!=rules[tok].end(); ++i){
            if(*(i->begin()) > -1){
                first[tok].insert( *(i->begin()) );
                if( *(i->begin())!=EPSILON )
                    table[tok][(TokenType)*(i->begin())] = i;
            }
            else{
                set<int> t = get_first( (NonTerminal)*(i->begin()) );
                first[tok].insert(t.begin(), t.end());
                for(set<int>::iterator j=t.begin(); j!=t.end(); ++j){
                    if( *j!=EPSILON )
                        table[tok][(TokenType)*j] = i;
                }

            }
        }
    }
    return first[tok];
}

void calc_first(){
    for(map<NonTerminal, vector<vector<int>>>::iterator i=rules.begin(); i!=rules.end(); ++i){
        first[i->first] = get_first(i->first);
    }
}

set<int> get_follow(NonTerminal tok){
    if(tok==(NonTerminal)numeric_limits<int>::min()){
        if (follow[tok].size()>1)
            return follow[tok];
    }
    else 
        if (!follow[tok].empty())
            return follow[tok];

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
                                if(find(first[(NonTerminal)*itr].begin(), first[(NonTerminal)*itr].end(), (int)EPSILON) == first[(NonTerminal)*itr].end()){
                                    follow[tok].insert(first[(NonTerminal)*itr].begin(), first[(NonTerminal)*itr].end());
                                    break;
                                }
                                else{
                                    follow[tok].insert(first[(NonTerminal)*itr].begin(), first[(NonTerminal)*itr].end());
                                    follow[tok].erase(EPSILON);
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
void calc_follow(){
    follow[(NonTerminal)numeric_limits<int>::min()].insert(DOLLAR);
    for(map<NonTerminal, vector<vector<int>>>::iterator i=rules.begin(); i!=rules.end(); ++i){
        follow[i->first] = get_follow(i->first);

        // building table
        //cout<<i->first<<": ";
        for(vector<vector<int>>::iterator j=rules[i->first].begin(); j!=rules[i->first].end(); ++j){
            //cout<<*(j->begin())<<" | ";
            if( *(j->begin()) == EPSILON ){
                for(set<int>::iterator k=follow[i->first].begin(); k!=follow[i->first].end(); ++k){
                    //cout<<*k<<":";
                    table[i->first][(TokenType)*k] = j;
                }
            }
        }
        //cout<<"\n";
    }
}

bool validate(){
    stack<int> st;
    st.push(A);
    int noi=1;
    for(vector<Token>::iterator i=tokens.begin(); i!=tokens.end() || !st.empty();){
        int top=DOLLAR;
        TokenType tok = DOLLAR;
        if(!st.empty()){
            top = st.top();
        }
        if(i!=tokens.end()){
            tok = i->type;
        }
        //cout<<"i:"<<noi<<"\n";
        //i->print();
        //dumpstack(st);
        //cout<<"\n";
        if(top>=0){
            if (top==tok){
                st.pop();
                ++i;
            }
            else if(top==EPSILON)
                st.pop();
            else
                return false;
        }
        else{
            try{
                st.pop();
                for(vector<int>::reverse_iterator j=table[(NonTerminal)top][tok]->rbegin(); j!=table[(NonTerminal)top][tok]->rend(); ++j)
                    st.push(*j);
            }
            catch(exception& e){
                return false;
            }
        }
        noi++;
    }
    if(st.empty())
        return true;
    else
        return false;
}

ParseTreeNode* add_node(int node){
    ParseTreeNode *p=NULL;
    vector<vector<int>>::iterator r;

    if(node>=0){
        if(node==EPSILON){
            return NULL;
        }
        if(node == itr->type){
            p = new ParseTreeNode(&(*itr));
            ++itr;
        }
        else{
            throw ParserException(string()+"Expected "+to_string(node)+", got "+to_string(itr->type)+":"+itr->lexeme+"\n");
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


int main(){
    initRules();
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
    cout<<"\n";
    calc_first();
    cout<<"\n\nFirst:\n";
    for(map<NonTerminal, set<int>>::iterator i=first.begin(); i!=first.end(); ++i){
        cout<<i->first<<":";
        for(set<int>::iterator j=i->second.begin(); j!=i->second.end(); ++j)
            cout<<" "<<*j;
        cout<<"\n";
    }
    cout<<"\n\n\n";
    calc_follow();
    cout<<"\n\nFollow:\n";
    for(map<NonTerminal, set<int>>::iterator i=follow.begin(); i!=follow.end(); ++i){
        cout<<i->first<<":";
        //for(set<int>::iterator j=i->second.begin(); j!=i->second.end(); ++j)
        for(auto const& j: i->second)
            cout<<" "<<j;
        cout<<"\n";
    }
    cout<<"\n\n\n\nTable:\n";
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

    cout<<"\n\n\n";
    init_tokens();
    for(vector<Token>::iterator i=tokens.begin(); i!=tokens.end(); ++i ){
        cout<<i->lexeme<<" ";
    }
    //cout<<"\n\n\n"<<validate();
    cout<<endl;
    ParseTreeNode *p = add_node(A);
    p->preorder();
    delete p;
    return 0;
}