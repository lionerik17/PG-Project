#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace gps {

    class BoundingBox {
    public:
        BoundingBox();
        BoundingBox(const glm::vec3& min, const glm::vec3& max);

        void calculate(const std::vector<glm::vec3>& vertices);
        void transform(const glm::mat4& modelMatrix);
        bool isColliding(const BoundingBox& other) const;

        glm::vec3 getMin() const;
        glm::vec3 getMax() const;
        std::vector<glm::vec3> getCorners() const;
        void setMin(const glm::vec3& newMin);
        void setMax(const glm::vec3& newMax);
    private:
        glm::vec3 min;
        glm::vec3 max;
    };

} // namespace gps

#endif // BOUNDINGBOX_H
