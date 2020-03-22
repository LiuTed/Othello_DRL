#include "Othello.h"
#include "MCTS.h"
#ifndef DISABLE_DRL
#include "MCTS_DRL.h"
#endif
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

#include "jsoncpp/json.h"

void usage(const char *name)
{
    printf(
        "Usage: %s [-bwhs] [-v]\n"
        "\t-h: show this message and exit\n"
        "\t-b: player choose black stone (default)\n"
        "\t-w: player choose white stone\n"
        "\t-s: self play\n"
        "\t-v: verbose AI message\n",
        name
    );
    exit(0);
}

int main(int argc, char **argv)
{
#ifdef _BOTZONE_ONLINE
    Othello game;
    std::string str;
    std::getline(std::cin, str);
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    //start to parse history input
    int len = input["responses"].size();
    int x = input["requests"][0]["x"].asInt(),
        y = input["requests"][0]["y"].asInt();
    if(x >= 0)
        game.step(y, x);
    for(int i = 0; i < len; i++)
    {
        auto resp = input["responses"][i];
        x = resp["x"].asInt();
        y = resp["y"].asInt();
        if(x >= 0)
            game.step(y, x);
        else
            game.skip();

        auto req = input["requests"][i + 1];
        x = req["x"].asInt();
        y = req["y"].asInt();
        if(x >= 0)
            game.step(y, x);
        else
            game.skip();
    }
    //create MCTS tree
#ifdef DISABLE_DRL
    MCTS mcts(game);
    bool flag = true;
#else
    tensorflow::Net net("./data/graph.pb");
    net.restore("./data/ckpt");
    MCTS_DRL mcts(game, net);
    bool flag = false;
#endif
    while(!mcts.game().finish())
    {
        //the time limit is doubled at first turn on botzone
        int coeff = (flag ? 2 : 1);
        flag = false;
        //decide next move
        int act = mcts.decide(100000, CLOCKS_PER_SEC / 100 * 85 * coeff);
        Json::Value ret;
        if(act == -1)
        {
            ret["response"]["x"] = -1;
            ret["response"]["y"] = -1;
        }
        else
        {
            ret["response"]["x"] = act % 8;
            ret["response"]["y"] = act / 8;
        }
        mcts.act(act);
        double reward = mcts.curReward();
        ret["debug"] = reward;
        Json::FastWriter writer;
        std::cout << writer.write(ret) << std::endl;
        std::cout << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << std::endl << std::flush;
        //get enemy's move
        std::getline(std::cin, str);
        reader.parse(str, input);
        x = input["x"].asInt(), y = input["y"].asInt();
        if(x >= 0)
        {
            mcts.act(y * 8 + x);
        }
        else
        {
            mcts.act(-1);
        }
    }
#else
    char c;
    int verbose = 0;
    int playerColor = Othello::black;
    for(int i = 1; i < argc; i++)
    {
        c = argv[i][1];
        switch(c)
        {
            case 'h':
                usage(argv[0]);
                break;
            case 'b':
                playerColor = Othello::black;
                break;
            case 'w':
                playerColor = Othello::white;
                break;
            case 's':
                playerColor = Othello::empty;
                break;
            case 'v':
                verbose++;
                break;
            default:
                usage(argv[0]);
                break;
        }
    }
    MCTS mcts;
    mcts.game().display();
    while(!mcts.game().finish())
    {
        //player's turn
        if(mcts.game().currentPlayer() == playerColor)
        {
            int r, c;
            double r1, r2;
            bool hasValid = false;
            for(int i = 0; i < 64; i++)
            {
                if(mcts.game().valid(i / 8, i % 8, playerColor))
                {
                    hasValid = true;
                    break;
                }
            }
            if(!hasValid)
            {
                printf("No valid move, pass\n");
                mcts.act(-1);
            }
            else
            {
                printf("Your turn, input row and column\n");
retry:
                std::cin >> r >> c;
                if(!Othello::inBoard(r, c) || !mcts.game().valid(r, c, playerColor))
                {
                    printf("Invalid move, please retry\n");
                    goto retry;
                }
                r1 = mcts.curReward();
                mcts.act(r * 8 + c);
                r2 = mcts.curReward();
                if(verbose)
                {
                    printf("reward: %lf -> %lf\n", r1, r2);
                }
            }
        }
        //AI's turn
        else
        {
            double r1, r2;
            int act = mcts.decide(100000, CLOCKS_PER_SEC * 3);
            if(act == -1)
                printf("action: skip\n");
            else
                printf("action: %d %d\n", act / 8, act % 8);
            r1 = mcts.curReward();
            mcts.act(act);
            r2 = mcts.curReward();
            if(verbose)
            {
                printf("reward: %lf -> %lf\n", r1, r2);
            }
        }
        mcts.game().display();
    }
    printf("black: %d, white: %d\n",
        mcts.game().count(Othello::black),
        mcts.game().count(Othello::white)
    );
#endif
}
