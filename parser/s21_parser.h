#ifndef INC_3DT_PARSE_OBJ_H
#define INC_3DT_PARSE_OBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vertex {
    float x;
    float y;
    float z;
    float w;
} Vertex;

typedef struct Vertices {
    Vertex *vertices;
    int count;
} Vertices;

typedef struct Face {
    int *vertex_indices;
    int *texture_indices;
    int *normal_indices;
    int vertex_count;
} Face;

typedef struct Faces {
    Face *faces;
    int count;
} Faces;


typedef struct Normal {
    float x;
    float y;
    float z;
} Normal;

typedef struct Normals {
    Normal *normals;
    int count;
} Normals;

typedef struct Texture {
    float u;
    float v;
    float w;
} Texture;

typedef struct Textures {
    Texture *textures;
    int count;
} Textures;

typedef struct Obj {
    Vertices *vertices;
    Faces *faces;
    Normals *normals;
    Textures *textures;
} Obj;

typedef struct Triangle {
    int vertex_indices[3];
    int texture_indices[3];
    int normal_indices[3];
} Triangle;

typedef struct Triangles {
    Triangle *triangles;
    int count;
} Triangles;

typedef struct VertexData {
    Vertex position;
    Texture texture;
    Normal normal;
} VertexData;

typedef struct VertexBuffer {
    VertexData *data;
    int count;
} VertexBuffer;


/// \brief Advance the pointer to the next non-whitespace character.
/// \param ptr The pointer to advance.
void advance(char **ptr);

/// \brief Advance the pointer to the next non-whitespace character.
/// \param ptr The pointer to advance.
void advance_whitespace(char **ptr);

/// \brief initialize an Obj struct.
/// \param error The error code.
/// \return The initialized Obj struct.
Obj *init_obj(int *error);

/// \brief Destroy an Obj struct.
/// \param obj The Obj struct to destroy.
void destroy_obj(Obj *obj);

/// \brief Parse an obj file.
/// \param filename The filename of the obj file.
/// \return The struct containing the parsed obj file.
Obj *parse_obj(const char *filename);

/// \brief Free a pointer if it is not NULL.
/// \param ptr The pointer to free.
void safe_free(void *ptr);

/// \brief Allocate memory for future parsing.
/// \param file The file to parse.
/// \param obj The obj struct to store the data in.
/// \param error The error code.
void prepare_obj(FILE *file, Obj *obj, int *error);

/// \brief Parse the obj file.
/// \param file The file to parse.
/// \param obj The obj struct to store the data in.
/// \param error The error code.
void parse_obj_file(FILE *file, Obj *obj, int *error);

/// \brief Parse a line of the obj file.
/// \param line The line to parse.
/// \param obj The obj struct to store the data in.
/// \param error The error code.
void parse_obj_line(char *line, Obj *obj, int *error);

/// \brief Parse a vertex.
/// \param line The line to parse.
/// \param error The error code.
/// \return The vertex.
Vertex parse_vertex(char *line, int *error);

/// \brief Parse a normal.
/// \param line The line to parse.
/// \param error The error code.
/// \return The normal.
Normal parse_normal(char *line, int *error);

/// \brief Parse a texture.
/// \param line The line to parse.
/// \param error The error code.
/// \return The texture.
Texture parse_texture(char *line, int *error);

/// \brief Parse a face.
/// \param line The line to parse.
/// \param error The error code.
/// \return The face.
Face parse_face(char *line, int *error);

/// \brief Convert all faces to triangles for rendering.
/// \param obj The obj struct to store the data in.
/// \param error The error code.
/// \return The triangles struct containing all the triangles.
Triangles triangulate(Obj *obj, int *error);

/// \brief Create a vertex buffer from the obj struct and the triangles.
/// \param obj The obj struct to store the data in.
/// \param triangles The triangles struct containing all the triangles.
/// \param error The error code.
/// \return The vertex buffer.
VertexBuffer create_vertex_buffer(Obj *obj, Triangles triangles, int *error);

#endif //INC_3DT_PARSE_OBJ_H
