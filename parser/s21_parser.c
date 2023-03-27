#include "s21_parser.h"

void advance(char **ptr) { (*ptr)++; }

void advance_n(char **ptr, int n) { (*ptr) += n; }

void advance_whitespace(char **ptr) {
  while ((**ptr == ' ' || **ptr == '\t') && **ptr != '\0') advance(ptr);
}

void advance_to_whitespace(char **ptr) {
  while (**ptr != ' ' && **ptr != '\t' && **ptr != '\0') advance(ptr);
}

Obj *init_obj(int *error) {
  Obj *obj = calloc(sizeof(Obj), 1);
  if (obj == NULL) {
    printf("Error: Could not allocate memory for obj");
    *error = 1;
  }
  if (!*error) {
    obj->vertices = calloc(sizeof(Vertices), 1);
    obj->faces = calloc(sizeof(Faces), 1);
    obj->normals = calloc(sizeof(Normals), 1);
    obj->textures = calloc(sizeof(Textures), 1);
    if (obj->vertices == NULL || obj->faces == NULL || obj->normals == NULL ||
        obj->textures == NULL) {
      printf("Error: Could not allocate memory for obj");
      *error = 1;
    }
  }
  return obj;
}

void safe_free(void *ptr) {
  if (ptr != NULL) free(ptr);
}

void destroy_obj(Obj *obj) {
  if (obj == NULL) return;
  safe_free(obj->vertices->vertices);
  safe_free(obj->vertices);
  for (int i = 0; i < obj->faces->count; i++) {
    safe_free(obj->faces->faces[i].vertex_indices);
    safe_free(obj->faces->faces[i].texture_indices);
    safe_free(obj->faces->faces[i].normal_indices);
  }
  safe_free(obj->faces->faces);
  safe_free(obj->faces);
  safe_free(obj->normals->normals);
  safe_free(obj->normals);
  safe_free(obj->textures->textures);
  safe_free(obj->textures);
  safe_free(obj);
}

Obj *parse_obj(const char *filename) {
  FILE *file = fopen(filename, "r");
  int error = 0;
  if (file == NULL) {
    printf("Error: Could not open file %s\n", filename);
    error = 1;
  }
  Obj *obj = NULL;
  if (!error) obj = init_obj(&error);
  if (!error) prepare_obj(file, obj, &error);
  if (!error) parse_obj_file(file, obj, &error);
  if (file != NULL) fclose(file);
  if (error && obj != NULL) {
    destroy_obj(obj);
    obj = NULL;
  }
  return obj;
}

void prepare_obj(FILE *file, Obj *obj, int *error) {
  if (file == NULL || obj == NULL) {
    *error = 1;
    return;
  }

  char line[256] = {0};
  while (fgets(line, 256, file)) {
    if (line[0] == 'v' && line[1] == ' ') {
      obj->vertices->count++;
    } else if (line[0] == 'v' && line[1] == 't') {
      obj->textures->count++;
    } else if (line[0] == 'v' && line[1] == 'n') {
      obj->normals->count++;
    } else if (line[0] == 'f' && line[1] == ' ') {
      obj->faces->count++;
    }
  }
  obj->vertices->vertices = calloc(sizeof(Vertex), obj->vertices->count);
  obj->textures->textures = calloc(sizeof(Texture), obj->textures->count);
  obj->normals->normals = calloc(sizeof(Normal), obj->normals->count);
  obj->faces->faces = calloc(sizeof(Face), obj->faces->count);
  if (obj->vertices->vertices == NULL || obj->textures->textures == NULL ||
      obj->normals->normals == NULL || obj->faces->faces == NULL) {
    printf("Error: Could not allocate memory for obj\n");
    *error = 1;
  }
  rewind(file);
}

