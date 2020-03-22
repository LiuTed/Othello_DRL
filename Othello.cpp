#include "Othello.h"
#include <iostream>

const double priori[8][8] = {
    {3.49, 0.77, 2.72, 1.65, 1.65, 2.72, 0.77, 3.49},
    {0.77, 0.61, 1.00, 1.00, 1.00, 1.00, 0.61, 0.77},
    {2.72, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 2.72},
    {1.65, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.65},
    {1.65, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.65},
    {2.72, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 2.72},
    {0.77, 0.61, 1.00, 1.00, 1.00, 1.00, 0.61, 0.77},
    {3.49, 0.77, 2.72, 1.65, 1.65, 2.72, 0.77, 3.49}
};

bool Othello::valid(int r, int c, signed char color) const
{
    if(ref(r, c) != empty) return false;
    //for each direction
    for(int dr = -1; dr <= 1; dr++)
    {
        for(int dc = -1; dc <= 1; dc++)
        {
            if(dr == 0 && dc == 0) continue;
            int rr = r + dr, cc = c + dc, d = 1;
            //try to search the continuous sequence of enemy stones in that direction
            for(; inBoard(rr, cc); rr += dr, cc += dc, d++)
            {
                if(ref(rr, cc) != -color) break;
            }
            if(d > 1 && inBoard(rr, cc) && ref(rr, cc) == color) return true;
        }
    }
    return false;
}

void Othello::step(int r, int c)
{
    //for each direction
    for(int dr = -1; dr <= 1; dr++)
    {
        for(int dc = -1; dc <= 1; dc++)
        {
            if(dr == 0 && dc == 0) continue;
            //search in that direction
            int rr = r + dr, cc = c + dc, d = 1;
            for(; inBoard(rr, cc); rr += dr, cc += dc, d++)
            {
                if(ref(rr, cc) != -nextColor) break;
            }
            //if can reverse stones in that direction
            if(d > 1 && inBoard(rr, cc) && ref(rr, cc) == nextColor)
            {
                int dd;
                //reverse each direction
                for(rr = r + dr, cc = c + dc, dd = 1; dd < d; rr += dr, cc += dc, dd++)
                {
                    ref(rr, cc) = nextColor;
                }
                stones[nextColor + 1] += d - 1;
                stones[-nextColor + 1] -= d - 1;
            }
        }
    }
    ref(r, c) = nextColor;
    stones[nextColor + 1] ++;
    stones[1]--;
    //change players to move
    nextColor = -nextColor;
}

void Othello::display() const
{
    static const char* line = " -----------------\n";
    using std::cout;
    using std::endl;
    cout << "  0 1 2 3 4 5 6 7\n";
    cout << line;
    for(int r = 0; r < width; r++)
    {
        cout << r << '|';
        for(int c = 0; c < width; c++)
        {
            if(ref(r, c) != empty)
            {
                cout << (ref(r, c) == black ? '*' : 'o');
            }
            else if(valid(r, c, nextColor))
            {
                cout << '.';
            }
            else
            {
                cout << ' ';
            }
            cout << '|';
        }
        cout << endl << line;
    }
}

bool Othello::noValid() const
{
    for(int r = 0; r < width; r++)
    {
        for(int c = 0; c < width; c++)
        {
            if(valid(r, c, black) || valid(r, c, white)) return false;
        }
    }
    return true;
}
