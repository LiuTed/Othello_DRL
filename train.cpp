#include "MCTS_DRL.h"
#include <algorithm>
#include <iostream>

#define bufSize 512u
GameHistory history[bufSize];
int indexBuf[bufSize];

int main(int argc, char **argv)
{
    using namespace tensorflow; 
    //load graph
    Net net(argv[1]);
    //restore/init graph weights
    if(argc > 3 || !net.restore(argv[2]).ok())
    {
       net.initGraph();
       std::cout << "init done\n";
    }
    unsigned int front = 0, back = 0;
    static const int batchSize = 32;
    for(int i = 0; i < bufSize; i++)
        indexBuf[i] = i;
    Othello game;
    for(int _ = 0; _ < 10000; _++)
    {
        MCTS_DRL mcts(game, net);
        unsigned int start = front;
        clock_t tstart = clock();
        //self-play
        while(!mcts.game().finish())
        {
            int act = mcts.decide(1000, CLOCKS_PER_SEC * 3);
            mcts.saveRoot(history[front++ % bufSize]);
            mcts.act(act);
        }
        clock_t tend = clock();
        int black = mcts.game().count(Othello::black),
            white = mcts.game().count(Othello::white);
        float reward = (black == white ? 0 : (black > white ? 1 : -1));
        std::cout << front - start << " steps takes "
                  << (float)(tend - tstart) / CLOCKS_PER_SEC
                  << " seconds with reward " << reward << "\n";

        for(unsigned int i = start; i < front; i++)
        {
            history[i % bufSize].reward = reward;
        }

        //randomly select games saved
        int range = std::min(front, bufSize);
        std::random_shuffle(indexBuf, indexBuf + range);
        //each game will be fed only once in average
        for(; back + batchSize <= range; back += batchSize)
        {
            Tensor boards(DT_FLOAT, TensorShape({batchSize, 8, 8, 8}));
            Tensor labels(DT_FLOAT, TensorShape({batchSize, 65}));
            Tensor rewards(DT_FLOAT, TensorShape({batchSize, 1}));
            for(int i = 0; i < batchSize; i++)
            {
                int idx = indexBuf[(i + back) % bufSize];
                history[idx].game.toTensor(boards.flat<float>().data()+i*8*8*8);
                memcpy(labels.flat<float>().data()+i*65, history[idx].labels, sizeof(history[idx].labels));
                rewards.flat<float>().data()[i] = history[idx].reward;
            }
            std::vector<Tensor> outputs;
            net.train(boards, labels, rewards, &outputs);
            float loss = outputs[0].scalar<float>()();
            int global_step = outputs[1].scalar<int>()();
            std::cout << global_step << ": " << loss << std::endl;
            if(global_step % 10 == 0)
            {
                net.save(argv[2]);
            }
        }
    }
}
