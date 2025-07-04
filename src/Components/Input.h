//
// Created by kvtoD on 6/30/2025.
//

#ifndef INPUT_H
#define INPUT_H

struct Input
{
    bool left;
    bool right;
    bool up;
    bool down;
    bool attack;
    bool attackOnce;
};

inline std::ostream &operator<<(std::ostream &os, const Input &input)
{
    os << "Input{left=" << input.left << ", right=" << input.right << ", up=" << input.up << ", down=" << input.down <<
        ", attack=" << input.attack << "}";
    return os;
}
#endif //INPUT_H
