#include "player.h"
#include "game.h"
#include "gamekey.h"
#include<string>
#include<iostream>
#include<memory>
#include<algorithm>
#include<vector>
#include<map>
#include "player.h"
Player::Player(string name, int mmr) : name{ name }, mmr{ mmr } {
    if (name.size() == 0 || mmr < 0 || mmr>9999) throw runtime_error("error Player 1");
}
string Player::get_name() const {
    return name;
}
int Player::get_mmr() const {
    return this->mmr;
}

shared_ptr<Game> Player::get_hosted_game() const {
    return hosted_game;
}

void Player::change_mmr(int n) {
    int newValue = mmr + n;
    if (newValue >= 0 && newValue <= 9999) {
        this->mmr = newValue;
    }
}

bool Player::host_game(string s, Mode m) {
    if (s.empty()) throw runtime_error("string s leer");
    if (this->get_hosted_game() != nullptr) {
        return false;
    }
    else {
        switch (m) {
        case Mode::Ranked:
            hosted_game = std::make_shared<RGame>(s, shared_from_this());
            break;
        case Mode::Unranked:
            hosted_game = std::make_shared<UGame>(s, shared_from_this());
            break;
        }
        return true;
    }
}

bool Player::join_game(shared_ptr<Game> g) {

    if (g->add_player(GameKey{}, shared_from_this())) {
        games.insert_or_assign(g->get_name(), std::weak_ptr<Game>(g));
        return true;
    }
    return false;
}




bool Player::leave_game(shared_ptr<Game> g) {
    if (g->remove_player(GameKey{}, shared_from_this()))
    {
        games.erase(g->get_name());
        return true;
    }
    return false;

}
bool Player::close_game() {
    if (hosted_game != nullptr) hosted_game = nullptr;
    else return false;

    return true;
}

vector<weak_ptr<Player>> Player::invite_players(const vector<weak_ptr<Player>>& v) {
    vector<weak_ptr<Player>> ret;
    for (const auto& ptrToPlayer : v)
    {
        bool success = false;
        if (!ptrToPlayer.expired()) {
            success = ptrToPlayer.lock()->join_game(hosted_game);
        }
        if (!success) {
            ret.push_back(ptrToPlayer);
        }
    }
    return ret;
}

// [name, mmr, hosts: hosted_game_name, games: {Game_name, Game_name, ... }]
ostream& Player::print(ostream& o) const {
    bool first = true;
    o << "[" << get_name() << ", " << get_mmr() << ", hosts: ";
    if (hosted_game) { o << get_hosted_game()->get_name(); }
    else {
        o << "nothing";
    }

    o << ", games: {";
    for (auto it : games) {
        if (!it.second.expired()) {
            if (!first) o << ", ";
            first = false;
            o << it.second.lock()->get_name();
        }
    }
    o << "}]";
    return o;
}

ostream& operator<<(ostream& o, const Player& p) {
    return p.print(o);
}