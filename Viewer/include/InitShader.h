#pragma once

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

GLuint Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
