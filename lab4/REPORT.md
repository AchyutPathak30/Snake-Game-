# Lab 4 — Group A

| | |
|---|---|
| Repository | AchyutPathak30/Snake-Game- |
| Base tag | `lab4-base` at commit `fbea4bb` |
| Pull request | https://github.com/AchyutPathak30/Snake-Game-/pulls |

---

## 1. Five rules — [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | Moving the snake in a direction advances its head position by 1 cell and preserves body length when no food is eaten. |
| 2 | The snake dies (`hitWall()` returns true) when its head reaches or crosses the board boundary walls. |
| 3 | Eating a fruit increases score by 10, increments apples eaten count by 1, and grows the snake body by 1 segment. |
| 4 | Hitting a bomb decreases score by 20 (floored at 0), increments bombs hit count by 1, and shrinks snake body by 1 segment (if length > 1). |
| 5 | Pressing 'W', 'S', 'A', 'D' steers the snake Up, Down, Left, or Right while disallowing immediate 180-degree reversals, and pressing 'X' terminates the game. |

---

## 2. What you could test, and what stopped you — [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | Moving the snake advances head by 1 and preserves length | Yes | None (`Snake::move()` at `snake.cpp:37`) |
| 2 | Snake dies on boundary wall collision | Yes | None (`Snake::hitWall()` at `snake.cpp:55`) |
| 3 | Eating fruit adds 10 score and grows snake | No | `snake.cpp:89` — `rand()` inside `Game::spawn()`, called at line 202 (`spawn(fruits, 1)`) and line 70 (`spawn(fruits, 6)`), making fruit placement non-deterministic. |
| 4 | Hitting bomb reduces score by 20 and shrinks body | No | `snake.cpp:89` — `rand()` inside `Game::spawn()`, called at line 192 (`spawn(bombs, 1)`) and line 72 (`spawn(bombs, 3)`), making bomb placement non-deterministic. |
| 5 | Steering input and game termination | No | `snake.cpp:146` (`_kbhit()`) and `snake.cpp:148` (`_getch()`) inside `Game::input()` read directly from the hardware console buffer with no parameter or stream seam. |

> **Rules testable without modifying the source: 2 / 5**

---

## 3. Coverage, and what it missed — [6]

| | |
|---|---|
| Line coverage | 41.18 % |
| Branch coverage | 47.92 % |
| Command used | `g++ --coverage -O0 -g tests.cpp -o tests.exe && .\tests.exe && gcov -b tests.cpp` |

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | Snake body length is preserved during standard movement |
| Line that runs | `snake.cpp:49` (`body.pop_back();` inside `Snake::move()`) |
| The assertion that is missing | `assert(s.body.size() == 1);` |

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | Steering input: Pressing direction keys ('W', 'S', 'A', 'D') updates snake direction while rejecting 180-degree reversals, and pressing 'X' sets gameOver to true. |
| Commit 1 (seam) | `49b2fd7` |
| Commit 2 (test) | `1359f1f` |
| Seam kind | object |
| Enabling point | `Game` constructor parameter `Game(InputSource *input = nullptr)` |
| What production code gave up | Production code gave up direct inlined calls to `<conio.h>`'s `_kbhit()` and `_getch()`, replacing them with an abstract collaborator pointer `inputSource` that dispatches through a virtual method table. For human interactive gameplay (100ms frames), this virtual dispatch adds negligible nanosecond latency and zero behavioral difference, preserving 100% of player functionality while enabling unit tests to inject deterministic simulated keystrokes. |

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | stub |
| The method under test | `Game::input()` |

Two sentences: was the collaborator asked a question or told to do something, and why does that decide the answer above?

The collaborator (`InputSource`) was asked questions by the method under test (`hasKey()` queries whether input is available, and `getKey()` queries which character was pressed) rather than told to execute an action. Because it returns configured answers that direct execution flow down desired code paths without verifying behavior through side-effect assertions, it is a stub rather than a mock or spy.

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | Assertion roulette | `tests.cpp:32` | Add diagnostic failure messages to each bare `assert()` statement (e.g., `assert((s.body[0].x == start_x + 1) && "Snake head X must advance by 1");`). |
| 2 | General fixture | `tests.cpp:108` | Encapsulate mutable global variables (`gameOver`, `score`) inside the `Game` instance rather than resetting global state across test runs. |
