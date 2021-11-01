#include <fmt/core.h>

#include "abcg.hpp"
#include "openglwindow.hpp"

int main(int argc, char **argv) {
  try {
    // Create application instance
    abcg::Application app(argc, argv);

    // Create openGL window
    auto window{std::make_unique<OpenGLWindow>()};
    window->setOpenGLSettings(
        {.samples = 2, .preserveWebGLDrawingBuffer = true});
    window->setWindowSettings(
        {.width = 600, .height = 600, .title = "Regular Polygons"});

    // run application
    app.run(std::move(window));
  } catch (abcg::Exception &exception) {
    fmt::print("{}\n", exception.what());
    return -1;
  }
  return 0;
}