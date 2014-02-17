#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <random>
#include <utility>
#include <list>
#include <numeric>

#include "BoardState.h"
#include "StateAction.h"


#define PLAY_RANDOM
#undef PLAY_SELF

#define ROW_COUNT 6
#define COL_COUNT 7

#define SPLIT .99
#define LEFTOVER ((1.0 - SPLIT)/(COL_COUNT - 1))

#define WIN_WEIGHT 1.0
#define TIE_WEIGHT 0.0
#define LOSS_WEIGHT -1.0

#define GAMMA 0.5

std::unordered_map<StateAction, double, StateAction::Hash, StateAction::Pred> Q;
std::unordered_map<StateAction, int, StateAction::Hash, StateAction::Pred> Q_count;

void updateQ(const StateAction &sa, double ret) {
    if (Q.find(sa) == Q.end()) {
        Q[sa] = ret;
        Q_count[sa] = 1;
        return;
    }

    Q[sa] = (Q_count[sa] * Q[sa] + ret)/(Q_count[sa] + 1);
    Q_count[sa] = Q_count[sa]+1;
}

Action pick_random(std::vector<std::pair<Action, double> > &probs);

double random_start() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 0.1);
    return dis(gen);
}

Action Pi(BoardState &b) {
    int bestCol = -1;
    double bestWeight = -1;
    for (int i = 0; i < COL_COUNT; ++i) {
        StateAction sa(&b, i);
        double weight = (Q.find(sa) == Q.end()) ? random_start() : Q[sa];
        if (weight > bestWeight) {
            bestCol = i;
            bestWeight = weight;
        }
    }

    std::vector<std::pair<Action, double> > probs(COL_COUNT);
    for (int i = 0; i < COL_COUNT; ++i) {
        if (i == bestCol)
            probs[i] = std::pair<Action, double>(i, SPLIT);
        else
            probs[i] = std::pair<Action, double>(i, LEFTOVER);
    }

    return pick_random(probs);
}

Action random_action();

Action pick_random(std::vector<std::pair<Action, double> > &probs) {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    double roll = dis(gen);

    std::vector<std::pair<Action, double> >::reverse_iterator iter = probs.rbegin();
    double total_prob = 1.0;
    for(; iter != probs.rend(); ++iter) {
        total_prob -= iter->second;
        if (roll >= total_prob)
            return iter->first;
    }

    return probs[0].first;
}

Action random_action() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, COL_COUNT-1);
    return dis(gen);
}

struct Episode {
    std::list<StateAction> sas;
    enum Result result;
};

struct Episode* genEpisode() {
    struct Episode *e = new struct Episode;
    BoardState *b = new BoardState(ROW_COUNT, COL_COUNT);
    enum Result r = Invalid;
    int i = 0;
    for (;;) {
        Action p1Action = Pi(*b);
        BoardState *bn = b->move(p1Action, Player1, &r);

        while (r == Invalid) {
            delete bn;
            p1Action = Pi(*b);
            bn = b->move(p1Action, Player1, &r);
        }
        StateAction *sa = new StateAction(b, p1Action);
        e->sas.push_back(*sa);


        if (r == Win || r == Tie || r == Loss) {
            e->result = r;
            return e;
        }

        assert(r == Intermediate);

#ifdef PLAY_RANDOM
        Action p2Action = random_action();
#else
        Action p2Action = Pi(*bn);
#endif
        b = bn->move(p2Action, Player2, &r);

        while (r == Invalid) {
            delete b;
#ifdef PLAY_RANDOM
            Action p2Action = random_action();
#else
            Action p2Action = Pi(*bn);
#endif
            p2Action = random_action();
            b = bn->move(p2Action, Player2, &r);
        }
        delete bn;

        if (r == Win || r == Tie || r == Loss) {
            if (r == Win)
                e->result = Loss;
            else if (r == Loss)
                e->result = Win;
            else if (r == Tie)
                e->result = Tie;

            delete b;

            return e;
        }

        assert(r == Intermediate);

    }

    assert(e->result == Win || e->result == Tie || e->result == Loss);

    return NULL;
}

void updateQFromEpisode(struct Episode &e) {
    int i = 0;
    for(std::list<StateAction>::iterator it = e.sas.begin(); it != e.sas.end(); ++it) {
        double weight;
        if (e.result == Win)
            weight = WIN_WEIGHT;
        else if (e.result == Loss)
            weight = LOSS_WEIGHT;
        else if (e.result == Tie)
            weight = TIE_WEIGHT;

        double futureDiscountedReturn = weight * std::pow(GAMMA, i);
        updateQ(*it, futureDiscountedReturn);
        i++;
    }
}


int main() {
    int winCount = 0;

    for (int i = 0; i < 10000000; ++i) {
        struct Episode *e = genEpisode();
        if (e->result == Win)
            winCount++;

        updateQFromEpisode(*e);
        delete e;

        if (i >= 10000 && i % 10000 == 0) {
            std::cout << "Iteration: " << i;
            std::cout << "\tWin %: " << ((double)winCount/(double)(10000));
            std::cout << "\tState/Action Pairs Tracked: " << Q.size() << std::endl;
            winCount = 0;
        }
    }

    std::cout << "Done" << std::endl;
    std::cout << ((double)winCount/(double)(10000)) << std::endl;
}

