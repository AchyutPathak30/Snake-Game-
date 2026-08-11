#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

int score = 0;
int applesEaten = 0;
int bombsHit = 0;
int score2 = 0;
int applesEaten2 = 0;
int bombsHit2 = 0;
int loser = 0; // 1 = Player 1, 2 = Player 2, 3 = Both (Draw)
bool gameOver = false;

struct Point
{
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point &p) const { return x == p.x && y == p.y; }
};

class Snake
{
public:
    vector<Point> body;
    char dir;

    Snake()
    {
        // start with ONLY head
        body.push_back({WIDTH / 2, HEIGHT / 2});
        dir = 'R';
    }

    void move()
    {
        Point h = body[0];
        if (dir == 'U')
            h.y--;
        if (dir == 'D')
            h.y++;
        if (dir == 'L')
            h.x--;
        if (dir == 'R')
            h.x++;
        body.insert(body.begin(), h);
        body.pop_back();
    }

    void grow() { body.push_back(body.back()); }

    bool hitSelf() { return find(body.begin() + 1, body.end(), body[0]) != body.end(); }
    bool hitWall() { return body[0].x <= 0 || body[0].x >= WIDTH - 1 || body[0].y <= 0 || body[0].y >= HEIGHT - 1; }
};

class Game
{
public:
    Snake snake;
    Snake snake2;
    vector<Point> fruits, rocks, bombs;

    string HEAD = "🐍", BODY = "🟩", FRUIT = "🍎", ROCK = "🪨", BOMB = "💣", WALL = "🟥", EMPTY = "  ";

    Game()
    {
        SetConsoleOutputCP(65001);
        srand(time(0));

        // Start Player 1 on the left side, moving right
        snake.body[0] = {WIDTH / 3, HEIGHT / 2};
        snake.dir = 'R';

        // Start Player 2 on the right side, moving left
        snake2.body.clear();
        snake2.body.push_back({2 * WIDTH / 3, HEIGHT / 2});
        snake2.dir = 'L';

        spawn(fruits, 6);
        spawn(rocks, 3);
        spawn(bombs, 3);
    }

    bool occupied(Point p)
    {
        return find(snake.body.begin(), snake.body.end(), p) != snake.body.end() ||
               find(snake2.body.begin(), snake2.body.end(), p) != snake2.body.end() ||
               find(fruits.begin(), fruits.end(), p) != fruits.end() ||
               find(rocks.begin(), rocks.end(), p) != rocks.end() ||
               find(bombs.begin(), bombs.end(), p) != bombs.end();
    }

    void spawn(vector<Point> &v, int count)
    {
        while (count--)
        {
            Point p;
            do
                p = {rand() % (WIDTH - 2) + 1, rand() % (HEIGHT - 2) + 1};
            while (occupied(p));
            v.push_back(p);
        }
    }

