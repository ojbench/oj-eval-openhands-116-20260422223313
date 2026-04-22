#ifndef SRC_HPP
#define SRC_HPP
#include <iostream>
#include <utility>
#include <vector>
#include <string>

struct Map;

enum class instruction { UP, DOWN, LEFT, RIGHT, NONE };

bool is_food(Map* map, int x, int y);
bool is_wall(Map* map, int x, int y);
void eat_food(Map* map, int x, int y);
int get_height(Map* map);
int get_width(Map* map);

struct Snake {
  std::vector<std::pair<int, int>> body; // head at index 0
  instruction dir = instruction::NONE;

  void initialize(int x, int y, instruction ins) {
    body.clear();
    if (x >= 0 && y >= 0) body.push_back({x, y});
    dir = ins;
  }

  int get_length() { return static_cast<int>(body.size()); }

  static bool is_opposite(instruction a, instruction b) {
    if (a == instruction::UP && b == instruction::DOWN) return true;
    if (a == instruction::DOWN && b == instruction::UP) return true;
    if (a == instruction::LEFT && b == instruction::RIGHT) return true;
    if (a == instruction::RIGHT && b == instruction::LEFT) return true;
    return false;
  }

  bool move(Map* map, instruction ins) {
    if (ins != instruction::NONE) {
      if (dir != instruction::NONE && is_opposite(dir, ins)) {
        return false; // immediate death on reverse
      }
      dir = ins;
    }

    int dx = 0, dy = 0;
    switch (dir) {
      case instruction::UP: dx = -1; dy = 0; break;
      case instruction::DOWN: dx = 1; dy = 0; break;
      case instruction::LEFT: dx = 0; dy = -1; break;
      case instruction::RIGHT: dx = 0; dy = 1; break;
      default: dx = 0; dy = 0; break;
    }

    if (body.empty()) return false;
    int nx = body[0].first + dx;
    int ny = body[0].second + dy;

    int h = get_height(map);
    int w = get_width(map);
    if (nx < 0 || nx >= h || ny < 0 || ny >= w) return false;
    if (is_wall(map, nx, ny)) return false;

    bool grow = is_food(map, nx, ny);

    // check self-collision; moving into tail is allowed if not growing
    int limit = static_cast<int>(body.size()) - (grow ? 0 : 1);
    for (int i = 0; i < limit; ++i) {
      if (body[i].first == nx && body[i].second == ny) return false;
    }

    // apply move
    body.insert(body.begin(), {nx, ny});
    if (grow) {
      eat_food(map, nx, ny);
    } else {
      body.pop_back();
    }

    return true;
  }

  std::pair<int, std::pair<int, int>*> get_snake() {
    int n = static_cast<int>(body.size());
    auto* arr = new std::pair<int, int>[n > 0 ? n : 1];
    for (int i = 0; i < n; ++i) arr[i] = body[i];
    return {n, arr};
  }
};

const int MaxWidth = 20;

struct Map {
  bool wall[MaxWidth][MaxWidth]{};
  bool food[MaxWidth][MaxWidth]{};
  int width = 0, height = 0;

  int get_height() { return height; }
  int get_width() { return width; }

  bool is_food(int x, int y) { return (x >= 0 && x < height && y >= 0 && y < width) ? food[x][y] : false; }
  void eat_food(int x, int y) { if (x >= 0 && x < height && y >= 0 && y < width) food[x][y] = false; }
  bool is_wall(int x, int y) { return (x >= 0 && x < height && y >= 0 && y < width) ? wall[x][y] : true; }

  void initialize(Snake* snake) {
    for (int i = 0; i < MaxWidth; ++i) {
      for (int j = 0; j < MaxWidth; ++j) {
        wall[i][j] = false;
        food[i][j] = false;
      }
    }

    std::cin >> height >> width;
    int head_x = -1, head_y = -1;
    for (int i = 0; i < height; ++i) {
      std::string line; std::cin >> line;
      for (int j = 0; j < width && j < static_cast<int>(line.size()); ++j) {
        char c = line[j];
        if (c == '#') wall[i][j] = true;
        else if (c == '*') food[i][j] = true;
        else if (c == '@') { head_x = i; head_y = j; }
      }
    }

    std::string d; std::cin >> d;
    instruction ins = instruction::NONE;
    if (!d.empty()) {
      switch (d[0]) {
        case 'U': ins = instruction::UP; break;
        case 'D': ins = instruction::DOWN; break;
        case 'L': ins = instruction::LEFT; break;
        case 'R': ins = instruction::RIGHT; break;
        default: ins = instruction::NONE; break;
      }
    }

    snake->initialize(head_x, head_y, ins);
  }

  void print(Snake* snake) {
    std::vector<std::string> grid(height, std::string(width, '.'));
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        if (wall[i][j]) grid[i][j] = '#';
        else if (food[i][j]) grid[i][j] = '*';
      }
    }

    auto snake_body = snake->get_snake();
    int n = snake_body.first;
    auto* arr = snake_body.second;
    for (int i = n - 1; i >= 0; --i) {
      int x = arr[i].first, y = arr[i].second;
      if (x >= 0 && x < height && y >= 0 && y < width) {
        if (i == 0) grid[x][y] = '@'; else grid[x][y] = 'o';
      }
    }
    delete[] arr;

    for (int i = 0; i < height; ++i) {
      std::cout << grid[i] << std::endl;
    }
  }
};

struct Game{
  Map *map;
  Snake *snake;
  int score;
  int round;
  void initialize(){
    map = new Map();
    snake = new Snake();
    map->initialize(snake);
    score = 0;
    round = 0;
  }
  bool step()
  {
    char str[MaxWidth];
    std::cin >> str;
    instruction ins;
    switch(str[0]){
      case 'U':
        ins = instruction::UP;
        break;
      case 'D':
        ins = instruction::DOWN;
        break;
      case 'L':
        ins = instruction::LEFT;
        break;
      case 'R':
        ins = instruction::RIGHT;
        break;
      default:
        ins = instruction::NONE;
    }
    if(snake->move(map, ins)){
      score++;
    }else{
      return false;
    }
    return true;
  }
  void print(){
    std::cout<< "Round: " << round << std::endl;
    map->print(snake);
  }
  void play()
  {
    while(step()){
      round++;
      print();
    }
    score += snake->get_length() ;
    std::cout << "Game Over" << std::endl;
    std::cout << "Score: " << score << std::endl;
  }
};

#endif
