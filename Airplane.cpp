#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Airplane {
private:
    glm::vec3 position;          // Position of the airplane
    glm::vec3 velocity;          // Velocity of the airplane
    glm::vec3 forwardDirection;  // Forward direction of the airplane
    glm::vec3 rightDirection;    // Right direction of the airplane
    glm::vec3 upDirection;       // Up direction of the airplane
    glm::mat4 modelMatrix;       // Model transformation matrix
    GLuint modelMatrixLoc;       // Shader location for model matrix
    float gravity;               // Gravitational acceleration
    float groundLevel;           // Ground level
    float speed;                 // Current speed of the airplane
    float minSpeed;              // Minimum speed for the airplane
    float maxSpeed;              // Maximum speed for the airplane
    float acceleration;          // Rate of acceleration
    float liftThreshold;         // Speed at which the airplane generates lift
    float deltaTime = 0.016f;    // Time step for updates
    float drag = 0.98f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float bankingSpeed = 15.0f;
    float maxBankingAngle = 45.0f;

    void accelerate() {
        speed += acceleration * deltaTime;
    }

    void decelerate() {
        speed -= acceleration * deltaTime;
    }

    void applyDrag() {
        speed *= drag;

        velocity.x *= drag;
        velocity.z *= drag;
    }

public:
    Airplane(glm::vec3 startPosition, glm::mat4 initialModelMatrix, GLuint shaderModelLoc, float groundY = 3.0f, float gravityAccel = -9.8f)
        : position(startPosition), velocity(0.0f), forwardDirection(glm::vec3(1.0f, 0.0f, 0.0f)),
        rightDirection(glm::vec3(0.0f, 0.0f, 1.0f)), upDirection(glm::vec3(0.0f, 1.0f, 0.0f)), gravity(gravityAccel), 
        modelMatrix(initialModelMatrix), modelMatrixLoc(shaderModelLoc), groundLevel(groundY), speed(0.0f),
        maxSpeed(50.0f), minSpeed(0.0f), acceleration(10.0f), liftThreshold(15.0f) {}

    void applyGravity() {
        velocity.y += gravity * deltaTime;
        position.y += velocity.y * deltaTime;

        if (position.y <= groundLevel) {
            position.y = groundLevel;
            velocity.y = 0.0f;
        }

        updateModelMatrix();
    }

    void moveForward(bool isAccelerating) {
        if (isAccelerating) {
            accelerate();
            if (speed >= liftThreshold) {
                velocity.y += (speed - liftThreshold) * 0.01f;
            }
        }
        else {
            applyDrag();
        }
        position += forwardDirection * speed * deltaTime;
        updateModelMatrix();
    }

    void moveBackward(bool isAccelerating) {
        if (isAccelerating) {
            decelerate();
        }
        else {
            applyDrag();
        }
        position -= -forwardDirection * speed * deltaTime;
        updateModelMatrix();
    }

    void turnLeft() {
        roll += bankingSpeed * deltaTime;

        yaw -= bankingSpeed * deltaTime;
        updateOrientation();
        updateModelMatrix();
    }

    void turnRight() {
        roll -= bankingSpeed * deltaTime;

        yaw += bankingSpeed * deltaTime;
        updateOrientation();
        updateModelMatrix();
    }

    void levelRoll() {
        if (roll > 0.0f) {
            roll -= bankingSpeed * deltaTime;
            if (roll < 0.0f) roll = 0.0f;
        }
        else if (roll < 0.0f) {
            roll += bankingSpeed * deltaTime;
            if (roll > 0.0f) roll = 0.0f;
        }
        updateModelMatrix();
    }

    void updateModelMatrix() {
        modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(roll), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void updateOrientation() {
        glm::vec3 front;
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        forwardDirection = glm::normalize(front);

        rightDirection = glm::normalize(glm::cross(forwardDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        upDirection = glm::normalize(glm::cross(rightDirection, forwardDirection));
    }

    void updateShader() const {
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    glm::vec3 getPosition() const {
        return position;
    }

    glm::vec3 getForwardDirection() const {
        return forwardDirection;
    }

    float getSpeed() const {
        return speed;
    }
};
