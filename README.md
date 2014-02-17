Connect 4 Q Reinforcment Learning Solver
-------------------

A Connect 4 AI that learns the optimal moves to play via [Q
Learning](http://en.wikipedia.org/wiki/Q-learning).


To compile:

    g++ BoardState.cpp StateAction.cpp main.cpp -o connect4

To run:

    ./connect5

The program will output something that looks like this:

    Iteration: 10000        Win %: 0.6812   State/Action Pairs Tracked: 58263
    Iteration: 20000        Win %: 0.7395   State/Action Pairs Tracked: 102588
    Iteration: 30000        Win %: 0.7767   State/Action Pairs Tracked: 140458
    \[...\]()

Output is written to stdout every 10,000 episodes (completed games of Connect 4). The
win percentage reflects the number of wins/losses for the past 10,000
games. The total number of state/action pairs tracked at that point is also recorded.

### Concepts

A state is a configuration of the board (which positions are filled with which player's chips or not filled at all).

An action is the index of the column that the AI placed its chip in at that state. At most 7 actions are possible at any state (7 columns in Connect 4, some may be full).

Actions on states lead to new states.


**The action taken at a state is chosen as follows**:

Of the 7 actions for a given state, the action that is
historically the best (leads most often to a win) is given a probabilty of 0.8, the rest are each given 0.2/6. The action taken is selected randomly according to this probability mass function.

A random factor is added to allow exploration of new moves which is
necessary when few state/action pairs have been seen and in the later
stages of a game when it is less likely that a particular board state has
been seen before.
