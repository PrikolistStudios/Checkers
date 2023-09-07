#include <time.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <list>

sf::Vector2i resolution(1600,900);
sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y),
                        "Stochastic Fractal");

int field_size = 800;
int start_x = resolution.x / 2 - field_size / 2+ 300,
    start_y = resolution.y / 2 - field_size / 2;

double cell_size = field_size / 8;

bool game_over = true;
std::string game_res = "Hell";

bool white_turn = true;

int cur_mode = 2;

  static sf::Texture white_checker_texture, black_checker_texture,
    white_piece_texture, black_piece_texture, black_queen_texture,
    white_queen_texture;

struct Unit {
  Unit(bool enemy, bool queen, int i, int j)
      : is_black_(enemy), is_queen_(queen), i_(i), j_(j) {}
  Unit() = default;
  bool is_black_, is_queen_;
  int i_, j_;
};

typedef std::list<Unit>::iterator UnitIt;
UnitIt selected_unit;

typedef std::list<Unit> Board;
Board units;

std::vector<Unit> highlighted;
//std::vector<Unit*> necessary_turn;


bool IsInField(int i, int j) { return !(i < 0 || i > 7 || j < 0 || j > 7); }

bool IsCellBusy(int i, int j, Board& b) {
  for (auto val : b) {
    if (val.i_ == i && val.j_ == j) {
      return true;
    }
  }
  return false;
}

UnitIt GetUnit(int i, int j, Board& b) {
  for (auto it = b.begin(); it != b.end(); ++it) {
    if (it->i_ == i && it->j_ == j) {
      return it;
    }
  }
  return b.end();
}

