#include <iostream>
#include "glm/glm.hpp"
#include <SDL.h>

int main() {
    glm::vec3 v(1.0f, 0.0f, 0.0f);
    //print the vector
    std::cout << "Hello, World!" << std::endl;
    std::cout << "v = (" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    return 0;
}
