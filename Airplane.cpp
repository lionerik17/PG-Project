#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Airplane {
private:
    glm::vec3 position;          // Position of the airplane
    glm::vec3 velocity;          // Velocity of the airplane
    glm::vec3 forwardDirection;  // Forward direction of the airplane
    glm::vec3 rightDirection;    // Right direction of the airplane
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

    void accelerate() {
        if (speed < maxSpeed) {
            speed += acceleration * deltaTime;
        }
    }

    void applyDrag() {
        if (speed > minSpeed) {
            speed *= drag;
        }
        else {
            speed = 0.0f;
        }
    }

public:
    Airplane(glm::vec3 startPosition, glm::mat4 initialModelMatrix, GLuint shaderModelLoc, float groundY = 3.0f, float gravityAccel = -9.8f)
        : position(startPosition), velocity(0.0f), forwardDirection(glm::vec3(1.0f, 0.0f, 0.0f)),
        rightDirection(glm::vec3(0.0f, 0.0f, 1.0f)), gravity(gravityAccel), modelMatrix(initialModelMatrix),
        modelMatrixLoc(shaderModelLoc), groundLevel(groundY), speed(0.0f), maxSpeed(50.0f), minSpeed(0.0f),
        acceleration(10.0f), liftThreshold(15.0f) {}

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

    void moveBackward(float speed) {
        position -= forwardDirection * speed * deltaTime;
        updateModelMatrix();
    }

    void moveRight(float speed) {
        position += rightDirection * speed * deltaTime;
        updateModelMatrix();
    }

    void moveLeft(float speed) {
        position -= rightDirection * speed * deltaTime;
        updateModelMatrix();
    }

    void updateModelMatrix() {
        modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void updateShader() const {
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    glm::vec3 getPosition() const {
        return position;
    }

    float getSpeed() const {
        return speed;
    }
};