std::vector<Unit> GetAchievable(Unit unit, Board& b = units) {
  std::vector<Unit> res;
  if (unit.is_queen_) {
    int i = 1;
    bool met_enemy = false;
    while (IsInField(unit.i_ - i, unit.j_ - i)) {
      if (!IsCellBusy(unit.i_ - i, unit.j_ - i, b)) {
        res.push_back(Unit(met_enemy, false, unit.i_ - i, unit.j_ - i));
      } else if ((GetUnit(unit.i_ - i, unit.j_ - i, b))->is_black_ !=
                 unit.is_black_) {
        if (met_enemy) break;
        met_enemy = true;
      } else {
        break;
      }
      i++;
    }

    i = 1;
    met_enemy = false;
    while (IsInField(unit.i_ - i, unit.j_ + i)) {
      if (!IsCellBusy(unit.i_ - i, unit.j_ + i, b)) {
        res.push_back(Unit(met_enemy, false, unit.i_ - i, unit.j_ + i));
      } else if ((GetUnit(unit.i_ - i, unit.j_ + i,b))->is_black_ !=
                 unit.is_black_) {
        if (met_enemy) break;
        met_enemy = true;
      } else {
        break;
      }
      i++;
    }

    i = 1;
    met_enemy = false;
    while (IsInField(unit.i_ + i, unit.j_ + i)) {
      if (!IsCellBusy(unit.i_ + i, unit.j_ + i, b)) {
        res.push_back(Unit(met_enemy, false, unit.i_ + i, unit.j_ + i));
      } else if ((GetUnit(unit.i_ + i, unit.j_ + i, b))->is_black_ !=
                 unit.is_black_) {
        if (met_enemy) break;
        met_enemy = true;
      } else {
        break;
      }
      i++;
    }

    i = 1;
    met_enemy = false;
    while (IsInField(unit.i_ + i, unit.j_ - i)) {
      if (!IsCellBusy(unit.i_ + i, unit.j_ - i, b)) {
        res.push_back(Unit(met_enemy, false, unit.i_ + i, unit.j_ - i));
      } else if ((GetUnit(unit.i_ + i, unit.j_ - i, b))->is_black_ !=
                 unit.is_black_) {
        if (met_enemy) break;
        met_enemy = true;
      } else {
        break;
      }
      i++;
    }
  } else {
    if (IsInField(unit.i_ - 1, unit.j_ - 1)) {
      if (!IsCellBusy(unit.i_ - 1, unit.j_ - 1,b)) {
        res.push_back(Unit(false, false, unit.i_ - 1, unit.j_ - 1));
      } else if ((GetUnit(unit.i_ - 1, unit.j_ - 1, b))->is_black_ !=
                 unit.is_black_) {
        if (IsInField(unit.i_ - 2, unit.j_ - 2) &&
            !IsCellBusy(unit.i_ - 2, unit.j_ - 2, b)) {
          res.push_back(Unit(true, false, unit.i_ - 2, unit.j_ - 2));
        }
      }
    }

    if (IsInField(unit.i_ - 1, unit.j_ + 1)) {
      if (!IsCellBusy(unit.i_ - 1, unit.j_ + 1,b)) {
        res.push_back(Unit(false, false, unit.i_ - 1, unit.j_ + 1));
      } else if ((GetUnit(unit.i_ - 1, unit.j_ + 1, b))->is_black_ !=
                 unit.is_black_) {
        if (IsInField(unit.i_ - 2, unit.j_ + 2) &&
            !IsCellBusy(unit.i_ - 2, unit.j_ + 2,b)) {
          res.push_back(Unit(true, false, unit.i_ - 2, unit.j_ + 2));
        }
      }
    }
    if (IsInField(unit.i_ + 1, unit.j_ + 1)) {
      if (!IsCellBusy(unit.i_ + 1, unit.j_ + 1,b)) {
        res.push_back(Unit(false, false, unit.i_ + 1, unit.j_ + 1));
      } else if ((GetUnit(unit.i_ + 1, unit.j_ + 1,b))->is_black_ !=
                 unit.is_black_) {
        if (IsInField(unit.i_ + 2, unit.j_ + 2) &&
            !IsCellBusy(unit.i_ + 2, unit.j_ + 2,b)) {
          res.push_back(Unit(true, false, unit.i_ + 2, unit.j_ + 2));
        }
      }
    }

    if (IsInField(unit.i_ + 1, unit.j_ - 1)) {
      if (!IsCellBusy(unit.i_ + 1, unit.j_ - 1,b)) {
        res.push_back(Unit(false, false, unit.i_ + 1, unit.j_ - 1));
      } else if ((GetUnit(unit.i_ + 1, unit.j_ - 1,b))->is_black_ !=
                 unit.is_black_) {
        if (IsInField(unit.i_ + 2, unit.j_ - 2) &&
            !IsCellBusy(unit.i_ + 2, unit.j_ - 2,b)) {
          res.push_back(Unit(true, false, unit.i_ + 2, unit.j_ - 2));
        }
      }
    }
  }

  bool found = false;
  for (auto val : res) {
    if (val.is_black_) {
      found = true;
      break;
    }
  }

  if (found) {
    for (int i = 0; i < res.size(); ++i) {
      if (!res[i].is_black_) {
        res.erase(res.begin() + i);
        i--;
      }
    }
  }

  for (int i = 0; i < res.size(); ++i) {
    if (!unit.is_black_) {
      if (!res[i].is_black_ && res[i].i_ > unit.i_ && !unit.is_queen_) {
        res.erase(res.begin() + i);
        i--;
      }
    }
    else {
      if (!res[i].is_black_ && res[i].i_ < unit.i_ && !unit.is_queen_) {
        res.erase(res.begin() + i);
        i--;
      }
    }
  }


  return res;
}

std::vector<UnitIt> GetUnitsToDoTurnNecessarily() {
  std::vector<UnitIt> res;
  for (auto it = units.begin(); it != units.end();++it) {
    if (white_turn != it->is_black_) {
      auto suspended = GetAchievable(*it);
      for (auto &to_check : suspended) {
        if (to_check.is_black_) {
          res.push_back(it);
          break;
        }
      }
    }
  }

  return res;
}

std::vector<UnitIt> GetUnitsWithTurnPossibility() { 
  std::vector<UnitIt> res;
  for (auto it = units.begin(); it != units.end(); ++it) {
    if (white_turn != it->is_black_) {
      auto achievable = GetAchievable(*it);
      if (!achievable.empty()) {
        res.push_back(it);
      }
    }
  }

  return res;
}

bool MoveUnit(UnitIt start, Unit goal, Board& b) {
  if (goal.is_black_) {
    int offset_from_selected = 0;
    while (std::abs(start->i_ - goal.i_) >= ++offset_from_selected) {
      int check_i, check_j;
      check_i =
          start->i_ + offset_from_selected * ((start->i_ > goal.i_) ? -1 : 1);
      check_j =
          start->j_ + offset_from_selected * ((start->j_ > goal.j_) ? -1 : 1);
      if (IsCellBusy(check_i, check_j,b)) {
         b.erase(GetUnit(check_i, check_j,b));

        break;
      }
    }
  }
  start->i_ = goal.i_, start->j_ = goal.j_;

  if (start->is_black_ && goal.i_ == 7 || !start->is_black_ && goal.i_ == 0) {
    start->is_queen_ = true;
  }

  auto achievable = GetAchievable(*start);
  bool can_kill = false;
  for (auto val : achievable) {
    if (val.is_black_) {
      can_kill = true;
      break;
    }
  }
  if (!can_kill || !goal.is_black_) {
    //white_turn = !white_turn;
    //highlighted.clear();
    //necessary_turn = GetUnitsToDoTurnNecessarily();
    return true;
  }
  return false;
}

