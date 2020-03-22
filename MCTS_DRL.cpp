#include "MCTS_DRL.h"
#include <random>
#include <chrono>

MCTS::Node* MCTS_DRL::expand(Node* p)
{
    using tensorflow::Tensor;
    if(p->status.finish()) return p;
#ifdef _BOTZONE_ONLINE
    if(p->memSize >= 192 << 20) return p;
#endif
    //convert game board to tensor
    Tensor input(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, 8, 8, 8}));
    std::vector<Tensor> outputs;
    p->status.toTensor(input.flat<float>().data());
    //evaluate network
    net.inference(input, &outputs);
    
    int color = p->status.currentPlayer();
    double expsum = 0;
    double tmp[64];
    int idx[64];
    int ptr = 0;
    for(int r = 0; r < p->status.width; r++)
    {
        for(int c = 0; c < p->status.width; c++)
        {
            //for each valid move, prepare to expand a child node
            if(p->status.valid(r, c, color))
            {
                double priori = outputs[0].flat<float>()(r * 8 + c);
                tmp[ptr] = priori;
                expsum += priori;
                idx[ptr++] = r * 8 + c;
            }
        }
    }
    p->value = outputs[1].scalar<float>()();
    //no valid move yet game not finished, pass this turn
    if(ptr == 0)
    {
        Node *chld = new Node(p->status);
        chld->parent = p;
        chld->status.skip();
        p->childs[-1] = std::make_pair(chld, 1);
        chld->memSize += sizeof(decltype(p->childs)::value_type);
        return p;
    }
    //create each child node
    for(int i = 0; i < ptr; i++)
    {
        Node *chld = new Node(p->status);
        chld->parent = p;
        chld->status.step(idx[i] / 8, idx[i] % 8);
        p->childs[idx[i]] = std::make_pair(chld, tmp[i] / expsum);
        chld->memSize += sizeof(decltype(p->childs)::value_type);
    }
    return p;
}

double MCTS_DRL::simulate(Node* p)
{
    if(p->status.finish())
    {
        int black = p->status.count(Othello::black), white = p->status.count(Othello::white);
        return black == white ? 0 :
                    (black > white ? Othello::black : Othello::white);
    }
    else
    {
        return p->value;
    }
}

int MCTS_DRL::decide(int iteration, long tlimit)
{
    clock_t ts = clock();
    while(iteration-- && (tlimit == 0 || clock() - ts < tlimit))
    {
        iterate();
    }
    int action = -1;
    double maxi = -100; 
    for(auto i: root->childs)
    {
        Node *p = i.second.first;
        //find node with best value
        //as the visit count is relatively small
        if(p->reward * root->status.currentPlayer() / p->count > maxi)
        {
            maxi = p->reward * root->status.currentPlayer() / p->count;
            action = i.first;
        }
    }
    return action;
}

void MCTS_DRL::saveRoot(GameHistory& hist)
{
    static std::default_random_engine def_gen(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<int> dist(0, 7);
    static auto dice = std::bind(dist, def_gen);
    int aug = dice();
    //save status
    hist.game = root->status;
    //save visit probability
    memset(hist.labels, 0, sizeof(hist.labels));
    for(auto i: root->childs)
    {
        if(i.first == -1)
        {
            hist.labels[64] = (float)i.second.first->count / root->count;
        }
        else
        {
            hist.labels[i.first] = (float)i.second.first->count / root->count;
        }
    }
    //randomly augmentation
    hist.game.augment(aug);
    Othello::augment(aug, hist.labels);
}
