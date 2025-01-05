#include "WindowsInput.h"
#include "../../Molecular/Core/App.h"
#include <GLFW/glfw3.h>

namespace Molecular
{
    bool Input::IsKeyPressed(int keycode)
    {
        auto* window = static_cast<GLFWwindow*>(App::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPressed(int button)
    {
        auto* window = static_cast<GLFWwindow*>(App::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    // glm::vec2 Input::GetMousePosition()
    // {
    //     auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    //     double xpos, ypos;
    //     glfwGetCursorPos(window, &xpos, &ypos);
    //
    //     return { (float)xpos, (float)ypos };
    // }

    float Input::GetMouseX()
    {
        auto* window = static_cast<GLFWwindow*>(App::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return (float)xpos;
    }

    float Input::GetMouseY()
    {
        auto* window = static_cast<GLFWwindow*>(App::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return (float)ypos;
    }
}