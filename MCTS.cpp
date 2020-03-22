#include "MCTS.h"
#include <cmath>
#include <random>
#include <functional>
#include <chrono>
#include <ctime>
#include <iostream>

int MCTS::Node::memSize = 0;

MCTS::MCTS(): root(new MCTS::Node)
{}

MCTS::MCTS(const Othello& game): root(new MCTS::Node(game))
{}

MCTS::~MCTS()
{
    delete root;
}

MCTS::Node* MCTS::select()
{
    Node *ptr = root;
    while(ptr && !ptr->childs.empty())
    {
        double maxi = -100;
        int index = -1;
        double sqrcnt = std::sqrt(ptr->count);
        for(auto i: ptr->childs)
        {
            Node *c = i.second.first;
            //unvisited node first
            if(c->count == 0)
            {
                index = i.first;
                break;
            }
            //select the node with maximum priori + reward
            //the importance of priori decreases as visit count increases
            double value =
                i.second.second * sqrcnt / (1 + c->count)
                + (c->reward * ptr->status.currentPlayer() + 1) / (c->count);
            if(value > maxi)
            {
                maxi = value;
                index = i.first;
            }
        }
        ptr = ptr->childs[index].first;
    }
    return ptr;
}

MCTS::Node* MCTS::expand(Node* p)
{
    if(p->status.finish()) return p;
#ifdef _BOTZONE_ONLINE
    if(p->memSize >= 192 << 20) return p;
#endif
    static std::default_random_engine def_gen(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<double> dist(0, 1);
    static auto dice = std::bind(dist, def_gen);
    double expsum = 0;
    int color = p->status.currentPlayer();
    double tmp[64];
    int idx[64];
    int ptr = 0;
    for(int r = 0; r < p->status.width; r++)
    {
        for(int c = 0; c < p->status.width; c++)
        {
            //for each valid move, prepare to create child node
            if(p->status.valid(r, c, color))
            {
                tmp[ptr] = priori[r][c];
                expsum += priori[r][c];
                idx[ptr++] = r * 8 + c;
            }
        }
    }
    //no valid move, pass this turn
    if(ptr == 0)
    {
        Node *chld = new Node(p->status);
        chld->parent = p;
        chld->status.skip();
        p->childs[-1] = std::make_pair(chld, 1);
        chld->memSize += sizeof(decltype(p->childs)::value_type);
        return chld;
    }
    double rd = dice() * expsum;
    Node *res = nullptr;
    //create nodes and randomly select one to simulate
    for(int i = 0; i < ptr; i++)
    {
        Node *chld = new Node(p->status);
        chld->parent = p;
        chld->status.step(idx[i] / 8, idx[i] % 8);
        p->childs[idx[i]] = std::make_pair(chld, tmp[i] / expsum);
        chld->memSize += sizeof(decltype(p->childs)::value_type);
        if(rd <= tmp[i] && !res)
        {
            res = chld;
        }
        rd -= tmp[i];
    }
    return res;
}

double MCTS::simulate(Node* p)
{
    double tmp[64];
    int idx[64];
    static std::default_random_engine def_gen(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<double> dist(0, 1);
    static auto dice = std::bind(dist, def_gen);
    Othello status = p->status;
    //randomly play based on priori probability until the game ends
    while(!status.finish())
    {
        double expsum = 0;
        int color = status.currentPlayer();
        int ptr = 0;
        for(int r = 0; r < status.width; r++)
        {
            for(int c = 0; c < status.width; c++)
            {
                if(status.valid(r, c, color))
                {
                    tmp[ptr] = priori[r][c];
                    expsum += priori[r][c];
                    idx[ptr++] = r * 8 + c;
                }
            }
        }
        if(ptr == 0)
        {
            status.skip();
            continue;
        }
        //randomly select move
        double rd = dice() * expsum;
        for(int i = 0; i < ptr; i++)
        {
            if(rd <= tmp[i])
            {
                status.step(idx[i] / 8, idx[i] % 8);
                break;
            }
            rd -= tmp[i];
        }
    }
    int black = status.count(status.black), white = status.count(status.white);
    return black == white ? 0 :
                (black > white ? status.black : status.white);
}

void MCTS::backup(Node* p, double reward)
{
    while(p)
    {
        p->reward += reward;
        p->count += 1;
        p = p->parent;
    }
}

void MCTS::iterate()
{
    Node *p = select();
    p = expand(p);
    double reward = simulate(p);
    backup(p, reward);
}


int MCTS::decide(int iteration, long tlimit)
{
    clock_t ts = clock();
    while(iteration-- && (tlimit == 0 || clock() - ts < tlimit))
    {
        iterate();
    }
    int action = -1, maxi = 0;
    //select most visited node
    for(auto i: root->childs)
    {
        if(i.second.first->count > maxi)
        {
            maxi = i.second.first->count;
            action = i.first;
        }
    }
    return action;
}

void MCTS::act(int action)
{
    auto iter = root->childs.find(action);
    //action has not been explored
    if(iter == root->childs.end())
    {
        //create tree
        Node *p = new Node(root->status);
        if(action == -1)
            p->status.skip();
        else
            p->status.step(action / 8, action % 8);
        //discard current tree
        delete root;
        //enter
        root = p;
    }
    else
    {
        //discard rest part
        Node *p = iter->second.first;
        root->childs.erase(iter);
        delete root;
        //enter the sub-tree
        root = p;
        root->parent = nullptr;
    }
}

void MCTS::debug() const
{
    std::cout << root->value << ' ' << root->count << ' '
              << root->reward << ' ' << root->reward / root->count << std::endl;
    for(auto i: root->childs)
    {
        if(i.first == -1)
            std::cout << '\t' << i.first << ": " << i.second.second << ' '
                      << i.second.first->count << ' ' << i.second.first->reward << ' '
                      << i.second.first->reward / i.second.first->count << std::endl;
        else
            std::cout << '\t' << (i.first / 8) << ' ' << (i.first % 8) << ": " << i.second.second << ' '
                      << i.second.first->count << ' ' << i.second.first->reward << ' '
                      << i.second.first->reward / i.second.first->count << std::endl;
    }
}
