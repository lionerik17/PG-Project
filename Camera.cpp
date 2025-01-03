#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace gps {

    // Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        // Calculate the camera front direction
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        // Right direction is perpendicular to the front and up direction
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
        // Recalculate up direction to ensure orthogonality
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraRightDirection, this->cameraFrontDirection));

        // Initialize yaw and pitch
        this->yaw = 0.0f;
        this->pitch = 0.0f;
    }

    // Return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    glm::vec3 Camera::getPosition() const {
        return cameraPosition;
    }

    float Camera::getPitch() const {
        return pitch;
    }

    float Camera::getYaw() const {
        return yaw;
    }

    void Camera::setPosition(const glm::vec3& position) {
        cameraPosition = position;

        // Update the camera target to maintain consistency
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::setTarget(const glm::vec3& target) {
        cameraTarget = target;

        // Recalculate the camera front direction based on the new target
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);

        // Recalculate the right and up vectors to maintain orthogonality
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    // Update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        }

        // Update the camera target based on the new position and front direction
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    // Update the camera internal parameters following a camera rotate event
    // yaw - camera rotation around the y axis
    // pitch - camera rotation around the x axis
    // Class members to store yaw and pitch

    // Update the camera internal parameters following a camera rotate event
    // yawOffset - camera rotation around the y axis
    // pitchOffset - camera rotation around the x axis
    void Camera::rotate(float pitchOffset, float yawOffset) {
        // Update the yaw and pitch angles
        yaw += yawOffset;
        pitch += pitchOffset;

        // Clamp the pitch value to prevent flipping
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        // Calculate the new front vector using spherical coordinates
        glm::vec3 front;
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

        cameraFrontDirection = glm::normalize(front);

        // Recalculate the right and up vectors to maintain orthogonality
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}
