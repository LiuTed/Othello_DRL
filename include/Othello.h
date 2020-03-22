#ifndef __OTHELLO_H__
#define __OTHELLO_H__
#include <cstring>
#include <iostream>
/*
------------>c
|
|
|
|
v
r
*/
class Othello final
{
public:
    static const int width = 8;
    static const signed char empty = 0, black = 1, white = -1;
protected:
    signed char board[width * width];//r*c
    int nextColor;
    int stones[3];
    signed char& ref (int r, int c) {return board[r * width + c];}
    //if no one can play next move?
    bool noValid() const;
public:
    const signed char& ref (int r, int c) const {return board[r * width + c];}
    Othello(): nextColor(black), stones{2, 60, 2}
    {
        memset(board, 0, sizeof(board));
        ref(3, 3) = ref(4, 4) = white;
        ref(3, 4) = ref(4, 3) = black;
    }
    Othello(const Othello& rhs)
    {
        memcpy(board, rhs.board, sizeof(board));
        nextColor = rhs.nextColor;
        memcpy(stones, rhs.stones, sizeof(stones));
    }
    Othello& operator = (const Othello& rhs)
    {
        memcpy(board, rhs.board, sizeof(board));
        nextColor = rhs.nextColor;
        memcpy(stones, rhs.stones, sizeof(stones));
        return *this;
    }
    void init()
    {
        memset(board, 0, sizeof(board));
        ref(3, 3) = ref(4, 4) = white;
        ref(3, 4) = ref(4, 3) = black;
        nextColor = black;
        stones[0] = stones[2] = 2;
        stones[1] = 60;
    }
    //if this game finished
    bool finish() const
    {
        return stones[0] == 0 ||
               stones[2] == 0 ||
               stones[1] <= 0 ||
               noValid();
    }
    int currentPlayer() const
    {
        return nextColor;
    }
    //range check
    static bool inBoard(int r, int c)
    {
        return r >= 0 && r < width && c >= 0 && c < width;
    }
    bool valid(int r, int c, signed char color) const;
    //next player put a stone
    void step(int r, int c);
    //no valid move, pass this turn
    void skip()
    {
        nextColor = -nextColor;
    }
    int count(signed char color) const
    {
        return stones[color + 1];
    }
    void display() const;

    //toTensor: convert game board to Tensor that can be fed into network
    //board->int[8,8,8]
    //height/width: 8
    //channel: 8: [empty, black, white, blackValid, whiteValid, nextIsBlack, nextIsWhite, 1]
    template<typename T>
    void toTensor(T* dst)
    {
        memset(dst, 0, sizeof(T)*8*8*8);
        for(int r = 0; r < width; r++)
        {
            for(int c = 0; c < width; c++)
            {
                T* b = dst + (r * width + c) * 8;
                switch(ref(r, c))
                {
                    case empty: b[0] = 1; break;
                    case black: b[1] = 1; break;
                    case white: b[2] = 1; break;
                    default: break;
                }
                if(valid(r, c, black)) b[3] = 1;
                else if(valid(r, c, white)) b[4] = 1;
                if(nextColor == black) b[5] = 1;
                else b[6] = 1;
                b[7] = 1;
            }
        }
    }

    // augment:
    // 0: no action; 1: mirror r;
    // 2: mirror c; 3: mirror r, c;
    // 4: clockwise 90; 5: clockwise 270;
    // 6: mirror r + 90; 7: mirror c + 90
    void augment(int method)
    {
        Othello tmp;
        tmp.nextColor = nextColor;
        memcpy(tmp.stones, stones, sizeof(stones));
        for(int r = 0; r < width; r++)
        {
            for(int c = 0; c < width; c++)
            {
                switch(method)
                {
                    case 0: tmp.ref(r, c) = ref(r, c); break;
                    case 1: tmp.ref(r, c) = ref(width-1-r, c); break;
                    case 2: tmp.ref(r, c) = ref(r, width-1-c); break;
                    case 3: tmp.ref(r, c) = ref(width-1-r, width-1-c); break;
                    case 4: tmp.ref(r, c) = ref(c, width-1-r); break;
                    case 5: tmp.ref(r, c) = ref(width-1-c, r); break;
                    case 6: tmp.ref(r, c) = ref(c, r); break;
                    case 7: tmp.ref(r, c) = ref(width-1-c, width-1-r); break;
                }
            }
        }
        *this = tmp;
    }
    template<typename T>
    static void augment(int method, T* buf)
    {
        T tmp[width][width];
        for(int r = 0; r < width; r++)
        {
            for(int c = 0; c < width; c++)
            {
                switch(method)
                {
                    case 0: tmp[r][c] = buf[r*8+c]; break;
                    case 1: tmp[r][c] = buf[(width-1-r)*8+c]; break;
                    case 2: tmp[r][c] = buf[r*8+(width-1-c)]; break;
                    case 3: tmp[r][c] = buf[(width-1-r)*8+(width-1-c)]; break;
                    case 4: tmp[r][c] = buf[c*8+(width-1-r)]; break;
                    case 5: tmp[r][c] = buf[(width-1-c)*8+r]; break;
                    case 6: tmp[r][c] = buf[c*8+r]; break;
                    case 7: tmp[r][c] = buf[(width-1-c)*8+(width-1-r)]; break;
                }
            }
        }
        memcpy(buf, tmp, sizeof(T)*width*width);
    }
};

//pre-defined priori probability of move at each position
extern const double priori[8][8];

#endif
