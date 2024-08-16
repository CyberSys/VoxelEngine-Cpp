#include "Mesh.hpp"

#include <cstring>
#include <GL/glew.h>

int Mesh::meshesCount = 0;
int Mesh::drawCalls = 0;

RawMesh::RawMesh(
    const float* vertexBuffer, size_t vertices, 
    const int* indexBuffer, size_t indices, const vattr* attrs
) {
    vertexSize = 0;
    int vattrsCount = 0;
    for (; attrs[vattrsCount].size; vattrsCount++) {
        vertexSize += attrs[vattrsCount].size;
    }
    this->vertexBuffer.resize(vertices*vertexSize);
    std::memcpy(this->vertexBuffer.data(), vertexBuffer, sizeof(float)*vertexSize*vertices);
    this->indexBuffer.resize(indices);
    std::memcpy(this->indexBuffer.data(), indexBuffer, sizeof(int)*indices);
    this->attrs.resize(vattrsCount+1, {0});
    std::memcpy(this->attrs.data(), attrs, sizeof(vattr)*vattrsCount);
}

std::unique_ptr<Mesh> RawMesh::createMesh() {
    return std::make_unique<Mesh>(
        vertexBuffer.data(), vertexBuffer.size()/vertexSize, 
        indexBuffer.data(), indexBuffer.size(), attrs.data()
    );
}

Mesh::Mesh(const float* vertexBuffer, size_t vertices, const int* indexBuffer, size_t indices, const vattr* attrs) : 
    ibo(0),
    vertices(vertices),
    indices(indices)
{
    meshesCount++;
    vertexSize = 0;
    for (int i = 0; attrs[i].size; i++) {
        vertexSize += attrs[i].size;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    reload(vertexBuffer, vertices, indexBuffer, indices);

    // attributes
    int offset = 0;
    for (int i = 0; attrs[i].size; i++) {
        int size = attrs[i].size;
        glVertexAttribPointer(i, size, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (GLvoid*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        offset += size;
    }

    glBindVertexArray(0);
}

Mesh::~Mesh(){
    meshesCount--;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    if (ibo != 0) glDeleteBuffers(1, &ibo);
}

void Mesh::reload(const float* vertexBuffer, size_t vertices, const int* indexBuffer, size_t indices){
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (vertexBuffer != nullptr && vertices != 0) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexSize * vertices, vertexBuffer, GL_STATIC_DRAW);
    }
    else {
        glBufferData(GL_ARRAY_BUFFER, 0, {}, GL_STATIC_DRAW);
    }
    if (indexBuffer != nullptr && indices != 0) {
        if (ibo == 0) glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices, indexBuffer, GL_STATIC_DRAW);
    }
    else if (ibo != 0) {
        glDeleteBuffers(1, &ibo);
    }
    this->vertices = vertices;
    this->indices = indices;
}

void Mesh::draw(unsigned int primitive){
    drawCalls++;
    glBindVertexArray(vao);
    if (ibo != 0) {
        glDrawElements(primitive, indices, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(primitive, 0, vertices);
    }
    glBindVertexArray(0);
}

void Mesh::draw() {
    draw(GL_TRIANGLES);
}
