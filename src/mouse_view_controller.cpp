#include "mouse_view_controller.hpp"

// GLFW
#include <GLFW/glfw3.h>

// glm 
#include <glm/gtc/matrix_transform.hpp>

namespace ve {


	void MouseViewController::updateFromMouse(GLFWwindow* window, float dt, VeGameObject& gameObject)
	{

        // get current cursor position
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        // calculate offset from previous position 
        static double lastXpos = xPos;
        static double lastYpos = yPos;
        double deltaX = xPos - lastXpos;
        double deltaY = yPos - lastYpos;

        // update last position for next frame
        lastXpos = xPos;
        lastYpos = yPos;

        // convert offset to rotation based on sensitivity
        float rotX = -deltaY * sensitivity.lookSpeed * dt;
        float rotY = deltaX * sensitivity.lookSpeed * dt;

        // apply rotation to camera 
        gameObject.transform.rotation += glm::vec3(rotX, rotY, 0.0f);

        // limit pitch value 
        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
	}

}
