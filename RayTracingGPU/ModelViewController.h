#pragma once

#include <Utils/Glm.h>

class ModelViewController final
{
public:
    void Reset(const glm::mat4& modelView);

    glm::mat4 ModelView() const;

    bool OnKey(int key, int scancode, int action, int mods);
    bool OnCursorPosition(double xpos, double ypos);
    bool OnMouseButton(int button, int action, int mods);
    bool UpdateCamera(double speed, double timeDelta);

private:
    void MoveForward(float d);
    void MoveRight(float d);
    void MoveUp(float d);
    void Rotate(float y, float x);
    void UpdateVectors();

    // Matrices and vectors.
    glm::mat4 m_Orientation{};

    glm::vec4 m_Position{};
    glm::vec4 m_Right{1, 0, 0, 0};
    glm::vec4 m_Up{0, 1, 0, 0};
    glm::vec4 m_Forward{0, 0, -1, 0};

    // Control states.
    bool m_CameraMovingLeft{};
    bool m_CameraMovingRight{};
    bool m_CameraMovingBackward{};
    bool m_CameraMovingForward{};
    bool m_CameraMovingDown{};
    bool m_CameraMovingUp{};

    float m_CameraRotX{};
    float m_CameraRotY{};
    float m_ModelRotX{};
    float m_ModelRotY{};

    double m_MousePosX{};
    double m_MousePosY{};

    bool m_MouseLeftPressed{};
    bool m_MouseRightPressed{};
};
