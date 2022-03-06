//
// Created by mind on 2022/2/27.
//

#ifndef CCC_DIAGNOSE_H
#define CCC_DIAGNOSE_H

#include <iostream>
#include <string_view>
#include <sstream>

namespace CCC {
    template<typename ...U>
    void diagnose(std::string_view line, const size_t n_line, const size_t start_pos, const size_t length, U ...u) {
        using std::cerr, std::endl;

        cerr << "source code:" << n_line << ':' << start_pos + 1
             << ": \033[1;31mError: \033[0m";
        ((cerr << u), ...) << endl;
        cerr << '\t' << n_line << " |\t"
             << line.substr(0, start_pos)
             << "\033[1;31m" << line.substr(start_pos, length) << "\033[0m"
             << line.substr(start_pos + length)
             << endl;

        // 定位
        cerr << "\t\t";
        for (int _ = 0; _ < start_pos; _++) {
            cerr << ' ';
        }
        cerr << "\033[1;31m" << '^' << "\033[0m" << endl;

        exit(1);
    }
}

#endif //CCC_DIAGNOSE_H
