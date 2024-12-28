#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BoundingBox.h"

class Airplane {
private:
    glm::vec3 position;           // Position of the airplane
    glm::vec3 velocity;           // Velocity of the airplane
    glm::vec3 forwardDirection;   // Forward direction of the airplane
    glm::vec3 rightDirection;     // Right direction of the airplane
    glm::vec3 upDirection;        // Up direction of the airplane
    glm::mat4 modelMatrix;        // Model transformation matrix
    GLuint modelMatrixLoc;        // Shader location for model matrix
    BoundingBox boundingBox;      // Airplane bounding box    
    float gravity;                // Gravitational acceleration
    float groundLevel;            // Ground level
    float speed;                  // Current speed of the airplane
    float minSpeed;               // Minimum speed for the airplane
    float maxSpeed;               // Maximum speed for the airplane
    float acceleration;           // Rate of acceleration
    float liftThreshold;          // Speed at which the airplane generates lift
    float deltaTime = 0.016f;     // Time step for updates
    float drag = 0.98f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float bankingSpeed = 15.0f;
    float maxBankingAngle = 15.0f;
    float maxYawAngle = 15.0f;
    BoundingBox originalBoundingBox;

    void accelerate() {
        speed += acceleration * deltaTime;
        speed = glm::clamp(speed, -maxSpeed, maxSpeed);
    }

    void decelerate() {
        speed -= acceleration * deltaTime;
        speed = glm::clamp(speed, -maxSpeed, maxSpeed);
    }

    void applyDrag() {
        speed *= drag;
        velocity.x *= drag;
        velocity.z *= drag;
    }

public:
    Airplane(glm::vec3 startPosition, glm::mat4 initialModelMatrix, GLuint shaderModelLoc, BoundingBox initialBoundingBox, float groundY = 3.0f, float gravityAccel = -9.8f)
        : position(startPosition), velocity(0.0f), forwardDirection(glm::vec3(1.0f, 0.0f, 0.0f)),
        rightDirection(glm::vec3(0.0f, 0.0f, 1.0f)), upDirection(glm::vec3(0.0f, 1.0f, 0.0f)), gravity(gravityAccel), 
        modelMatrix(initialModelMatrix), modelMatrixLoc(shaderModelLoc), boundingBox(initialBoundingBox), originalBoundingBox(initialBoundingBox),
        groundLevel(groundY), speed(0.0f), maxSpeed(50.0f), minSpeed(0.0f), acceleration(10.0f), liftThreshold(15.0f) {}

    void applyGravity() {
        velocity.y += gravity * deltaTime;
        position += velocity * deltaTime;

        if (position.y <= groundLevel) {
            position.y = groundLevel;
            velocity.y = 0.0f;
        }

        updateModelMatrix();
    }

    void moveForward(bool isAccelerating) {
        if (isAccelerating) {
            accelerate();
            //velocity = forwardDirection * speed;
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
        if (roll > maxBankingAngle) roll = maxBankingAngle;

        yaw -= bankingSpeed * deltaTime;
        yaw = glm::clamp(yaw, -maxYawAngle, maxYawAngle);
        std::cout << "Yaw: " << yaw << " degrees\n"; // Debug output
        updateOrientation();
        updateModelMatrix();
    }

    void turnRight() {
        roll -= bankingSpeed * deltaTime;
        if (roll < -maxBankingAngle) roll = -maxBankingAngle;

        yaw += bankingSpeed * deltaTime;
        yaw = glm::clamp(yaw, -maxYawAngle, maxYawAngle);
        std::cout << "Yaw: " << yaw << " degrees\n"; // Debug output
        //if (yaw < -bankingSpeed) yaw = -bankingSpeed;
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
        
        updateOrientation();
        updateModelMatrix();
    }

    void levelYaw() {
        if (yaw > 0.0f) {
            yaw -= bankingSpeed * deltaTime;
            if (yaw < 0.0f) yaw = 0.0f;
        }
        else if (yaw < 0.0f) {
            yaw += bankingSpeed * deltaTime;
            if (yaw > 0.0f) yaw = 0.0f;
        }

        updateOrientation();
        updateModelMatrix();
    }

    void updateModelMatrix() {
        modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(roll), glm::vec3(1.0f, 0.0f, 0.0f));

        boundingBox = originalBoundingBox;
        boundingBox = boundingBox.transform(modelMatrix);
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

    void setPosition(const glm::vec3& newPosition) {
        position = newPosition;
        float lowestPoint = boundingBox.min.y;
        std::cout << lowestPoint;
        if (lowestPoint < groundLevel) {
            float correction = groundLevel - lowestPoint;
            position.y += correction;
        }
        updateModelMatrix();
    }

    glm::vec3 getPosition() const {
        return position;
    }

    glm::vec3 getUpDirection() const {
        return upDirection;
    }

    glm::vec3 getForwardDirection() const {
        return forwardDirection;
    }

    float getSpeed() const {
        return speed;
    }

    BoundingBox getBoundingBox() const{
        return boundingBox;
    }
};
