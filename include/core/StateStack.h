#pragma once
#include <memory>
#include <vector>
#include "GameState.h"

class Screen;
class StateStack {
public:
    void push(GameState);
    void pop();
    void handleInput();
    void update(float dt);
    void draw() const;
private:
    std::vector<std::unique_ptr<Screen>> stack;
};
