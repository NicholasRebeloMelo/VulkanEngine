#pragma once

#include "ve_gameobject.hpp"
#include "Window.hpp"

namespace ve {
    class MouseMovementController {
    public:
        void look(GLFWwindow *window, float dt, VeGameObject &gameObject);

        float lookSpeed{ 0.05f };
    };
}