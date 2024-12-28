#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <glm/glm.hpp>
#include <iostream>

class BoundingBox {
public:
    glm::vec3 min;
    glm::vec3 max;

    // Default constructor
    BoundingBox();

    // Constructor with specified corners
    BoundingBox(const glm::vec3& minCorner, const glm::vec3& maxCorner);

    // Check if this bounding box intersects with another
    bool intersects(const BoundingBox& otherBox) const;

    // Translate the bounding box by a given vector
    void translate(const glm::vec3& translation);

    BoundingBox transform(const glm::mat4& modelMatrix) const;

    // Expand the bounding box by a given scale
    void scale(const glm::vec3& scale);

    // Debugging function to print the bounding box values
    void print() const;
private:
    std::vector<glm::vec3> getTransformedCorners(const glm::mat4& transform) const;


    std::vector<glm::vec3> getSeparatingAxes(const std::vector<glm::vec3>& thisCorners, const std::vector<glm::vec3>& otherCorners) const;

    bool overlapOnAxis(const std::vector<glm::vec3>& thisCorners, const std::vector<glm::vec3>& otherCorners, const glm::vec3& axis) const;
};

#endif // BOUNDINGBOX_H
