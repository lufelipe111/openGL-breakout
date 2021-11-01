#include "bricks.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Bricks::initializeGL(GLuint program, int lines, int columns) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  int quantity = lines * columns;

  m_bricks.clear();
  m_bricks.resize(quantity);

  glm::vec2 gap = glm::vec2{2.8f, 1.5f};
  glm::vec2 offset = glm::vec2{0.4f, 0.2f};
  int idx = 0;
  for (int i = 0; i < lines; i++) {
    for (int j = 0; j < columns; j++) {
      m_bricks[idx] = createBrick(
          glm::vec2{gap.x * 0.04 * j - offset.x, gap.y * 0.04f * i + offset.y});
      idx++;
    }
  }
}

void Bricks::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  for (int i = 0; i < lines * columns; i++) {
    abcg::glBindVertexArray(m_bricks[i].m_vao);

    abcg::glUniform1f(m_scaleLoc, m_bricks[i].m_scale);
    abcg::glUniform2fv(m_translationLoc, 1, &m_bricks[i].m_translation.x);

    abcg::glUniform4fv(m_colorLoc, 1, &m_bricks[i].m_color.r);
    abcg::glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);
  }

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

Bricks::Brick Bricks::createBrick(glm::vec2 translation) {
  Brick brick;

  brick.resistance = 3;
  brick.m_translation = translation;

  switch (brick.resistance) {
    case 1:
      brick.m_color = glm::vec4{1, 0, 0, 1};  // red
      break;
    case 2:
      brick.m_color = glm::vec4{0, 1, 0, 1};  // green
      break;
    case 3:
      brick.m_color = glm::vec4{0, 0, 1, 1};  // blue
      break;

    default:
      brick.m_color = glm::vec4{1, 1, 1, 1};  // white
      break;
  }

  brick.m_scale = 0.04f;

  // create geometry
  std::array<glm::vec2, 4> positions{
      // player paddle
      glm::vec2{-brick.width / 2, +brick.height / 2},
      glm::vec2{+brick.width / 2, +brick.height / 2},
      glm::vec2{+brick.width / 2, -brick.height / 2},
      glm::vec2{-brick.width / 2, -brick.height / 2},
  };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{brick.width / 2, brick.width / 2};
  }

  const std::array indices{0, 1, 2, 0, 2, 3};

  // Generate VBO
  abcg::glGenBuffers(1, &brick.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, brick.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &brick.m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, brick.m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &brick.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(brick.m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, brick.m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, brick.m_ebo);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return brick;
}

void Bricks::terminateGL() {
  for (auto brick : m_bricks) {
    abcg::glDeleteBuffers(1, &brick.m_vbo);
    abcg::glDeleteVertexArrays(1, &brick.m_vao);
    abcg::glDeleteVertexArrays(1, &brick.m_ebo);
  }
}