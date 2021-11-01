#include "openglwindow.hpp"

#include <imgui.h>

#include <glm/gtx/rotate_vector.hpp>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.set(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.button.button == SDL_BUTTON_RIGHT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Up));
  }
  if (event.type == SDL_MOUSEMOTION) {
    glm::ivec2 mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    glm::vec2 direction{glm::vec2{mousePosition.x - m_viewportWidth / 2,
                                  mousePosition.y - m_viewportHeight / 2}};
    direction.y = -direction.y;
    // m_player.setRotation(std::atan2(direction.y, direction.x) - M_PI_2);
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;

  m_player.initializeGL(m_objectsProgram);
  m_ball.initializeGL(m_objectsProgram);
  m_bricks.initializeGL(m_objectsProgram, m_bricks.lines, m_bricks.columns);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 5) {
    restart();
    return;
  }

  // Wall and ceil collision
  if (m_ball.m_translation.x > +1.0f)
    m_ball.m_velocity.x = -abs(m_ball.m_velocity.x);
  if (m_ball.m_translation.x < -1.0f)
    m_ball.m_velocity.x = +abs(m_ball.m_velocity.x);
  if (m_ball.m_translation.y > +1.0f)
    m_ball.m_velocity.y = -abs(m_ball.m_velocity.y);

  // Paddle collsion
  const auto yDistance{m_ball.m_translation.y - m_player.m_translation.y};
  const auto isInPaddleRange =
      m_ball.m_translation.x > m_player.m_translation.x - 0.14f &&
      m_ball.m_translation.x < m_player.m_translation.x + 0.14f;

  if (yDistance < 0.01f && isInPaddleRange) {
    float xDist = m_player.m_translation.x - m_ball.m_translation.x;
    float angleComponent = xDist * glm::radians(75.0f) / 0.14;
    glm::vec2 upComponent = glm::vec2{0.0f, 0.5f};
    glm::vec2 updatedVelocity{glm::rotate(upComponent, angleComponent)};

    m_ball.m_velocity = glm::vec2{updatedVelocity.x, abs(updatedVelocity.y)};
  }

  // Brick collision
  for (auto &brick : m_bricks.m_bricks) {
    float xDist = brick.m_translation.x - m_ball.m_translation.x;
    float yDist = brick.m_translation.y - m_ball.m_translation.y;

    // bottom collision
    if (yDist < 0.04f && yDist > 0 && xDist < 0.08) {
      glm::vec2 upComponent = glm::vec2{0.0f, 0.5f};
      glm::vec2 updatedVelocity{glm::rotate(upComponent, glm::radians(180.0f))};
      m_ball.m_velocity =
          glm::vec2{m_ball.m_velocity.x, -abs(m_ball.m_velocity.y)};
      brick.resistance--;
    }

    // top collision
    if (yDist < 0.04f && yDist < 0 && xDist < 0.08) {
      glm::vec2 upComponent = glm::vec2{0.0f, 0.5f};
      glm::vec2 updatedVelocity{glm::rotate(upComponent, glm::radians(180.0f))};
      m_ball.m_velocity =
          glm::vec2{m_ball.m_velocity.x, +abs(m_ball.m_velocity.y)};
    }

    // left collision
    if (yDist < 0.04f && xDist < 0.08 && xDist < 0) {
      glm::vec2 upComponent = glm::vec2{0.0f, 0.5f};
      glm::vec2 updatedVelocity{glm::rotate(upComponent, glm::radians(180.0f))};
      m_ball.m_velocity =
          glm::vec2{-abs(m_ball.m_velocity.x), m_ball.m_velocity.y};
    }

    // right collision
    if (yDist < 0.04f && xDist < 0.08 && xDist > 0) {
      glm::vec2 upComponent = glm::vec2{0.0f, 0.5f};
      glm::vec2 updatedVelocity{glm::rotate(upComponent, glm::radians(180.0f))};
      m_ball.m_velocity =
          glm::vec2{abs(m_ball.m_velocity.x), m_ball.m_velocity.y};
    }
  }

  m_player.update(m_gameData, deltaTime);
  m_ball.update(m_player, deltaTime);
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_player.paintGL(m_gameData);
  m_ball.paintGL();
  m_bricks.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(300, 85)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_player.terminateGL();
  m_ball.terminateGL();
  m_bricks.terminateGL();
}
