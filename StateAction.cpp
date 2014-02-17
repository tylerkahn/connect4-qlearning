#include "StateAction.h"

bool StateAction::Pred::operator()(const StateAction& lhs, const StateAction& rhs) const {
    return (lhs.action == rhs.action && *(lhs.state) == *(rhs.state));
}

size_t StateAction::Hash::operator()(const StateAction& sa) const {
    size_t s = 0;
    size_t a = sa.action;

    BoardState *b = sa.state;

    for (int i = 0; i < b->rows; i++) {
        for (int j = 0; j < b->cols; ++j) {
            int k = (i * b->cols + j) % 64;
            int m = (b->board[i][j] == Player1 || b->board[i][j] == 0) ? 0 : 1;
            size_t v = (m << k);
            s |= v;
        }
    }

    return (s << 4) + a;
}

