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

class InputSource
{
public:
    virtual ~InputSource() = default;
    virtual bool hasKey() { return _kbhit() != 0; }
    virtual char getKey() { return toupper(_getch()); }
};

class Game
{
public:
    Snake snake;
    vector<Point> fruits, rocks, bombs;
    InputSource defaultInput;
    InputSource *inputSource;

    string HEAD = "🐍", BODY = "🟩", FRUIT = "🍎", ROCK = "🪨", BOMB = "💣", WALL = "🟥", EMPTY = "  ";

    Game(InputSource *input = nullptr)
    {
        inputSource = input ? input : &defaultInput;
        SetConsoleOutputCP(65001);
        srand(time(0));
        spawn(fruits, 6);
        spawn(rocks, 3);
        spawn(bombs, 3);
    }

    bool occupied(Point p)
    {
        return find(snake.body.begin(), snake.body.end(), p) != snake.body.end() ||
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

        cout << "Score: " << score << "\n";

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
                if (p == snake.body[0])
                {
                    cout << HEAD;
                    continue;
                }
                if (find(snake.body.begin() + 1, snake.body.end(), p) != snake.body.end())
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
        if (inputSource->hasKey())
        {
            char k = inputSource->getKey();
            if (k == 'X')
                gameOver = true;
            if (k == 'W' && snake.dir != 'D')
                snake.dir = 'U';
            if (k == 'S' && snake.dir != 'U')
                snake.dir = 'D';
            if (k == 'A' && snake.dir != 'R')
                snake.dir = 'L';
            if (k == 'D' && snake.dir != 'L')
                snake.dir = 'R';
        }
    }

    void logic()
    {
        snake.move();
        Point head = snake.body[0];

        if (snake.hitSelf() || snake.hitWall())
        {
            gameOver = true;
            return;
        }

        if (find(rocks.begin(), rocks.end(), head) != rocks.end())
        {
            gameOver = true;
            return;
        }

        // BOMB HIT SHRINK WORKS NOW ✅
        auto b = find(bombs.begin(), bombs.end(), head);
        if (b != bombs.end())
        {
            bombsHit++;
            score -= 20;
            if (score < 0)
                score = 0;

            if (snake.body.size() > 1)
                snake.body.pop_back(); // shrink

            bombs.erase(b);
            spawn(bombs, 1);
        }

        auto f = find(fruits.begin(), fruits.end(), head);
        if (f != fruits.end())
        {
            applesEaten++;
            score += 10;
            snake.grow();
            fruits.erase(f);
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
        cout << "🍎 Apples Eaten : " << applesEaten << "\n";
        cout << "💣 Bombs Hit    : " << bombsHit << "\n";
        cout << "🏆 Final Score  : " << score << "\n\n";
        cout << "Press any key to exit...";
        _getch();
    }
};

int main()
{
    Game game;
    game.run();
}