void parse_obj_file(FILE *file, Obj *obj, int *error) {
  if (file == NULL || obj == NULL) {
    *error = 1;
    return;
  }
  int vertex_index = 0;
  int texture_index = 0;
  int normal_index = 0;
  int face_index = 0;
  char line[256] = {0};
  while (fgets(line, 256, file)) {
    if (line[0] == 'v' && line[1] == ' ') {
      obj->vertices->vertices[vertex_index] = parse_vertex(line, error);
      vertex_index++;
    } else if (line[0] == 'v' && line[1] == 't') {
      obj->textures->textures[texture_index] = parse_texture(line, error);
      texture_index++;
    } else if (line[0] == 'v' && line[1] == 'n') {
      obj->normals->normals[normal_index] = parse_normal(line, error);
      normal_index++;
    } else if (line[0] == 'f' && line[1] == ' ') {
      obj->faces->faces[face_index] = parse_face(line, error);
      face_index++;
    }
  }
}

Vertex parse_vertex(char *line, int *error) {
  char *ptr = line;
  Vertex vertex;
  advance(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &vertex.x) != 1;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &vertex.y) != 1;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &vertex.z) != 1;
  ;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  if (*ptr != '\0' && *ptr != '\n') {
    *error = sscanf(ptr, "%f", &vertex.w) != 1;
  }
  if (*error) {
    printf("Error: Could not parse vertex\n");
  }
  return vertex;
}

Texture parse_texture(char *line, int *error) {
  char *ptr = line;
  Texture texture;
  advance(&ptr);
  advance(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &texture.u) != 1;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &texture.v) != 1;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  if (*ptr != '\0' && *ptr != '\n') {
    *error = sscanf(ptr, "%f", &texture.w) != 1;
  }
  if (*error) {
    printf("Error: Could not parse texture\n");
  }
  return texture;
}

Normal parse_normal(char *line, int *error) {
  char *ptr = line;
  Normal normal;
  advance(&ptr);
  advance(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &normal.x) != 1;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &normal.y) != 1;
  advance_to_whitespace(&ptr);
  advance_whitespace(&ptr);
  *error = sscanf(ptr, "%f", &normal.z) != 1;
  if (*error) {
    printf("Error: Could not parse normal\n");
  }
  return normal;
}

Face parse_face(char *line, int *error) {
  char *ptr = line;
  Face face = {0};
  advance(&ptr);
  advance_whitespace(&ptr);
  int count = 0;
  while (*ptr != '\0') {
    count++;
    advance_to_whitespace(&ptr);
    advance_whitespace(&ptr);
  }
  face.vertex_indices = calloc(sizeof(int), count);
  face.texture_indices = calloc(sizeof(int), count);
  face.normal_indices = calloc(sizeof(int), count);
  face.vertex_count = count;
  if (face.vertex_indices == NULL || face.texture_indices == NULL ||
      face.normal_indices == NULL) {
    printf("Error: Could not allocate memory for face\n");
    *error = 1;
  }
  ptr = line;

  //    regex: f(\s(v?)\d+(\/((vt?)\d+)?(\/((vn?)\d+)?)?)?){3,}

  advance(&ptr);
  for (int i = 0; i < count; i++) {
    advance(&ptr);
    if (*ptr == 'v') advance(&ptr);
    int len = 0;
    *error = sscanf(ptr, "%d%n", &face.vertex_indices[i], &len) != 1;
    advance_n(&ptr, len);
    if (*ptr == '/' && !*error) {
      advance(&ptr);
      if (*ptr == 'v' && *(ptr + 1) == 't') {
        advance_n(&ptr, 2);
        *error = sscanf(ptr, "%d%n", &face.texture_indices[i], &len) != 1;
      } else {
        len = 0;
        int scan = sscanf(ptr, "%d%n", &face.texture_indices[i], &len);
        if (scan != 1) face.texture_indices[i] = 0;
      }
      ptr += len;
      if (*ptr == '/' && !*error) {
        advance(&ptr);
        // ((vn?)\d+)?
        if (*ptr == 'v' && *(ptr + 1)) {
          advance_n(&ptr, 2);
          *error = sscanf(ptr, "%d%n", &face.normal_indices[i], &len) != 1;
        } else {
          len = 0;
          int scan = sscanf(ptr, "%d%n", &face.normal_indices[i], &len);
          if (scan != 1) face.normal_indices[i] = 0;
        }
        ptr += len;
      }
    }
  }

  if (*error) {
    printf("Error: Could not parse face\n");
  }
  return face;
}

