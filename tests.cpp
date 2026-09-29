#define main snake_main
#include "snake.cpp"
#undef main

#include <iostream>
#include <vector>
#include <cassert>

// Test double for Part D / Part E: StubInput
class StubInput : public InputSource {
    std::vector<char> keys;
    size_t idx = 0;
public:
    StubInput(const std::vector<char>& k = {}) : keys(k), idx(0) {}
    void setKey(char k) { keys = {k}; idx = 0; }
    void setKeys(const std::vector<char>& k) { keys = k; idx = 0; }
    bool hasKey() override { return idx < keys.size(); }
    char getKey() override { return idx < keys.size() ? keys[idx++] : 0; }
};

int main() {
    std::cout << "Starting Lab 4 Test Suite..." << std::endl;

    // --- Part B: Rule 1: Snake movement advances head in direction of travel ---
    {
        Snake s;
        assert(s.dir == 'R');
        assert(s.body.size() == 1);
        int start_x = s.body[0].x;
        int start_y = s.body[0].y;
        s.move();
        assert(s.body[0].x == start_x + 1);
        assert(s.body[0].y == start_y);
        assert(s.body.size() == 1);
        std::cout << "[PASS] Rule 1: Snake movement advances head and preserves length." << std::endl;
    }

    // --- Part B: Rule 2: Boundary wall collision detection ---
    {
        Snake s;
        // Test left boundary
        s.body[0] = {0, 10};
        assert(s.hitWall() == true);

        // Test right boundary
        s.body[0] = {WIDTH - 1, 10};
        assert(s.hitWall() == true);

        // Test top boundary
        s.body[0] = {20, 0};
        assert(s.hitWall() == true);

        // Test bottom boundary
        s.body[0] = {20, HEIGHT - 1};
        assert(s.hitWall() == true);

        // Test middle (safe)
        s.body[0] = {WIDTH / 2, HEIGHT / 2};
        assert(s.hitWall() == false);
        std::cout << "[PASS] Rule 2: Snake detects boundary wall collisions." << std::endl;
    }

    // --- Part D: Rule 5: Steering input changes direction and rejects 180 reversals ---
    {
        StubInput stub;
        Game game(&stub);

        // Initial direction is 'R'
        assert(game.snake.dir == 'R');

        // Turn Up with 'W' (allowed from 'R')
        stub.setKey('W');
        game.input();
        assert(game.snake.dir == 'U');

        // Disallow immediate 180 reversal into Down ('S') when heading 'U'
        stub.setKey('S');
        game.input();
        assert(game.snake.dir == 'U');

        // Turn Left with 'A' (allowed from 'U')
        stub.setKey('A');
        game.input();
        assert(game.snake.dir == 'L');

        // Disallow immediate 180 reversal into Right ('D') when heading 'L'
        stub.setKey('D');
        game.input();
        assert(game.snake.dir == 'L');

        // Turn Down with 'S' (allowed from 'L')
        stub.setKey('S');
        game.input();
        assert(game.snake.dir == 'D');

        // Disallow immediate 180 reversal into Up ('W') when heading 'D'
        stub.setKey('W');
        game.input();
        assert(game.snake.dir == 'D');

        // Turn Right with 'D' (allowed from 'D')
        stub.setKey('D');
        game.input();
        assert(game.snake.dir == 'R');

        // 'X' keypress sets gameOver to true
        gameOver = false; // Reset global state
        stub.setKey('X');
        game.input();
        assert(gameOver == true);

        std::cout << "[PASS] Rule 5: Steering input and game termination via Seam." << std::endl;
    }

    std::cout << "\nAll Lab 4 tests passed successfully!" << std::endl;
    return 0;
}
