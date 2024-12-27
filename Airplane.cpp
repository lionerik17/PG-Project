#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Airplane {
private:
    glm::vec3 position;       // Position of the airplane
    glm::vec3 velocity;       // Velocity of the airplane
    float gravity;            // Gravitational acceleration
    glm::mat4 modelMatrix;    // Model transformation matrix
    GLuint modelMatrixLoc;    // Shader location for model matrix
    float groundLevel;        // Ground level to stop the airplane
    float deltaTime = 0.016f;

public:
    Airplane(glm::vec3 startPosition, glm::mat4 initialModelMatrix, GLuint shaderModelLoc, float groundY = 3.0f, float gravityAccel = -9.8f)
        : position(startPosition), velocity(0.0f), gravity(gravityAccel), modelMatrix(initialModelMatrix),
        modelMatrixLoc(shaderModelLoc), groundLevel(groundY) {}

    void applyGravity() {
        velocity.y += gravity * deltaTime;

        position.y += velocity.y * deltaTime;

        if (position.y <= groundLevel) {
            position.y = groundLevel;
            velocity.y = 0.0f;
        }

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

    void applyForce(glm::vec3 force) {
        velocity += force;
    }

    void setPosition(glm::vec3 newPosition) {
        position = newPosition;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    void reset() {
        position = glm::vec3(0.0f, 6.0f, -10.0f);
        velocity = glm::vec3(0.0f);
    }
};