Triangles triangulate(Obj *obj, int *error) {
  // if vertex count is greater than 3, triangulate count - 2 triangles
  //    print_faces(obj->faces);
  int triangles_count = 0;
  for (int i = 0; i < obj->faces->count; i++)
    triangles_count += obj->faces->faces[i].vertex_count - 2;
  Triangles triangles = {0};
  triangles.count = triangles_count;
  triangles.triangles = calloc(sizeof(Triangle), triangles_count);
  if (triangles.triangles == NULL) {
    printf("Error: Could not allocate memory for triangles\n");
    *error = 1;
  }
  if (!*error) {
    int index = 0;
    for (int i = 0; i < obj->faces->count; i++) {
      for (int j = 0; j < obj->faces->faces[i].vertex_count - 2; j++) {
        triangles.triangles[index].vertex_indices[0] =
            obj->faces->faces[i].vertex_indices[0];
        triangles.triangles[index].vertex_indices[1] =
            obj->faces->faces[i].vertex_indices[j + 1];
        triangles.triangles[index].vertex_indices[2] =
            obj->faces->faces[i].vertex_indices[j + 2];
        triangles.triangles[index].texture_indices[0] =
            obj->faces->faces[i].texture_indices[0];
        triangles.triangles[index].texture_indices[1] =
            obj->faces->faces[i].texture_indices[j + 1];
        triangles.triangles[index].texture_indices[2] =
            obj->faces->faces[i].texture_indices[j + 2];
        triangles.triangles[index].normal_indices[0] =
            obj->faces->faces[i].normal_indices[0];
        triangles.triangles[index].normal_indices[1] =
            obj->faces->faces[i].normal_indices[j + 1];
        triangles.triangles[index].normal_indices[2] =
            obj->faces->faces[i].normal_indices[j + 2];
        index++;
      }
    }
  }
  return triangles;
}

VertexBuffer create_vertex_buffer(Obj *obj, Triangles triangles, int *error) {
  VertexData *vertex_data = calloc(sizeof(VertexData), triangles.count * 3);
  if (vertex_data == NULL) {
    printf("Error: Could not allocate memory for vertex data");
    *error = 1;
  }

  if (!*error) {
    for (int i = 0; i < triangles.count; i++) {
      for (int j = 0; j < 3; j++) {
        vertex_data[i * 3 + j].position =
            obj->vertices
                ->vertices[triangles.triangles[i].vertex_indices[j] - 1];
        vertex_data[i * 3 + j].texture =
            obj->textures
                ->textures[triangles.triangles[i].texture_indices[j] - 1];
        vertex_data[i * 3 + j].normal =
            obj->normals->normals[triangles.triangles[i].normal_indices[j] - 1];
      }
    }
  }

  VertexBuffer vb = {0};
  vb.count = triangles.count * 3;
  vb.data = vertex_data;

  // display vertex data
  //    printf("Vertex count: %d\n", vb.count);
  //    printf("Vertex Data:\n");
  //    for (int i = 0; i < vb.count; i++) {
  //        printf("\tVertex %d:\n", i);
  //        printf("\t\tPosition: (%f, %f, %f)\n", vb.data[i].position.x,
  //        vb.data[i].position.y, vb.data[i].position.z); printf("\t\tTexture:
  //        (%f, %f)\n", vb.data[i].texture.u, vb.data[i].texture.v);
  //        printf("\t\tNormal: (%f, %f, %f)\n", vb.data[i].normal.x,
  //        vb.data[i].normal.y, vb.data[i].normal.z);
  //    }
  return vb;
}