std::vector<Board> GetCaptures(Unit unit, Board& b) { 
  std::vector<Board> res;
  auto achievable = GetAchievable(unit, b);
  for (auto val : achievable) {
    if (val.is_black_) {
      Board bprime = b;
      UnitIt moving = GetUnit(unit.i_, unit.j_, bprime);
      MoveUnit(moving, val, bprime);
      auto captures = GetCaptures(*moving, bprime);
      if (captures.empty()) {
        res.push_back(bprime);
      }
      else {
        for (auto& el : captures) {
          res.push_back(el);
        }
      }
    }
  }
  
  return res;
}

std::vector<Board> GetSimpleMoves(Unit unit, Board& b) {
  std::vector<Board> res;
  auto achievable = GetAchievable(unit, b);

  for (auto val : achievable) {
    if (val.is_black_) {
      return std::vector<Board>();
    } else {
      Board bprime = b;
      UnitIt moving = GetUnit(unit.i_, unit.j_, bprime);
      MoveUnit(moving, val, bprime);
      res.push_back(bprime);
    }
  }

  return res;
}

std::vector<Board> GetLegalMoves(Board& b, bool for_black_) {
  std::vector<Board> captures;
  std::vector<Board> simple;
  for (auto val : b) {
    if (val.is_black_ == for_black_) {
      auto tmp = GetCaptures(val, b);
      for (auto el : tmp) {
        captures.push_back(el);
      }

      tmp = GetSimpleMoves(val, b);
      for (auto el : tmp) {
        simple.push_back(el);
      }
    }
  }

  if (captures.empty()) {
    return simple;
  } else {
    return captures;
  }
}

struct Node {
  Node(Board b) : game_state(b) {}
  Board game_state;
  std::vector<Node*> children;
  int heuristic;
};

class Bot {
 public:
  Bot(std::string name) : name_(name) {}
  void virtual DoTurn() = 0;
  std::string name() { return name_; }

 private:
  std::string name_;
};

Bot* bot;

class Newbie : public Bot {
 public:
  Newbie() : Bot("Newbie mode") {}
  void DoTurn() override final {
    auto moves = GetLegalMoves(units, true);
    units = moves[std::rand() % moves.size()];
  }
};

class AIBot : public Bot {
 public:
  AIBot(std::string name, int max_depth) : Bot(name), max_depth_(max_depth) {}
  void DoTurn() override final {
    Node* tree = new Node(units);
    Build(tree);
    CalcRecursiveHeuristic(tree);

    for (auto child : tree->children) {
      if (child->heuristic == tree->heuristic) {
        units = child->game_state;
        return;
      }
    }
  }

 private:
  int max_depth_;

  double virtual GetHeuristic(Board& list) = 0;
  void Build(Node* node, int depth = 0) {
    if (depth == max_depth_) {
      node->heuristic = GetHeuristic(node->game_state);
      return;
    }
    auto moves = GetLegalMoves(node->game_state, (depth + 1) % 2);
    for (auto val : moves) {
      node->children.push_back(new Node(val));
      Build(node->children.back(), depth + 1);
    }
  }
  void CalcRecursiveHeuristic(Node* node, int depth = 0) {
    if (depth == max_depth_) {
      return;
    }
    int heuristic = INT32_MIN;
    for (Node* child : node->children) {
      CalcRecursiveHeuristic(child, depth + 1);
      if (heuristic == INT32_MIN) {
        heuristic = child->heuristic;
        continue;
      }

      if (depth % 2 && child->heuristic < heuristic ||
          depth % 2 == 0 && child->heuristic > heuristic) {
        heuristic = child->heuristic;
      }
    }

    node->heuristic = heuristic;
  }
};

class MiddleMan : public AIBot {
 public:
  MiddleMan() : AIBot("Middle mode", 3) {}

