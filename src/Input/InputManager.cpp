#include "InputManager.h"

#include "../Config.h"

namespace OpenNFS {
#ifndef __ANDROID__
    InputManager::InputManager(std::shared_ptr<GLFWwindow> const &window) : m_window(window) {
    }
#else
    InputManager::InputManager(std::shared_ptr<void> const &window) : m_window(window) {
    }
#endif

    void InputManager::Scan() {
#ifndef __ANDROID__
        glfwPollEvents();
        inputs.accelerate = glfwGetKey(m_window.get(), GLFW_KEY_W) == GLFW_PRESS;
        inputs.reverse = glfwGetKey(m_window.get(), GLFW_KEY_S) == GLFW_PRESS;
        inputs.brakes = glfwGetKey(m_window.get(), GLFW_KEY_SPACE) == GLFW_PRESS;
        inputs.right = glfwGetKey(m_window.get(), GLFW_KEY_D) == GLFW_PRESS;
        inputs.left = glfwGetKey(m_window.get(), GLFW_KEY_A) == GLFW_PRESS;
        inputs.reset = glfwGetKey(m_window.get(), GLFW_KEY_R) == GLFW_PRESS;

        inputs.cameraForwards = inputs.accelerate;
        inputs.cameraBackwards = inputs.reverse;
        inputs.cameraLeft = inputs.left;
        inputs.cameraRight = inputs.right;
        inputs.mouseLeft = (glfwGetMouseButton(m_window.get(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
        inputs.mouseRight = (glfwGetMouseButton(m_window.get(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
        glfwGetCursorPos(m_window.get(), &inputs.cursorX, &inputs.cursorY);

        // Detect a click on the 3D Window by detecting a click that isn't on ImGui
        if (inputs.mouseLeft &&
            !ImGui::GetIO().WantCaptureMouse) {
            m_windowStatus = GAME;
            ImGui::GetIO().MouseDrawCursor = false;
        } else if (glfwGetKey(m_window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            m_windowStatus = UI;
            ImGui::GetIO().MouseDrawCursor = true;
        }
#endif
    }

    void InputManager::ResetCursorPosition() const {
#ifndef __ANDROID__
        glfwSetCursorPos(m_window.get(), Config::get().windowSizeX / 2, Config::get().windowSizeY / 2);
#endif
    }

    WindowStatus InputManager::GetWindowStatus() const {
        return m_windowStatus;
    }
} // namespace OpenNFS
