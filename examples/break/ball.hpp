#ifndef BALL_HPP_
#define BALL_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "player.hpp"

class OpenGLWindow;

class Ball {
 public:
  void initializeGL(GLuint program);
  void paintGL();
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

  GLuint m_vao{};
  GLuint m_vbo{};

  glm::vec4 m_color{1};
  bool m_hit{false};
  int m_polygonSides{};
  float m_scale{};
  glm::vec2 m_translation{glm::vec2{0.0f, -0.8f}};
  glm::vec2 m_velocity{glm::vec2(0)};
  glm::vec2 m_direction{glm::vec2(0)};

  std::default_random_engine m_randomEngine;
};

#endif