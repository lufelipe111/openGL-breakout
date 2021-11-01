#ifndef BRICK_HPP_
#define BRICK_HPP_

#include <random>
#include <vector>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "player.hpp"

class OpenGLWindow;

class Bricks {
 public:
  void initializeGL(GLuint program, int lines, int columns);
  void paintGL(const GameData &gameData);
  void terminateGL();
  void update(const Player &player, float deltaTime);

 private:
  friend OpenGLWindow;
  friend Player;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  int lines = 8;
  int columns = 8;

  struct Brick {
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};
    int resistance{1};
    bool m_hit{false};
    float m_scale{0.025f};
    glm::vec4 m_color{glm::vec4(1)};
    glm::vec2 m_translation{};

    float width{7.0f};
    float height{3.0f};
  };

  float m_scale{0.015f};
  std::vector<Brick> m_bricks;

  Bricks::Brick createBrick(glm::vec2 translation);
};

#endif