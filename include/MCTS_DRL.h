#ifndef __MCTS_DRL_H__
#define __MCTS_DRL_H__
#include "MCTS.h"
#include "Net.h"

struct GameHistory
{
    Othello game;
    float labels[65];
    float reward;
};

class MCTS_DRL: public MCTS
{
    tensorflow::Net& net;
public:
    MCTS_DRL(tensorflow::Net& net): MCTS(), net(net) {}
    MCTS_DRL(const Othello& game, tensorflow::Net& net): MCTS(game), net(net) {}
    Node *expand(Node*) override;
    double simulate(Node*) override;
    int decide(int iteration = 5000, long tlimit = 0) override;
    //save the current status as training data
    void saveRoot(GameHistory&);
};

#endif
