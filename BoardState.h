#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <iostream>
#include <algorithm>
#include <string>

enum Player {
    Player1 = 1,
    Player2 = 2
};

enum Result {
    Invalid,
    Intermediate,
    Win,
    Loss,
    Tie
};

typedef int Action;


class BoardState {
    public:
        int rows, cols;
        int **board;

        BoardState (int, int);
        ~BoardState();

        BoardState(const BoardState& other) :
            rows(other.rows), cols(other.cols), board(new int*[other.rows])
        {
           for (int i = 0; i < this->rows; ++i) {
            this->board[i] = new int[this->cols];
            std::copy(other.board[i], other.board[i]+other.cols, this->board[i]);
           }
        }

        bool operator==(const BoardState &other) const;
        bool operator!=(const BoardState &other) const;

        static BoardState* initFromArray(int arr[] , int size, int cols);

        BoardState* move(Action a, enum Player p, enum Result *r);
        enum Result result(int changedRow, int changedCol, enum Player p);
};


const char* resultToString(enum Result r);

std::ostream& operator<<(std::ostream &strm, const BoardState &b);

#endif
