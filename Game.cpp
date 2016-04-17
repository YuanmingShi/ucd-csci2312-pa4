#include <sstream>
#include <iomanip>
#include <set>
#include "Game.h"
#include "Simple.h"
#include "Strategic.h"
#include "Food.h"
#include "Advantage.h"

namespace Gaming {

    const unsigned int Game::NUM_INIT_AGENT_FACTOR = 4;
    const unsigned int Game::NUM_INIT_RESOURCE_FACTOR = 2;
    const unsigned Game::MIN_WIDTH = 3;
    const unsigned Game::MIN_HEIGHT = 3;
    const double Game::STARTING_AGENT_ENERGY = 20;
    const double Game::STARTING_RESOURCE_CAPACITY = 10;

    PositionRandomizer Game::__posRandomizer = PositionRandomizer();

    // PRIVATE
    void Game::populate() { // populate the grid (used in automatic random initialization of a Game)
        std::default_random_engine gen;
        std::uniform_int_distribution<int> d(0, __width * __height);

        __numInitAgents = (__width * __height) / NUM_INIT_AGENT_FACTOR;
        __numInitResources = (__width * __height) / NUM_INIT_RESOURCE_FACTOR;
        unsigned int numStrategic = __numInitAgents / 2;
        unsigned int numSimple = __numInitAgents - numStrategic;
        unsigned int numAdvantages = __numInitResources / 4;
        unsigned int numFoods = __numInitResources - numAdvantages;

        while (numStrategic > 0) {
            int i = d(gen);
            if (i != (__width * __height) && __grid[i] == nullptr) {
                Position pos(i / __width, i % __width);
                __grid[i] = new Strategic(*this, pos, STARTING_AGENT_ENERGY);
                numStrategic--;
            }
        }

        while (numSimple > 0) {
            int i = d(gen);
            if (i != (__width * __height) && __grid[i] == nullptr) {
                Position pos(i / __width, i % __width);
                __grid[i] = new Simple(*this, pos, STARTING_AGENT_ENERGY);
                numSimple--;
            }
        }

        while (numFoods > 0) {
            int i = d(gen);
            if (i != (__width * __height) && __grid[i] == nullptr) {
                Position pos(i / __width, i % __width);
                __grid[i] = new Food(*this, pos, STARTING_RESOURCE_CAPACITY);
                numFoods--;
            }
        }

        while (numAdvantages > 0) {
            int i = d(gen);
            if (i != (__width * __height) && __grid[i] == nullptr) {
                Position pos(i / __width, i % __width);
                __grid[i] = new Advantage(*this, pos, STARTING_RESOURCE_CAPACITY);
                numAdvantages--;
            }
        }
    }

    //PUBLIC
    //Constructors / Destructor
    Game::Game() : __width(3), __height(3) {
        for (unsigned i = 0; i < (__width * __height); ++i) {
            __grid.push_back(nullptr);
        }
        __status = NOT_STARTED;
        __verbose = false;
        __round = 0;
    }

    Game::Game(unsigned width, unsigned height, bool manual) : __width(width), __height(height) { // note: manual population by default
        if (width < MIN_WIDTH || height < MIN_HEIGHT) {
            throw InsufficientDimensionsEx(MIN_WIDTH, MIN_HEIGHT, width, height);
        }

        __status = NOT_STARTED;
        __verbose = false;
        __round = 0;

        for (unsigned i = 0; i < (__width * __height); ++i) {
            __grid.push_back(nullptr);
        }

        if (!manual) {
            populate();
        }
    }

