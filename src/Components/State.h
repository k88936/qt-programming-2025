//
// Created by root on 7/3/25.
//

#ifndef STATE_H
#define STATE_H
enum state
{
    Idle,Moving,Jump,Falling,Attack,Dead, Hurt
};
struct State
{
    state currentState = Idle;
};
#endif //STATE_H
