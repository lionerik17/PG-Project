#include "BoundingBox.h"

BoundingBox::BoundingBox()
    : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

BoundingBox::BoundingBox(const glm::vec3& minCorner, const glm::vec3& maxCorner)
    : min(minCorner), max(maxCorner) {}

bool BoundingBox::intersects(const BoundingBox& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

std::vector<glm::vec3> BoundingBox::getTransformedCorners(const glm::mat4& transform) const {
    std::vector<glm::vec3> corners = {
        min,
        {min.x, min.y, max.z},
        {min.x, max.y, min.z},
        {min.x, max.y, max.z},
        {max.x, min.y, min.z},
        {max.x, min.y, max.z},
        {max.x, max.y, min.z},
        max
    };

    for (glm::vec3& corner : corners) {
        corner = glm::vec3(transform * glm::vec4(corner, 1.0f));
    }

    return corners;
}

std::vector<glm::vec3> BoundingBox::getSeparatingAxes(const std::vector<glm::vec3>& thisCorners, const std::vector<glm::vec3>& otherCorners) const {
    // Axes to test: face normals
    std::vector<glm::vec3> axes = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1)
    };

    // Add cross products of edge vectors to axes
    for (const glm::vec3& thisEdge : thisCorners) {
        for (const glm::vec3& otherEdge : otherCorners) {
            axes.push_back(glm::normalize(glm::cross(thisEdge, otherEdge)));
        }
    }

    return axes;
}

bool BoundingBox::overlapOnAxis(const std::vector<glm::vec3>& thisCorners, const std::vector<glm::vec3>& otherCorners, const glm::vec3& axis) const {
    if (glm::length(axis) < 1e-6f) {
        return true; // Skip near-zero axis
    }

    // Project corners onto the axis
    auto project = [&](const std::vector<glm::vec3>& corners, float& minProj, float& maxProj) {
        minProj = glm::dot(corners[0], axis);
        maxProj = minProj;

        for (const glm::vec3& corner : corners) {
            float proj = glm::dot(corner, axis);
            if (proj < minProj) minProj = proj;
            if (proj > maxProj) maxProj = proj;
        }
        };

    // Project this bounding box and the other bounding box
    float thisMin, thisMax;
    project(thisCorners, thisMin, thisMax);

    float otherMin, otherMax;
    project(otherCorners, otherMin, otherMax);

    // Check for overlap
    return thisMax >= otherMin && otherMax >= thisMin;
}

// Translate the bounding box
void BoundingBox::translate(const glm::vec3& translation) {
    min += translation;
    max += translation;
}

// Scale the bounding box
void BoundingBox::scale(const glm::vec3& scale) {
    glm::vec3 center = (min + max) * 0.5f;
    glm::vec3 halfSize = (max - min) * 0.5f;
    halfSize *= scale;

    min = center - halfSize;
    max = center + halfSize;
}

// Transform the bounding box by a model matrix
BoundingBox BoundingBox::transform(const glm::mat4& modelMatrix) const {
    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, max.y, max.z)
    };

    glm::vec3 transformedMin(std::numeric_limits<float>::max());
    glm::vec3 transformedMax(std::numeric_limits<float>::lowest());

    for (int i = 0; i < 8; ++i) {
        glm::vec4 transformedCorner = modelMatrix * glm::vec4(corners[i], 1.0f);
        glm::vec3 transformedPosition = glm::vec3(transformedCorner);

        transformedMin.x = std::min(transformedMin.x, transformedPosition.x);
        transformedMin.y = std::min(transformedMin.y, transformedPosition.y);
        transformedMin.z = std::min(transformedMin.z, transformedPosition.z);

        transformedMax.x = std::max(transformedMax.x, transformedPosition.x);
        transformedMax.y = std::max(transformedMax.y, transformedPosition.y);
        transformedMax.z = std::max(transformedMax.z, transformedPosition.z);
    }

    return BoundingBox(transformedMin, transformedMax);
}

void BoundingBox::print() const {
    std::cout << "BoundingBox Min: (" << min.x << ", " << min.y << ", " << min.z << ")\n";
    std::cout << "BoundingBox Max: (" << max.x << ", " << max.y << ", " << max.z << ")\n";
}
