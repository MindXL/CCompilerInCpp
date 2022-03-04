//
// Created by mind on 2022/2/27.
//

#ifndef CCC_DIAGNOSE_H
#define CCC_DIAGNOSE_H

#include <iostream>
#include <string_view>
#include <sstream>

namespace CCC {
//    std::string_view fetchLine(std::string_view &source, std::string_view::const_iterator &line_head) {
//        const long line_start = std::distance(source.begin(), line_head);
//        return source.substr(line_start, source.find('\n', line_start));
//    }

    template<typename ...U>
    void diagnose(std::string_view &source, std::string_view::const_iterator &line_head,
                  const size_t line_num, const size_t col_num,
                  const size_t length, U ...u) {
        using std::cerr, std::endl;

//        auto line = fetchLine(source, line_head);
        const long line_start = std::distance(source.begin(), line_head);
        auto line = source.substr(line_start, source.find('\n', line_start));

        cerr << "source code:" << line_num + 1 << ':' << col_num + 1
             << ": \033[1;31mError: \033[0m";
        ((cerr << u), ...) << endl;
        cerr << '\t' << line_num << " |\t"
             << line.substr(0, col_num)
             << "\033[1;31m" << line.substr(col_num, length) << "\033[0m"
             << line.substr(col_num + length)
             << endl;

        // 定位
        cerr << "\t\t";
        for (int _ = 0; _ < col_num; _++) {
            cerr << ' ';
        }
        cerr << "\033[1;31m" << '^' << "\033[0m" << endl;

        exit(1);
    }
}

#endif //CCC_DIAGNOSE_H
