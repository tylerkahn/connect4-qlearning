#include "BoardState.h"

const char* resultToString(enum Result r) {
    switch (r) {
        case Invalid: return "Invalid";
        case Intermediate: return "Intermediate";
        case Win: return "Win";
        case Loss: return "Loss";
        case Tie: return "Tie";
    }
    return NULL;
}


static bool validRowCol(int rows, int cols, int r, int c) {
    return (r < rows && r >= 0) && (c < cols && c >= 0);
}
static bool match(int **board, int r, int c, int p) {
    return board[r][c] == p;
}

enum Result BoardState::result(int changedRow, int changedCol, enum Player p) {
    // Tie
    bool filled = true;
    for (int i = 0; i < this->cols; ++i) {
        filled = filled && (this->board[0][i] == 1 || this->board[0][i] == 2);
    }
    if (filled) return Tie;

    bool end = false;
    int t = this->board[changedRow][changedCol];

    // vertical
    for (int i = 0; i < 4; ++i) {
        int rows[] = {changedRow+0-i, changedRow+1-i, changedRow+2-i, changedRow+3-i};
        end = end ||
              ((validRowCol(this->rows, this->cols, rows[0], changedCol)
                && match(this->board, rows[0], changedCol, p)) &&
              (validRowCol(this->rows, this->cols, rows[1], changedCol)
                && match(this->board, rows[1], changedCol, p)) &&
              (validRowCol(this->rows, this->cols, rows[2], changedCol)
                && match(this->board, rows[2], changedCol, p)) &&
              (validRowCol(this->rows, this->cols, rows[3], changedCol)
                && match(this->board, rows[3], changedCol, p)));
    }

    //horizontal
    for (int i = 0; i < 4; ++i) {
        int cols[] = {changedCol+0-i, changedCol+1-i, changedCol+2-i, changedCol+3-i};
        end = end ||
              ((validRowCol(this->rows, this->cols, changedRow, cols[0])
                && match(this->board, changedRow, cols[0], p)) &&
              (validRowCol(this->rows, this->cols, changedRow, cols[1])
                && match(this->board, changedRow, cols[1], p)) &&
              (validRowCol(this->rows, this->cols, changedRow, cols[2])
                && match(this->board, changedRow, cols[2], p)) &&
              (validRowCol(this->rows, this->cols, changedRow, cols[3])
                && match(this->board, changedRow, cols[3], p)));
    }


    // diagonal \
    // 
    for (int i = 0; i < 4; ++i) {
        int cols[] = {changedCol+0-i, changedCol+1-i, changedCol+2-i, changedCol+3-i};
        int rows[] = {changedRow+0-i, changedRow+1-i, changedRow+2-i, changedRow+3-i};

        end = end ||
              ((validRowCol(this->rows, this->cols, rows[0], cols[0])
                && match(this->board, rows[0], cols[0], p)) &&
              (validRowCol(this->rows, this->cols, rows[1], cols[1])
                && match(this->board, rows[1], cols[1], p)) &&
              (validRowCol(this->rows, this->cols, rows[2], cols[2])
                && match(this->board, rows[2], cols[2], p)) &&
              (validRowCol(this->rows, this->cols, rows[3], cols[3])
                && match(this->board, rows[3], cols[3], p)));

    }

    // diagonal /
    for (int i = 0; i < 4; ++i) {
        int cols[] = {changedCol-0+i, changedCol-1+i, changedCol-2+i, changedCol-3+i};
        int rows[] = {changedRow+0-i, changedRow+1-i, changedRow+2-i, changedRow+3-i};

        end = end ||
              ((validRowCol(this->rows, this->cols, rows[0], cols[0])
                && match(this->board, rows[0], cols[0], p)) &&
              (validRowCol(this->rows, this->cols, rows[1], cols[1])
                && match(this->board, rows[1], cols[1], p)) &&
              (validRowCol(this->rows, this->cols, rows[2], cols[2])
                && match(this->board, rows[2], cols[2], p)) &&
              (validRowCol(this->rows, this->cols, rows[3], cols[3])
                && match(this->board, rows[3], cols[3], p)));

    }


    if (end) {
        return (t == p) ? Win : Loss;
    }
    return Intermediate;
}


BoardState* BoardState::move(Action a, enum Player p, enum Result *r) {
    int changedRow = -1;
    if (this->board[0][a] != 0) { // empty
        *r = Invalid;
    }

    BoardState *newState = new BoardState(*this);

    for (int i = newState->rows-1; i >= 0; --i) {
        if (newState->board[i][a] == 0) {
            newState->board[i][a] = p;
            changedRow = i;
            *r = newState->result(changedRow, a, p);
            break;
        }
    }
    return newState;
}

BoardState::BoardState(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;

    this->board = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        this->board[i] = new int[cols];
        for (int j = 0; j < this->cols; ++j) {
            this->board[i][j] = 0;
        }
    }
}

BoardState *BoardState::initFromArray(int arr[], int size, int cols) {
    int rows = size/cols;
    BoardState *b = new BoardState(rows, cols);
    for (int i = 0; i < size; i++) {
        int r = (i / rows);
        int c = i % cols;
        if (arr[i] == 1 || arr[i] == 2) {
            b->board[r][c] = arr[i];
        }
    }
    return b;
}

bool BoardState::operator==(const BoardState &other) const {
    if (this->rows != other.rows)
        return false;
    if (this->cols != other.cols)
        return false;

    for (int i = this->rows-1; i >= 0; i--) {
        if (std::memcmp(this->board[i], other.board[i], this->cols) != 0)
            return false;
    }

    return true;
}

bool BoardState::operator!=(const BoardState &other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream &strm, const BoardState &b) {
    for (int i = 0; i < b.rows; ++i) {
        for (int j = 0; j < b.cols; ++j) {
            if (b.board[i][j] == 0)
                strm << '_' << " ";
            else
                strm << b.board[i][j] << " ";
        }
        strm << std::endl;
    }
    return strm;
}

BoardState::~BoardState() {
    for (int i = 0; i < this->rows; i++) {
        delete[] this->board[i];
    }
    delete[] this->board;
}
