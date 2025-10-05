
#include "UNO.h"
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>

using namespace std;

struct CardInternal {
    string color;
    string value;

    bool matches(const CardInternal &other) const {
        return color == other.color || value == other.value;
    }
};

struct GameData {
    int numPlayers;
    vector<vector<CardInternal>> hands;
    vector<CardInternal> deck;
    vector<CardInternal> discard;
    int currentPlayer;
    int direction;
};

static GameData gameStorage[100];
static const UNOGame* gameObjs[100];
static int gameCount = 0;

static int getGameIndex(const UNOGame* game) {
    for(int i=0; i<gameCount; i++) {
        if(gameObjs[i] == game) return i;
    }
    gameObjs[gameCount] = game;
    return gameCount++;
}

static GameData& getData(const UNOGame* game) {
    return gameStorage[getGameIndex(game)];
}

static vector<string> colors = {"Red", "Blue", "Green", "Yellow"};
static vector<string> values = {"0","1","2","3","4","5","6","7","8","9","Skip","Reverse","Draw Two"};

static vector<CardInternal> createDeck() {
    vector<CardInternal> d;

    // Add cards in specific order for each color
    for(auto &color : colors) {
        // One 0 card
        d.push_back(CardInternal{color, "0"});

        // Two of each number 1-9
        for(int i=1; i<=9; i++) {
            d.push_back(CardInternal{color, values[i]});
            d.push_back(CardInternal{color, values[i]});
        }

        // Two of each action card (Skip, Reverse, Draw Two)
        d.push_back(CardInternal{color, "Skip"});
        d.push_back(CardInternal{color, "Skip"});
        d.push_back(CardInternal{color, "Reverse"});
        d.push_back(CardInternal{color, "Reverse"});
        d.push_back(CardInternal{color, "Draw Two"});
        d.push_back(CardInternal{color, "Draw Two"});
    }

    return d;
}

UNOGame::UNOGame(int n) {
    GameData& data = getData(this);
    data.numPlayers = n;
    data.currentPlayer = 0;
    data.direction = 1;
    data.hands.resize(n);
}

void UNOGame::initialize() {
    GameData& data = getData(this);
    data.deck = createDeck();

    // Shuffle with fixed seed
    mt19937 rng(1234);
    shuffle(data.deck.begin(), data.deck.end(), rng);

    // Deal 7 cards to each player
    int deckIndex = 0;
    for(int p=0; p<data.numPlayers; p++) {
        data.hands[p].clear();
        for(int i=0; i<7; i++) {
            data.hands[p].push_back(data.deck[deckIndex++]);
        }
    }

    // Top card starts discard pile
    data.discard.clear();
    data.discard.push_back(data.deck[deckIndex++]);
}

void UNOGame::playTurn() {
    GameData& data = getData(this);
    CardInternal top = data.discard.back();
    auto &hand = data.hands[data.currentPlayer];

    // Find first matching card (color match or value match)
    auto it = hand.end();
    for(auto i = hand.begin(); i != hand.end(); i++) {
        if(i->matches(top)) {
            it = i;
            break;
        }
    }

    CardInternal played{"",""};

    if(it != hand.end()) {
        // Play the matching card
        played = *it;
        hand.erase(it);
    } else {
        // Draw a card (simulate by removing last card from hand if exists)
        if(!hand.empty()) {
            played = hand.back();
            hand.pop_back();
        } else {
            // No cards to play, skip turn
            data.currentPlayer = (data.currentPlayer + data.direction + data.numPlayers) % data.numPlayers;
            return;
        }
    }

    // Add played card to discard pile
    data.discard.push_back(played);

    // Handle action cards
    if(played.value == "Skip") {
        // Skip next player
        data.currentPlayer = (data.currentPlayer + data.direction + data.numPlayers) % data.numPlayers;
    } else if(played.value == "Reverse") {
        // Reverse direction
        data.direction *= -1;
    } else if(played.value == "Draw Two") {
        // Next player draws 2 (simulate by removing 2 cards from their hand)
        int nextPlayer = (data.currentPlayer + data.direction + data.numPlayers) % data.numPlayers;
        for(int i=0; i<2 && !data.hands[nextPlayer].empty(); i++) {
            data.hands[nextPlayer].pop_back();
        }
        // Skip next player's turn
        data.currentPlayer = (data.currentPlayer + data.direction + data.numPlayers) % data.numPlayers;
    }

    // Move to next player
    data.currentPlayer = (data.currentPlayer + data.direction + data.numPlayers) % data.numPlayers;
}

string UNOGame::getState() const {
    GameData& data = getData(this);
    ostringstream oss;

    oss << "Player " << data.currentPlayer
        << "'s turn, Direction: " << (data.direction==1 ? "Clockwise" : "Counter-Clockwise")
        << ", Top: " << data.discard.back().color << " " << data.discard.back().value
        << ", Players cards: ";

    for(int p=0; p<data.numPlayers; p++) {
        oss << "P" << p << ":" << data.hands[p].size();
        if(p != data.numPlayers-1) oss << ", ";
    }

    return oss.str();
}

bool UNOGame::isGameOver() const {
    GameData& data = getData(this);
    for(auto &h : data.hands) {
        if(h.empty()) return true;
    }
    return false;
}

int UNOGame::getWinner() const {
    GameData& data = getData(this);
    for(int i=0; i<data.numPlayers; i++) {
        if(data.hands[i].empty()) return i;
    }
    return -1;
}