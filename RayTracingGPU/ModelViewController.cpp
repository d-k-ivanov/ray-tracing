#include "ModelViewController.h"

#define NOMINMAX
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef APIENTRY

void ModelViewController::Reset(const glm::mat4& modelView)
{
    const auto inverse = glm::inverse(modelView);

    m_Position    = inverse * glm::vec4(0, 0, 0, 1);
    m_Orientation = glm::mat4(glm::mat3(modelView));

    m_CameraRotX = 0;
    m_CameraRotY = 0;
    m_ModelRotX  = 0;
    m_ModelRotY  = 0;

    m_MouseLeftPressed  = false;
    m_MouseRightPressed = false;

    UpdateVectors();
}

glm::mat4 ModelViewController::ModelView() const
{
    const auto cameraRotX = static_cast<float>(m_ModelRotY / 300.0);
    const auto cameraRotY = static_cast<float>(m_ModelRotX / 300.0);

    const auto model = glm::rotate(glm::mat4(1.0f), cameraRotY * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f))
                       * glm::rotate(glm::mat4(1.0f), cameraRotX * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    const auto view = m_Orientation * glm::translate(glm::mat4(1), -glm::vec3(m_Position));

    return view * model;
}

bool ModelViewController::OnKey(const int key, const int scancode, const int action, const int mods)
{
    switch(key)
    {
        case GLFW_KEY_S:
            m_CameraMovingBackward = action != GLFW_RELEASE;
            return true;
        case GLFW_KEY_W:
            m_CameraMovingForward = action != GLFW_RELEASE;
            return true;
        case GLFW_KEY_A:
            m_CameraMovingLeft = action != GLFW_RELEASE;
            return true;
        case GLFW_KEY_D:
            m_CameraMovingRight = action != GLFW_RELEASE;
            return true;
        case GLFW_KEY_LEFT_CONTROL:
            m_CameraMovingDown = action != GLFW_RELEASE;
            return true;
        case GLFW_KEY_LEFT_SHIFT:
            m_CameraMovingUp = action != GLFW_RELEASE;
            return true;
        default:
            return false;
    }
}

bool ModelViewController::OnCursorPosition(const double xpos, const double ypos)
{
    const auto deltaX = static_cast<float>(xpos - m_MousePosX);
    const auto deltaY = static_cast<float>(ypos - m_MousePosY);

    if(m_MouseLeftPressed)
    {
        m_CameraRotX += deltaX;
        m_CameraRotY += deltaY;
    }

    if(m_MouseRightPressed)
    {
        m_ModelRotX += deltaX;
        m_ModelRotY += deltaY;
    }

    m_MousePosX = xpos;
    m_MousePosY = ypos;

    return m_MouseLeftPressed || m_MouseRightPressed;
}

bool ModelViewController::OnMouseButton(const int button, const int action, const int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        m_MouseLeftPressed = action == GLFW_PRESS;
    }

    if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        m_MouseRightPressed = action == GLFW_PRESS;
    }

    return true;
}

bool ModelViewController::UpdateCamera(const double speed, const double timeDelta)
{
    const auto d = static_cast<float>(speed * timeDelta);

    if(m_CameraMovingLeft)
        MoveRight(-d);
    if(m_CameraMovingRight)
        MoveRight(d);
    if(m_CameraMovingBackward)
        MoveForward(-d);
    if(m_CameraMovingForward)
        MoveForward(d);
    if(m_CameraMovingDown)
        MoveUp(-d);
    if(m_CameraMovingUp)
        MoveUp(d);

    constexpr float rotationDiv = 300;
    Rotate(m_CameraRotX / rotationDiv, m_CameraRotY / rotationDiv);

    const bool updated = m_CameraMovingLeft || m_CameraMovingRight || m_CameraMovingBackward || m_CameraMovingForward
                         || m_CameraMovingDown || m_CameraMovingUp || m_CameraRotY != 0 || m_CameraRotX != 0;

    m_CameraRotY = 0;
    m_CameraRotX = 0;

    return updated;
}

void ModelViewController::MoveForward(const float d)
{
    m_Position += d * m_Forward;
}

void ModelViewController::MoveRight(const float d)
{
    m_Position += d * m_Right;
}

void ModelViewController::MoveUp(const float d)
{
    m_Position += d * m_Up;
}

void ModelViewController::Rotate(const float y, const float x)
{
    m_Orientation = glm::rotate(glm::mat4(1), x, glm::vec3(1, 0, 0)) * m_Orientation
                    * glm::rotate(glm::mat4(1), y, glm::vec3(0, 1, 0));

    UpdateVectors();
}

void ModelViewController::UpdateVectors()
{
    // Given the ortientation matrix, find out the x,y,z vector orientation.
    const auto inverse = glm::inverse(m_Orientation);

    m_Right   = inverse * glm::vec4(1, 0, 0, 0);
    m_Up      = inverse * glm::vec4(0, 1, 0, 0);
    m_Forward = inverse * glm::vec4(0, 0, -1, 0);
}
