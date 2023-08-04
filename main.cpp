#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"
#include <SDL.h>
#include "glm/gtc/matrix_transform.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
float rotationAngle = 0.0f;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Face {
    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> uvIndices;
    std::vector<unsigned int> normalIndices;
};

std::vector<Vertex> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;
std::vector<Face> faces;

bool loadOBJ(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error al abrir el archivo OBJ: " << filePath << std::endl;
        return false;
    }

    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;
    std::vector<Face> tempFaces;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        } else if (type == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            tempUvs.push_back(uv);
        } else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        } else if (type == "f") {
            Face face;
            char slash;
            unsigned int vertexIndex, uvIndex, normalIndex;
            for (int i = 0; i < 3; i++) {
                iss >> vertexIndex >> slash >> uvIndex >> slash >> normalIndex;
                face.vertexIndices.push_back(vertexIndex - 1);
                face.uvIndices.push_back(uvIndex - 1);
                face.normalIndices.push_back(normalIndex - 1);
            }
            tempFaces.push_back(face);
        }
    }

    file.close();

    // Copiar los datos a las variables globales
    vertices.resize(tempVertices.size());
    uvs.resize(tempUvs.size());
    normals.resize(tempNormals.size());

    for (size_t i = 0; i < tempVertices.size(); i++) {
        vertices[i].position = tempVertices[i];
    }

    for (size_t i = 0; i < tempUvs.size(); i++) {
        uvs[i] = tempUvs[i];
    }

    for (size_t i = 0; i < tempNormals.size(); i++) {
        normals[i] = tempNormals[i];
    }

    faces.assign(tempFaces.begin(), tempFaces.end());

    return true;
}


void init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void setColor(const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void point(int x, int y) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

void render() {
    // Calcular la matriz de transformación de escala
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f)); // Escala de 0.01
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelMatrix = rotationMatrix * scaleMatrix;

    // Dibujar los triángulos del modelo OBJ
    for (const Face& face : faces) {
        for (size_t i = 0; i < face.vertexIndices.size(); i += 3) {
            unsigned int vertexIndex1 = face.vertexIndices[i];
            unsigned int vertexIndex2 = face.vertexIndices[i + 1];
            unsigned int vertexIndex3 = face.vertexIndices[i + 2];

            glm::vec3 vertex1 = vertices[vertexIndex1].position;
            glm::vec3 vertex2 = vertices[vertexIndex2].position;
            glm::vec3 vertex3 = vertices[vertexIndex3].position;

            // Aplicar la transformación de escala
            glm::vec4 transformedVertex1 = modelMatrix * glm::vec4(vertex1, 1.0f);
            glm::vec4 transformedVertex2 = modelMatrix * glm::vec4(vertex2, 1.0f);
            glm::vec4 transformedVertex3 = modelMatrix * glm::vec4(vertex3, 1.0f);

            // Reflejar en el eje Y
            transformedVertex1.y *= -1.0f;
            transformedVertex2.y *= -1.0f;
            transformedVertex3.y *= -1.0f;

            // Convertir las coordenadas de los vértices a espacio de pantalla
            int x1 = static_cast<int>(transformedVertex1.x * SCREEN_WIDTH / 2 + SCREEN_WIDTH / 2);
            int y1 = static_cast<int>(transformedVertex1.y * SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 2);

            int x2 = static_cast<int>(transformedVertex2.x * SCREEN_WIDTH / 2 + SCREEN_WIDTH / 2);
            int y2 = static_cast<int>(transformedVertex2.y * SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 2);

            int x3 = static_cast<int>(transformedVertex3.x * SCREEN_WIDTH / 2 + SCREEN_WIDTH / 2);
            int y3 = static_cast<int>(transformedVertex3.y * SCREEN_HEIGHT / 2 + SCREEN_HEIGHT / 2);

            // Dibujar el triángulo
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
            SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
        }
    }
}




int main(int argc, char** argv) {
    init();
    bool running = true;

    if (!loadOBJ("rocket.obj")) {
        std::cerr << "Error al cargar el modelo OBJ." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        clear();
        const float rotationSpeed = 0.001f; // Puedes ajustar la velocidad de rotación aquí
        rotationAngle += rotationSpeed;
        render();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
