#include <chrono>
#include <random>
#include "DefaultAgentStrategy.h"

namespace Gaming {

    DefaultAgentStrategy::DefaultAgentStrategy() {

    }

    DefaultAgentStrategy::~DefaultAgentStrategy() {

    }

    ActionType DefaultAgentStrategy::operator()(const Surroundings &s) const {
        std::vector<int> positions;
        unsigned int seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine rnd(seed);

        // Check for Advantage
        for (int i = 0; i < 9; ++i) {
            //std::cout << "Surr of " << __id << ": i: " << i << ": " << s.array[i] << std::endl;
            if (s.array[i] == PieceType::ADVANTAGE) {
                //std::cout << "Pushing advantage" << std::endl;
                positions.push_back(i);
                //std::cout << "size of positions: " << positions.size() << std::endl;
            }
        }
        //  Check for Food
        if (positions.size() == 0) {
            for (int i = 0; i < 9; ++i) {
                //std::cout << "Surrounding[i]: " << s.array[i] << std::endl;
                if (s.array[i] == PieceType::FOOD) {
                    positions.push_back(i);
                    //std::cout << "Available position: " << i << std::endl;
                }
            }
        }
        //  Check for Empty
        if (positions.size() == 0) {
            for (int i = 0; i < 9; ++i) {
                //std::cout << "Surrounding[i]: " << s.array[i] << std::endl;
                if (s.array[i] == PieceType::EMPTY) {
                    positions.push_back(i);
                    //std::cout << "Available position: " << i << std::endl;
                }
            }
        }
        //  Check for Simple
        if (positions.size() == 0) {
            for (int i = 0; i < 9; ++i) {
                //std::cout << "Surrounding[i]: " << s.array[i] << std::endl;
                if (s.array[i] == PieceType::SIMPLE) {
                    positions.push_back(i);
                    //std::cout << "Available position: " << i << std::endl;
                }
            }
        }

        if (positions.size() > 0) {
            int posIndex = positions[rnd() % positions.size()];
            if (positions.size() == 1) posIndex = positions[0];

            //std::cout << "Chosen Position: " << posIndex << std::endl;
            ActionType ac;
            switch (posIndex) {
                case 0: ac = NW; break;
                case 1: ac = N; break;
                case 2: ac = NE; break;
                case 3: ac = W; break;
                case 4: ac = STAY; break;
                case 5: ac = E; break;
                case 6: ac = SW; break;
                case 7: ac = S; break;
                case 8: ac = SE; break;
                default: ac = STAY;
            }
            return (ac);
        }

        return ActionType::STAY;
    }

}