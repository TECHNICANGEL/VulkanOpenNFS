#pragma once

#ifndef __ANDROID__
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#else
#include <GLES3/gl3.h>
#endif
#include <memory>

namespace OpenNFS {
    enum WindowStatus : uint8_t {
        UI,
        GAME
    };

    class InputManager {
    public:
        struct Inputs {
            bool accelerate{};
            bool reverse{};
            bool brakes{};
            bool left{};
            bool right{};
            bool reset{};
            bool cameraForwards{};
            bool cameraBackwards{};
            bool cameraLeft{};
            bool cameraRight{};
            double cursorX{};
            double cursorY{};
            bool mouseLeft{};
            bool mouseRight{};
        };

#ifndef __ANDROID__
        explicit InputManager(std::shared_ptr<GLFWwindow> const &window);
#else
        explicit InputManager(std::shared_ptr<void> const &window);
#endif
        void Scan();
        void ResetCursorPosition() const;
        [[nodiscard]] WindowStatus GetWindowStatus() const;

        Inputs inputs{};


      private:
#ifndef __ANDROID__
        std::shared_ptr<GLFWwindow> m_window;
#else
        std::shared_ptr<void> m_window;
#endif
        WindowStatus m_windowStatus {GAME};
    };
} // namespace OpenNFS
