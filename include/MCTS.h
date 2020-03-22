#ifndef __MCTS_H__
#define __MCTS_H__
#include "Othello.h"

#include <map>

class MCTS
{
public:
    struct Node
    {
        //total memory size used by MCTS, used on botzone
        static int memSize;
        //map: action -> (child node, priori probability)
        std::map<int, std::pair<Node*, double> > childs;
        Node *parent;
        //reward: total reward updated when Backup
        //value: the network output
        double reward, value;
        //visit count
        int count;
        //stored current game status
        Othello status;
        Node(): childs(), parent(nullptr), reward(0), value(0), count(0), status()
        {
            memSize += sizeof(Node);
        }
        Node(const Othello& game): childs(), parent(nullptr),
            reward(0), value(0), count(0), status(game)
        {
            memSize += sizeof(Node);
        }
        ~Node()
        {
            for(auto i: childs) delete i.second.first;
            memSize -= sizeof(Node) + sizeof(decltype(childs)::value_type) * childs.size();
        }
    };
protected:
    Node *root;
    //select a node to expand
    Node *select();
    //expand that node and return one of its child/itself
    //which means the node where simulate starts
    virtual Node* expand(Node*);
    //randomly play to end and return the reward
    //or simply return the network output of value
    virtual double simulate(Node*);
    //update the reward and visit count to its ancestors
    void backup(Node*, double reward);
public:
    //one select-expand-simulate-backup loop
    void iterate();
    //decide next move
    virtual int decide(int iteration = 5000, long tlimit = 0);
    //enter the related sub-tree and discard the rest part of tree
    void act(int action);
    
    MCTS();
    MCTS(const Othello &game);
    virtual ~MCTS();

    const Othello& game() const {return root->status;}
    //current reward, used to debug
    double curReward() {return root->reward / root->count;}
    //show the status of root node
    void debug() const;
};

#endif