    void draw()
    {
        COORD c = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

        cout << "Player 1 Score: " << score << "   |   Player 2 Score: " << score2 << "       \n";

        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                Point p(x, y);

                if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1)
                {
                    cout << WALL;
                    continue;
                }
                if (p == snake.body[0] || p == snake2.body[0])
                {
                    cout << HEAD;
                    continue;
                }
                if (find(snake.body.begin() + 1, snake.body.end(), p) != snake.body.end() ||
                    find(snake2.body.begin() + 1, snake2.body.end(), p) != snake2.body.end())
                {
                    cout << BODY;
                    continue;
                }
                if (find(fruits.begin(), fruits.end(), p) != fruits.end())
                {
                    cout << FRUIT;
                    continue;
                }
                if (find(rocks.begin(), rocks.end(), p) != rocks.end())
                {
                    cout << ROCK;
                    continue;
                }
                if (find(bombs.begin(), bombs.end(), p) != bombs.end())
                {
                    cout << BOMB;
                    continue;
                }
                cout << EMPTY;
            }
            cout << "\n";
        }
    }

    void input()
    {
        if (_kbhit())
        {
            char k = toupper(_getch());
            if (k == 'X')
                gameOver = true;
            if (k == 'W')
            {
                if (snake.dir != 'D') snake.dir = 'U';
                if (snake2.dir != 'D') snake2.dir = 'U';
            }
            if (k == 'S')
            {
                if (snake.dir != 'U') snake.dir = 'D';
                if (snake2.dir != 'U') snake2.dir = 'D';
            }
            if (k == 'A')
            {
                if (snake.dir != 'R') snake.dir = 'L';
                if (snake2.dir != 'R') snake2.dir = 'L';
            }
            if (k == 'D')
            {
                if (snake.dir != 'L') snake.dir = 'R';
                if (snake2.dir != 'L') snake2.dir = 'R';
            }
        }
    }

    void logic()
    {
        snake.move();
        snake2.move();
        Point head1 = snake.body[0];
        Point head2 = snake2.body[0];

        bool snake1Dead = snake.hitWall() || snake.hitSelf() || (find(snake2.body.begin(), snake2.body.end(), head1) != snake2.body.end());
        bool snake2Dead = snake2.hitWall() || snake2.hitSelf() || (find(snake.body.begin(), snake.body.end(), head2) != snake.body.end());

        bool snake1HitRock = (find(rocks.begin(), rocks.end(), head1) != rocks.end());
        bool snake2HitRock = (find(rocks.begin(), rocks.end(), head2) != rocks.end());

        if (snake1Dead || snake1HitRock)
        {
            gameOver = true;
            loser = 1;
        }
        if (snake2Dead || snake2HitRock)
        {
            gameOver = true;
            if (loser == 1)
                loser = 3;
            else
                loser = 2;
        }

        if (gameOver)
            return;

        // Player 1 Bomb collision
        auto b1 = find(bombs.begin(), bombs.end(), head1);
        if (b1 != bombs.end())
        {
            bombsHit++;
            score -= 20;
            if (score < 0)
                score = 0;

            if (snake.body.size() > 1)
                snake.body.pop_back();

            bombs.erase(b1);
            spawn(bombs, 1);
        }

        // Player 2 Bomb collision
        auto b2 = find(bombs.begin(), bombs.end(), head2);
        if (b2 != bombs.end())
        {
            bombsHit2++;
            score2 -= 20;
            if (score2 < 0)
                score2 = 0;

            if (snake2.body.size() > 1)
                snake2.body.pop_back();

            bombs.erase(b2);
            spawn(bombs, 1);
        }

        // Player 1 Fruit collision
        auto f1 = find(fruits.begin(), fruits.end(), head1);
        if (f1 != fruits.end())
        {
            applesEaten++;
            score += 10;
            snake.grow();
            fruits.erase(f1);
            spawn(fruits, 1);
        }

        // Player 2 Fruit collision
        auto f2 = find(fruits.begin(), fruits.end(), head2);
        if (f2 != fruits.end())
        {
            applesEaten2++;
            score2 += 10;
            snake2.grow();
            fruits.erase(f2);
            spawn(fruits, 1);
        }
    }

    void run()
    {
        while (!gameOver)
        {
            draw();
            input();
            logic();
            Sleep(100);
        }

        system("cls");
        cout << "\n\n   💀 GAME OVER 💀\n\n";
        if (loser == 3)
            cout << "   🤝 DRAW! Both players lost! 🤝\n\n";
        else
            cout << "   ❌ Player " << loser << " Lost! ❌\n\n";

        cout << "--- Player 1 Stats ---\n";
        cout << "🍎 Apples Eaten : " << applesEaten << "\n";
        cout << "💣 Bombs Hit    : " << bombsHit << "\n";
        cout << "🏆 Final Score  : " << score << "\n\n";

        cout << "--- Player 2 Stats ---\n";
        cout << "🍎 Apples Eaten : " << applesEaten2 << "\n";
        cout << "💣 Bombs Hit    : " << bombsHit2 << "\n";
        cout << "🏆 Final Score  : " << score2 << "\n\n";

        cout << "Press any key to exit...";
        _getch();
    }
};

int main()
{
    Game game;
    game.run();
}