    Game::~Game() {
        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            if (*it != nullptr) {
                delete *it;
            }
        }
    }

    // Accessors
    unsigned int Game::getNumPieces() const {
        unsigned int numPieces = 0;
        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            if (*it != nullptr)
                numPieces++;
        }

        return numPieces;
    }

    unsigned int Game::getNumAgents() const {
        unsigned int numAgents = 0;

        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            Agent *simple = dynamic_cast<Agent*>(*it);
            if (simple) numAgents++;
        }

        return numAgents;
    }

    unsigned int Game::getNumSimple() const {
        unsigned int numAgents = 0;

        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            Simple *simple = dynamic_cast<Simple*>(*it);
            if (simple) numAgents ++;
        }

        return numAgents;
    }

    unsigned int Game::getNumStrategic() const {
        unsigned int numAgents = 0;

        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            Strategic *simple = dynamic_cast<Strategic*>(*it);
            if (simple) numAgents ++;
        }

        return numAgents;
    }

    unsigned int Game::getNumResources() const {
        unsigned int numAgents = 0;

        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            Resource *simple = dynamic_cast<Resource*>(*it);
            if (simple) numAgents++;
        }

        return numAgents;
    }

    const Piece *Game::getPiece(unsigned int x, unsigned int y) const {
        if (y >= __width || x >= __height) throw OutOfBoundsEx(__width, __height, x, y);
        if (__grid[y + (x * __width)] == nullptr) throw PositionEmptyEx(x, y);
        return __grid[y + (x * __width)];
    }

    // grid population methods
    void Game::addSimple(const Position &position) {
        int index = position.y + (position.x * __width);
        if (position.y >= __width || position.x >= __height) throw OutOfBoundsEx(__width, __height, position.x, position.y);
        if (__grid[index]) throw PositionNonemptyEx(position.x, position.y);

        __grid[index] = new Simple(*this, position, STARTING_AGENT_ENERGY);
    }

    void Game::addSimple(const Position &position, double energy) { // used for testing only
        int index = position.y + (position.x * __width);
        if (position.y >= __width || position.x >= __height) throw OutOfBoundsEx(__width, __height, position.x, position.y);
        if (__grid[index]) throw PositionNonemptyEx(position.x, position.y);

        __grid[index] = new Simple(*this, position, energy);
    }

    void Game::addSimple(unsigned x, unsigned y) {
        int index = y + (x * __width);
        if (y >= __width || x >= __height) throw OutOfBoundsEx(__width, __height, x, y);
        if (__grid[index]) throw PositionNonemptyEx(x, y);

        __grid[index] = new Simple(*this, Position(x, y), STARTING_AGENT_ENERGY);
    }

    void Game::addSimple(unsigned y, unsigned x, double energy) {
        int index = y + (x * __width);
        if (y >= __width || x >= __height) throw OutOfBoundsEx(__width, __height, x, y);
        if (__grid[index]) throw PositionNonemptyEx(x, y);

        __grid[index] = new Simple(*this, Position(x, y), energy);
    }

    void Game::addStrategic(const Position &position, Strategy *s) {
        int index = position.y + (position.x * __width);
        if (position.y >= __width || position.x >= __height) throw OutOfBoundsEx(__width, __height, position.x, position.y);
        if (__grid[index]) throw PositionNonemptyEx(position.x, position.y);

        __grid[index] = new Strategic(*this, position, STARTING_AGENT_ENERGY, s);
    }

    void Game::addStrategic(unsigned x, unsigned y, Strategy *s) {
        int index = y + (x * __width);
        if (y >= __width || x >= __height) throw OutOfBoundsEx(__width, __height, x, y);
        if (__grid[index]) throw PositionNonemptyEx(x, y);

        __grid[index] = new Strategic(*this, Position(x, y), STARTING_AGENT_ENERGY, s);
    }

    void Game::addFood(const Position &position) {
        int index = position.y + (position.x * __width);
        if (position.y >= __width || position.x >= __height) throw OutOfBoundsEx(__width, __height, position.x, position.y);
        if (__grid[index]) throw PositionNonemptyEx(position.x, position.y);

        __grid[index] = new Food(*this, position, STARTING_RESOURCE_CAPACITY);
    }

    void Game::addFood(unsigned x, unsigned y) {
        int index = y + (x * __width);
        if (y >= __width || x >= __height) throw OutOfBoundsEx(__width, __height, x, y);
        if (__grid[index]) throw PositionNonemptyEx(x, y);

        __grid[index] = new Food(*this, Position(x, y), STARTING_RESOURCE_CAPACITY);
    }

    void Game::addAdvantage(const Position &position) {
        int index = position.y + (position.x * __width);
        if (position.y >= __width || position.x >= __height) throw OutOfBoundsEx(__width, __height, position.x, position.y);
        if (__grid[index]) throw PositionNonemptyEx(position.x, position.y);

        __grid[index] = new Advantage(*this, position, STARTING_RESOURCE_CAPACITY);
    }

    void Game::addAdvantage(unsigned x, unsigned y) {
        int index = y + (x * __width);
        if (y >= __width || x >= __height) throw OutOfBoundsEx(__width, __height, x, y);
        if (__grid[index]) throw PositionNonemptyEx(x, y);

        __grid[index] = new Advantage(*this, Position(x, y), STARTING_RESOURCE_CAPACITY);
    }

    const Surroundings Game::getSurroundings(const Position &pos) const {
        //std::cout << "Getting surroundings..." << std::endl;
        Surroundings sur;
        for (int i = 0; i < 9; ++i) {
            sur.array[i] = EMPTY;
        }
        for (int row = -1; row <= 1; ++row) {
            for (int col = -1; col <= 1; ++col) {
                if (pos.x + row >= 0 && pos.x + row < __height
                    && pos.y + col >= 0 && pos.y + col < __width) {
                    // In bounds
                    unsigned int index = pos.y + col + ((pos.x + row) * __width);
                    //Piece *piece = __grid[pos.y + y + ((pos.x + x) * __width)];
                    if (__grid[index])
                        sur.array[col + 1 + ((row + 1) * 3)] = __grid[index]->getType();
                }
                else {
                    // Out of bounds
                    sur.array[col + 1 + ((row + 1) * 3)] = INACCESSIBLE;
                }
            }
        }
        sur.array[4] = SELF;

        return sur;
    }

    // gameplay methods
    const ActionType Game::reachSurroundings(const Position &from, const Position &to) { // note: STAY by default
        int x;
        int y;
        x = to.x - from.x;
        y = to.y - from.y;
        x++;
        y++;
        unsigned int index = (y + (x * 3));
        switch (index) {
            case 0: return NW;
            case 1: return N;
            case 2: return NE;
            case 3: return W;
            case 4: return STAY;
            case 5: return E;
            case 6: return SW;
            case 7: return S;
            case 8: return SE;
            default: return STAY;
        }
    }

    bool Game::isLegal(const ActionType &ac, const Position &pos) const {
        int x, y;
        x = pos.x;
        y = pos.y;
        switch (ac) {
            case E: y++; break;
            case NE: y++; x--; break;
            case N: x--; break;
            case NW: y--; x--; break;
            case W: y--; break;
            case SW: y--; x++; break;
            case S: x++; break;
            case SE: x++; y++; break;
            default: break;
        }
        Position p((unsigned )x, (unsigned)y);
        if (p.x < __height && p.y < __width)
            return true;
        return false;
    }

    const Position Game::move(const Position &pos, const ActionType &ac) const { // note: assumes legal, use with isLegal()
        if (isLegal(ac, pos)) {
            int x, y;
            x = pos.x;
            y = pos.y;
            switch (ac) {
                case E: y++; break;
                case NE: y++; x--; break;
                case N: x--; break;
                case NW: y--; x--; break;
                case W: y--; break;
                case SW: y--; x++; break;
                case S: x++; break;
                case SE: x++; y++; break;
                default: break;
            }
            Position p((unsigned )x, (unsigned)y);
            return p;
            //__grid[(unsigned)y + ((unsigned)x * __height)] = __grid[pos.y + (pos.x * __width)];
            //__grid[pos.y + (pos.x * __width)] = nullptr;
        }
        return pos;
    }

    void Game::round() {   // play a single round
        //__status = PLAYING;
        //std::cout << "Round started" << std::endl;
        std::set<Piece*> pieces;
        for (auto it = __grid.begin(); it != __grid.end(); ++it) {
            if (*it) {
                pieces.insert(pieces.end(), *it);
                //pieces.insert(*it);
                (*it)->setTurned(false);
            }
        }

        // Take turns
        for (auto it = pieces.begin(); it != pieces.end(); ++it) {
            if (!(*it)->getTurned()) {
                (*it)->setTurned(true);
                (*it)->age();
                ActionType ac = (*it)->takeTurn(getSurroundings((*it)->getPosition()));
                //std::cout << "------- Game::round -------" << std::endl;
                //std::cout << "Action: " << ac << std::endl;
                Position pos0 = (*it)->getPosition();
                //std::cout << "Pos0: " << pos0.x << "x" << pos0.y << std::endl;
                Position pos1 = move(pos0, ac);
                //std::cout << "Pos1: " << pos1.x << "x" << pos1.y << std::endl;
                if (pos0.x != pos1.x || pos0.y != pos1.y) {
                    Piece *p = __grid[pos1.y + (pos1.x * __width)];
                    if (p) {
                        (*(*it)) * (*p);
                        if ((*it)->getPosition().x != pos0.x || (*it)->getPosition().y != pos0.y) {
                            // piece moved
                            __grid[pos1.y + (pos1.x * __width)] = (*it);
                            __grid[pos0.y + (pos0.x * __width)] = p;
                        }
                    } else {
                        // empty move
                        (*it)->setPosition(pos1);
                        __grid[pos1.y + (pos1.x * __width)] = (*it);
                        __grid[pos0.y + (pos0.x * __width)] = nullptr;
                        //std::cout << "position updated of piece" << std::endl;
                    }
                }
            }
        }

        // Update positions and delete
        // Delete invalid first
        for (unsigned int i = 0; i < __grid.size(); ++i) {
            //if (__grid[i]) std::cout << "Piece viable: " << __grid[i]->isViable() << std::endl;
            if (__grid[i] && !(__grid[i]->isViable())) {
                delete __grid[i];
                __grid[i] = nullptr;
            }
            //if (__grid[i]) __grid[i]->age();
        }

        // Update positions of remaining
        /*for (unsigned int i = 0; i < __grid.size(); ++i) {
            Piece *currentPiece = __grid[i];
            if (currentPiece) {
                Position pos = currentPiece->getPosition();
                if (__grid[pos.y + (pos.x * __width)] != currentPiece) {
                    __grid[pos.y + (pos.x * __width)] = currentPiece;
                    __grid[i] = nullptr;
                    //std::cout << "place in __grid changed of a piece" << std::endl;
                }
            }
            i++;
        }*/

        // Check game over
        if (getNumResources() <= 0) {
            __status = Status::OVER;
        }
        __round++;
    }

    void Game::play(bool verbose) {    // play game until over
        __verbose = verbose;
        __status = PLAYING;
        std::cout << *this;
        while (__status != OVER) {
            round();
            if (verbose) std::cout << *this;
        }
        if (!verbose) std::cout << *this;
    }

    std::ostream &operator<<(std::ostream &os, const Game &game) {
        os << "Round " << game.__round << std::endl;
        int column = 0;
        for (auto it = game.__grid.begin(); it != game.__grid.end(); ++it) {
            if (*it == nullptr) {
                os << "[" << std::setw(6) << "]";
            } else {
                //os ;
                std::stringstream ss;
                ss << "[" << **it;
                std::string str;
                std::getline(ss, str);
                //os << str << std::setw(3) << "]";
                os << str << "]";
                //os << std::setw(4) << "[" << **it;
                //os << "]";
                //os ;
            }
            if (++column == game.__width) {
                column = 0;
                os << std::endl;
            }
        }
        os << "Status: ";
        switch (game.getStatus()) {
            case Game::Status::NOT_STARTED:
                std::cout << "Not Started..." << std::endl; break;
            case Game::Status::PLAYING:
                std::cout << "Playing..." << std::endl; break;
            default:
                std::cout << "Over!" << std::endl; break;
        }
        return os;
    }
}