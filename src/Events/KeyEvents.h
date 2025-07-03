//
// Created by kvtoD on 6/30/2025.
//

#ifndef KEYEVENTS_H
#define KEYEVENTS_H
#include <qnamespace.h>

// using Qt::Key;
typedef Qt::Key Key;
struct OnKey {
    Key key;
    // Constructor that auto-converts from Qt key
};

struct AtKey {
    Key key;
    // Constructor that auto-converts from Qt key
};

#endif //KEYEVENTS_H