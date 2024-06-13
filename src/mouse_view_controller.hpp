#pragma once

#include "ve_gameobject.hpp"
#include "Window.hpp"

namespace ve {
    class MouseViewController {
    public:
        struct Sensitivity {
            float lookSpeed = 0.5f;
        };

        // update camera based on mouse movement
        void updateFromMouse(GLFWwindow* window, float dt, VeGameObject& gameObject);

        Sensitivity sensitivity;
    };
}