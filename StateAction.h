#ifndef STATEACTION_H
#define STATEACTION_H

#include "BoardState.h"

class StateAction {

    public:
        BoardState *state;
        int action;

        StateAction(BoardState *s, Action a) : state(s), action(a) {};

        struct Pred {
            bool operator()(const StateAction& lhs, const StateAction& rhs) const;
        };
        struct Hash {
            std::size_t operator()(const StateAction& sa) const;
        };
};


#endif

