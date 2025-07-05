//
// Created by root on 7/5/25.
//

#ifndef REGISTERABLE_H
#define REGISTERABLE_H

template<typename registerFunctor>
class Registerable {
public:
    explicit Registerable()
    {
        static bool registered = registerFunctor::_register();
    }

};

#endif //REGISTERABLE_H