 private:
  double virtual GetHeuristic(Board& b) override {
    // pieces, queens, back_row, mid_box, mid_rows, endangered, protected
    std::vector<double> ai(7), player(7);
    for (auto unit : b) {
      std::vector<double>& mas = unit.is_black_ ? ai : player;
      if (unit.is_queen_) {
        mas[1]++;
      } else {
        mas[0]++;
      }

      if (unit.i_ == 8) {
        mas[2]++;
        mas[6]++;
      }

      if (unit.i_ == 3 || unit.i_ == 4) {
        if (unit.j_ >= 2 && unit.j_ <= 5) {
          mas[3]++;
        } else {
          mas[4]++;
        }
      }
    }

    std::vector<double> weights = {4, 8, 1, 2, 1.5, -3, 3};
    double res = 0;
    for (int i = 0; i < 5; ++i) {
      res += (ai[i] - player[i]) * weights[i];
    }

    int rand_min = -5, rand_max = 5;
    return res + rand_min + rand() % (rand_max - rand_min + 1);
  }
};

class Hardcore : public AIBot {
 public:
  Hardcore() : AIBot("Hard mode", 5) {}

 private:
  double virtual GetHeuristic(Board& b) override {
    // pieces, queens, back_row, mid_box, mid_rows, endangered, protected
    std::vector<double> ai(7), player(7);
    for (auto unit : b) {
      std::vector<double>& mas = unit.is_black_ ? ai : player;
      if (unit.is_queen_) {
        mas[1]++;
      } else {
        mas[0]++;
      }

      if (unit.i_ == 8) {
        mas[2]++;
        mas[6]++;
      }

      if (unit.i_ == 3 || unit.i_ == 4) {
        if (unit.j_ >= 2 && unit.j_ <= 5) {
          mas[3]++;
        } else {
          mas[4]++;
        }
      }
    }

    std::vector<double> weights = {4, 8, 1, 2, 1.5, -3, 3};
    double res = 0;
    for (int i = 0; i < 5; ++i) {
      res += (ai[i] - player[i]) * weights[i];
    }

    return res;
  }
};


void ResetField() {
  units.clear();
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (i != 3 && i != 4) {
        if ((i + j) % 2) {
          units.push_back(Unit(i < 3, false, i, j));
        }
      }
    }
  }

  white_turn = true;
  highlighted.clear();
  //necessary_turn.clear();
  game_over = false;
}

bool CheckForWin(Board& b) {
   bool met_black = false, met_white = false;
   for (auto val : b) {
     if (val.is_black_) met_black = true;
     if (!val.is_black_) met_white = true;
   }

   if (!(met_black && met_white)) {
     return true;
   }
   return false;
}

void DrawField() {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      sf::Sprite cell;
      cell.setTexture((i + j) % 2 ? black_checker_texture
                                  : white_checker_texture);
      cell.setPosition(
          sf::Vector2f(start_x + j * cell_size, start_y + i * cell_size));
      window.draw(cell);
    }
  }

  for (auto val : units) {
    sf::Vector2f pos(start_x + cell_size * (val.j_),
                     start_y + cell_size * (val.i_));
    sf::Sprite circle;
    circle.setPosition(pos);
    if (val.is_queen_) {
      circle.setTexture(val.is_black_ ? black_queen_texture
                                      : white_queen_texture);
    } else {
      circle.setTexture(val.is_black_ ? black_piece_texture
                                      : white_piece_texture);
    }
    window.draw(circle);
  }

  sf::Font font;
  if (!font.loadFromFile("ALGER.TTF")) {
    std::cout << "bebra";
  }

  //sf::Text whose_turn;
  //whose_turn.setFont(font);
  //whose_turn.setString((white_turn) ? "White turn" : "Black turn");
  //whose_turn.setFillColor(sf::Color::White);
  //whose_turn.setPosition(sf::Vector2f(start_x, start_y - 50));
  //whose_turn.setCharacterSize(30);
  //window.draw(whose_turn);

  sf::Text modes;
  modes.setFont(font);
  modes.setCharacterSize(30);

  modes.setString("1 - Newbie mode");
  modes.setFillColor(cur_mode == 1 ? sf::Color::Red : sf::Color::White);
  modes.setPosition({50, 200});
  window.draw(modes);

  modes.setString("2 - Middleman mode");
  modes.setFillColor(cur_mode == 2 ? sf::Color::Red : sf::Color::White);
  modes.setPosition({50, 230});
  window.draw(modes);

  modes.setString("3 - Hardcore mode");
  modes.setFillColor(cur_mode == 3 ? sf::Color::Red : sf::Color::White);
  modes.setPosition({50, 260});
  window.draw(modes);

  for (auto val : highlighted) {
    sf::RectangleShape cell;
    cell.setPosition(sf::Vector2f(start_x + val.j_ * cell_size,
                                  start_y + val.i_ * cell_size));
    cell.setSize(sf::Vector2f(cell_size, cell_size));
    cell.setFillColor(sf::Color::Transparent);
    cell.setOutlineColor(sf::Color::Green);
    cell.setOutlineThickness(-10);
    window.draw(cell);
  }

  if (highlighted.empty()) {
    std::vector<UnitIt> necessary_turn = GetUnitsToDoTurnNecessarily();
    for (auto val : necessary_turn) {
      sf::RectangleShape cell;
      cell.setPosition(sf::Vector2f(start_x + val->j_ * cell_size,
                                    start_y + val->i_ * cell_size));
      cell.setSize(sf::Vector2f(cell_size, cell_size));
      cell.setFillColor(sf::Color::Transparent);
      cell.setOutlineColor(sf::Color::Green);
      cell.setOutlineThickness(-10);
      window.draw(cell);
    }
  }
  if (game_over) {
    sf::Text text;
    text.setFont(font);
    text.setString(sf::String((game_res) + " wins! Press 'R' to restart"));
    text.setPosition(sf::Vector2f(50, start_y));
    text.setFillColor(sf::Color::White);
    window.draw(text);
  }
}

