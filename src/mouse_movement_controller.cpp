#include "mouse_movement_controller.hpp"

namespace ve {

    void MouseMovementController::look(GLFWwindow *window, float dt, VeGameObject &gameObject) {
        // Get mouse position and delta
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Calculate mouse movement
        static float lastX = xpos, lastY = ypos;
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        // Sensitivity adjustment (you can customize this)
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // Check if right mouse button is pressed
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            // Rotate the camera based on mouse movement
            gameObject.transform.rotation.y += xoffset * lookSpeed;
            gameObject.transform.rotation.x += yoffset * lookSpeed;

            // Clamp pitch to prevent inverted view
            gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        }
    }
}