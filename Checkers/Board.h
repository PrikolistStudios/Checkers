#pragma once
#include <SFML/Graphics.hpp>
#include "Unit.h"
#include <vector>
#include <list>
#include <iostream>

class Board : public sf::Drawable {
 public:
  Board(int field_size_pixels, int field_size = 8, int black_rows = 3,
        int white_rows = 3)
      : field_size_pixels_(field_size_pixels),
        field_size_(field_size),
        cell_size_pixels_(field_size_pixels_ / field_size_) {
    for (int i = 0; i < field_size_; ++i) {
      for (int j = (i + 1) % 2; j < field_size_; j += 2) {
        if (i < black_rows) {
          units_.push_back(new Unit(i, j, true));
        } else if (i >= field_size_ - white_rows) {
          units_.push_back(new Unit(i, j, false));
        }
      }
    }
  }

  Board(std::list<Unit*>& units, int field_size_pixels, int field_size = 8,
        int black_rows = 3, int white_rows = 3)
      : units_(units),
        field_size_pixels_(field_size_pixels),
        field_size_(field_size),
        cell_size_pixels_(field_size_pixels_ / field_size_),
        start_x_() {}



  void virtual draw(sf::RenderTarget& target, sf::RenderStates states) const override {

    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        sf::RectangleShape cell;
        cell.setFillColor((i + j) % 2 ? sf::Color(0xAA761BFF)
                                      : sf::Color(0xF6DBADFF));
        cell.setPosition(
            sf::Vector2f(j * cell_size_pixels_, i * cell_size_pixels_));
        cell.setSize(sf::Vector2f(cell_size_pixels_, cell_size_pixels_));
        target.draw(cell, states);
      }
    }

    for (auto val : units_) {
      int border_indent = 10;
      sf::Vector2f pos(border_indent + cell_size_pixels_ * (val->j()),
                       border_indent + cell_size_pixels_ * (val->i()));
      sf::CircleShape circle;
      circle.setFillColor((val->black()) ? sf::Color::Black : sf::Color::White);
      circle.setPosition(pos);
      circle.setRadius(cell_size_pixels_ / 2 - border_indent);
      if (val->queen()) {
        circle.setOutlineColor(sf::Color::Magenta);
        circle.setOutlineThickness(10);
      }
      target.draw(circle);
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

    //sf::Text bot_name;
    //bot_name.setFont(font);
    //bot_name.setString(bot->name());
    //bot_name.setFillColor(sf::Color::White);
    //bot_name.setPosition(sf::Vector2f(start_x, start_y - 100));
    //bot_name.setCharacterSize(30);
    //window.draw(bot_name);

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
      auto necessary_turn = GetUnitsToDoTurnNecessarily();
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
    if (game_over_) {
      sf::Text text;
      text.setFont(font);
      text.setString(sf::String(game_res + " wins! Press 'R' to restart"));
      text.setPosition(sf::Vector2f(30, 30));
      text.setFillColor(sf::Color::White);
      window.draw(text);
    }
  }

  ~Board() {
    for (auto val : units_) {
      delete val;
    }
  }
 private:
  int field_size_, field_size_pixels_, start_x_, start_y_;
  double cell_size_pixels_;

  bool game_over_ = false, black_wins_;

  std::list<Unit*> units_;

  Unit* selected_unit_;
};
