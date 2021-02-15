#include "game.h"
#include "player.h"
#include "gamekey.h"
#include<string>
#include<iostream>
#include<algorithm>
#include<memory>
#include<numeric>
#include<vector>
#include<map>
using namespace std;


Game::Game(string name, shared_ptr<Player> host) : name{ name }, host{ host } {
    if (name.empty())throw runtime_error("player name needed");
    if (host == nullptr)throw runtime_error("host is nullptr");
}
string Game::get_name() const {
    return this->name;
}
map<string, shared_ptr<Player>>Game::get_players() const {
    return this->players;
}

bool Game::is_allowed(int n) const {
    if (n > (int)(0.9 * host->get_mmr()) && n < (int)(1.1 * host->get_mmr()))return true;
    return false;
}

bool Game::remove_player(const GameKey&, shared_ptr<Player> p) {
    return players.erase(p->get_name());
}
bool Game::add_player(const GameKey&, shared_ptr<Player> p) {
    if (is_allowed(p->get_mmr()) == false) return false;

    auto i = find_if(players.begin(), players.end(), [=](auto a) {
        return a.second->get_name() == p->get_name();
        });

    if (i != players.end()) { return false; }
    players[p->get_name()] = p;
    return true;
}




shared_ptr<Player> Game::best_player() const {
    if (players.size() == 0) throw runtime_error("is empty");
    auto it = max_element(players.begin(), players.end(), [](auto p1, auto p2) {
        return p1.second->get_mmr() < p2.second->get_mmr();
        });
    return it->second;
}

size_t Game::number_of_players() const {
    return players.size();
}

shared_ptr<Player> Game::play(size_t i) {
    shared_ptr<Player>winner = nullptr;
    size_t counter = 0;
    for (auto it = players.begin(); it != players.end(); ++it) {
        if (counter == i) {
            winner = it->second;
            break;
        }
        ++counter;
    }
    for (auto it = players.begin(); it != players.end(); ++it) {
        shared_ptr<Player>& curplay = it->second;
        if (curplay != winner) {
            int multi = 1;
            if (curplay->get_mmr() > winner->get_mmr()) { multi = 2; }
            curplay->change_mmr(multi * change(false));
        }
    }
    winner->change_mmr(change(true));
    return winner;
}
// [name, host->name, host->mmr, player: {[Player_name, Player_mmr], [Player_name,
//Player_mmr], ... }]

ostream& Game::print(ostream& o) const {
    o << "[" << name << ", " << host->get_name() << ", " << host->get_mmr() << ", player: {";

    bool first{ true };
    for (const auto& elem : players) {
        if (first) {
            o << "[" << elem.second->get_name() << ", " << elem.second->get_mmr() << "]";
            first = false;
        }
        else o << ", " "[" << elem.second->get_name() << ", " << elem.second->get_mmr() << "]";
    }
    o << "}]";

    return o;
}

ostream& operator<< (ostream& o, const Game& g) {
    return g.print(o);
}

RGame::RGame(string name, shared_ptr<Player> host) :Game{ name,host } {}

int RGame::change(bool x) const {
    if (x == true) return 5; else return -5;
}
UGame::UGame(string name, shared_ptr<Player> host) : Game{ name,host } {}

int UGame::change(bool) const {
    return 0;
}
ostream& RGame::print(ostream& o) const {
    o << "Ranked Game: ";
    return Game::print(o);
}

ostream& UGame::print(ostream& o) const {
    o << "Unranked Game: ";
    return Game::print(o);
}