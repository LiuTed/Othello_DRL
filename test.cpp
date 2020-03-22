#include "MCTS_DRL.h"
#include <algorithm>
#include <iostream>

int main(int argc, char **argv)
{
    using namespace tensorflow; 
    Net net(argv[1]);
    net.restore(argv[2]);
    Othello game;

    //DRL black
    {
        MCTS_DRL drl(game, net);
        MCTS base(game);
        while(!drl.game().finish())
        {
            int act = drl.decide(3000, CLOCKS_PER_SEC * 3);
            drl.debug();
            base.debug();
            std::cout << '\n';
            drl.act(act);
            base.act(act);
            if(drl.game().finish()) break;
            act = base.decide(3000, CLOCKS_PER_SEC * 3);
            drl.debug();
            base.debug();
            std::cout << '\n';
            drl.act(act);
            base.act(act);
        }
        int black = drl.game().count(Othello::black),
            white = drl.game().count(Othello::white);
        drl.game().display();
        std::cout << "Game 1: drl(*): " << black << " base(o): " << white << std::endl;
    }
    //DRL white
    {
        MCTS_DRL drl(game, net);
        MCTS base(game);
        while(!drl.game().finish())
        {
            int act = base.decide(3000, CLOCKS_PER_SEC * 3);
            drl.act(act);
            base.act(act);
            if(drl.game().finish()) break;
            act = drl.decide(3000, CLOCKS_PER_SEC * 3);
            drl.act(act);
            base.act(act);
        }
        int black = drl.game().count(Othello::black),
            white = drl.game().count(Othello::white);
        drl.game().display();
        std::cout << "Game 2: base(*): " << black << " drl(o): " << white << std::endl;
    }
}
