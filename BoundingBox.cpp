#include "BoundingBox.h"
#include <algorithm>
#include <limits>

namespace gps {

    BoundingBox::BoundingBox()
        : min(glm::vec3(std::numeric_limits<float>::max())),
        max(glm::vec3(std::numeric_limits<float>::lowest())) {}

    BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max)
        : min(min), max(max) {}

    void BoundingBox::calculate(const std::vector<glm::vec3>& vertices) {
        min = glm::vec3(std::numeric_limits<float>::max());
        max = glm::vec3(std::numeric_limits<float>::lowest());

        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
            max = glm::max(max, vertex);
        }
    }

    void BoundingBox::transform(const glm::mat4& modelMatrix) {
        glm::vec3 corners[8] = {
            min,
            glm::vec3(min.x, min.y, max.z),
            glm::vec3(min.x, max.y, min.z),
            glm::vec3(min.x, max.y, max.z),
            glm::vec3(max.x, min.y, min.z),
            glm::vec3(max.x, min.y, max.z),
            glm::vec3(max.x, max.y, min.z),
            max
        };

        glm::vec3 newMin(std::numeric_limits<float>::max());
        glm::vec3 newMax(std::numeric_limits<float>::lowest());

        for (const auto& corner : corners) {
            glm::vec4 transformedCorner = modelMatrix * glm::vec4(corner, 1.0f);
            newMin = glm::min(newMin, glm::vec3(transformedCorner));
            newMax = glm::max(newMax, glm::vec3(transformedCorner));
        }

        min = newMin;
        max = newMax;
    }

    bool BoundingBox::isColliding(const glm::vec3& point) const {
        return (point.x >= min.x && point.x <= max.x &&
            point.y >= min.y && point.y <= max.y &&
            point.z >= min.z && point.z <= max.z);
    }

    glm::vec3 BoundingBox::getMin() const {
        return min;
    }

    glm::vec3 BoundingBox::getMax() const {
        return max;
    }

    void gps::BoundingBox::setMin(const glm::vec3& newMin) {
        min = newMin;
    }

    void gps::BoundingBox::setMax(const glm::vec3& newMax) {
        max = newMax;
    }

} // namespace gps
