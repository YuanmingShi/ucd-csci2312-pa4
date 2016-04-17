#include <sstream>
#include <iomanip>
#include "Strategic.h"

namespace Gaming {

    const char Strategic::STRATEGIC_ID = 'T';

    Strategic::Strategic(const Game &g, const Position &p, double energy, Strategy *s)
            : Agent(g, p, energy) {
        __strategy = s;
    }

    Strategic::~Strategic() {
        delete __strategy;
    }

    void Strategic::print(std::ostream &os) const {
        std::string str;
        //str += Simple::SIMPLE_ID;
        str = std::to_string(__id);

        //os << '[';
        std::stringstream ss;
        ss << Strategic::STRATEGIC_ID;
        ss << str;
        /*for (int i = 0; i < (4 - str.length()); ++i)
            ss << ' ';*/
        std::getline(ss, str);
        //os << ss.rdbuf(); //<< ']';
        for (int i = 0; i < str.length(); ++i) {
            os << str[i];
        }
    }

    ActionType Strategic::takeTurn(const Surroundings &s) const {
        return (*__strategy)(s);
    }
}