void HandleEvents(sf::Event event) {
  if (event.type == sf::Event::Closed) {
    window.close();
  } else if (event.type == sf::Event::KeyReleased) {
    if (event.key.code == sf::Keyboard::R) {
      ResetField();
    } else if (event.key.code == sf::Keyboard::Num1) {
      cur_mode = 1;
      delete bot;
      bot = new Newbie;
      ResetField();
    } else if (event.key.code == sf::Keyboard::Num2) {
      cur_mode = 2;
      delete bot;
      bot = new MiddleMan;
      ResetField();
    } else if (event.key.code == sf::Keyboard::Num3) {
      cur_mode = 3;
      delete bot;
      bot = new Hardcore;
      ResetField();
    }

  } else if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
          sf::IntRect collision(
              sf::Vector2i(start_x + j * cell_size, start_y + i * cell_size),
              sf::Vector2i(cell_size, cell_size));
          if (collision.contains(sf::Mouse::getPosition(window))) {
            if (IsCellBusy(i, j, units)) {
              auto unit = GetUnit(i, j, units);
              if (unit->is_black_ != white_turn) {
                bool can_do_turn = true, nec = false;

                auto necessary_turn = GetUnitsToDoTurnNecessarily();
                for (auto val : necessary_turn) {
                  if (val->i_ == i && val->j_ == j) {
                    nec = true;
                  }
                  can_do_turn = nec;
                }

                if (can_do_turn) {
                  selected_unit = unit;
                  highlighted = GetAchievable(*unit);
                }
              }
            } else {
              bool do_turn = false, with_enemy;
              for (auto val : highlighted) {
                if (val.i_ == i && val.j_ == j) {
                  do_turn = true;
                  with_enemy = val.is_black_;
                  break;
                }
              }
              if (!do_turn) break;
              if (MoveUnit(selected_unit, Unit(with_enemy, false, i, j),
                           units)) {
                if (CheckForWin(units)) {
                  game_over = true;
                  game_res = "Heavens";
                }

                white_turn = !white_turn;
                highlighted.clear();
              } else {
                highlighted = GetAchievable(*selected_unit);
              }

              DrawField();

              if (!white_turn && !game_over) {
                bot->DoTurn();
                if (CheckForWin(units)) {
                  game_over = true;
                  game_res = "Hell";
                }
                white_turn = !white_turn;
              }
            }
          }
        }
      }
    }
  }
}

int main() {
  window.setVerticalSyncEnabled(true);
  window.setActive(true);

  white_checker_texture.loadFromFile("white_checker.jpg");
  white_piece_texture.loadFromFile("white_piece.png");
  white_queen_texture.loadFromFile("white_queen.png");
  black_checker_texture.loadFromFile("black_checker.jpg");
  black_piece_texture.loadFromFile("black_piece.png");
  black_queen_texture.loadFromFile("black_queen.png");


  ResetField();
  bot = new MiddleMan;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      HandleEvents(event);
    }

    window.clear(sf::Color::Black);
    DrawField();
    window.display();
  }

  return 0;
}
