//
// Created by kvtoD on 6/30/2025.
//

#ifndef ERRORS_H
#define ERRORS_H

    struct ComponentNotFoundException final : std::runtime_error {
        using std::runtime_error::runtime_error;
    };
#endif //ERRORS_H
