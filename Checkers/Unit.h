#pragma once
class Unit {
 public:
  Unit(int i, int j, bool is_black, bool is_queen = false)
      : i_(i), j_(j), is_black_(is_black), is_queen_(is_queen) {}

  int i() const { return i_; }
  int j() const { return j_; }
  void set_i(int i) { i_ = i; }
  void set_j(int j) { j_ = j; }
  bool black() const { return is_black_; }
  bool queen() const { return is_queen_; }
  void set_queen(bool queen) { is_queen_ = queen; }

 private:
  int i_, j_;
  bool is_black_, is_queen_;
};
