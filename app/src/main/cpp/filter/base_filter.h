//
// Created by templechen on 2019-04-28.
//

#ifndef FFMPEG_PLAYER_BASE_FILTER_H
#define FFMPEG_PLAYER_BASE_FILTER_H

#include <GLES3/gl3.h>
#include "../common/matrix_util.h"
extern "C" {
#include <libavutil/frame.h>
};

static GLfloat vertex[] = {
        1.0f, 1.0f,
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,
};

static GLfloat texture[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
};

class base_filter {

public:
    base_filter();

    virtual ~base_filter();

    virtual void init_program();

    void drawFrame(AVFrame *avFrame);

private:

    int width;
    int height;

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    const GLchar *aPosition = "aPosition";
    const GLchar *aTextureCoordinate = "aTextureCoordinate";
    const GLchar *uTextureMatrix = "uTextureMatrix";
    const GLchar *uTextureY = "uTextureY";
    const GLchar *uTextureU = "uTextureU";
    const GLchar *uTextureV = "uTextureV";
    GLint aPositionLocation = -1.0;
    GLint aTextureCoordinateLocation = -1.0;
    GLint uTextureMatrixLocation = -1.0;
    GLint uTextureYLocation = -1.0;
    GLint uTextureULocation = -1.0;
    GLint uTextureVLocation = -1.0;

    const char *vertex_shader_string = {
            "uniform mat4 uTextureMatrix;\n"
            "attribute vec4 aPosition;\n"
            "attribute vec4 aTextureCoordinate;\n"
            "varying vec2 vTextureCoord;\n"
            "varying vec2 vPosition;\n"
            "void main()\n"
            "{\n"
            "    vTextureCoord = (uTextureMatrix * aTextureCoordinate).xy;\n"
            "    gl_Position = aPosition;\n"
            "    vPosition = aPosition.xy;\n"
            "}\n"
    };
    const char *fragment_shader_string = {
            "precision mediump float;\n"
            "uniform sampler2D uTextureY;\n"
            "uniform sampler2D uTextureU;\n"
            "uniform sampler2D uTextureV;\n"
            "varying vec2 vTextureCoord;\n"
            "varying vec2 vPosition;\n"
            "void main()\n"
            "{\n"
            "    vec3 YUV;\n"
            "    vec3 RGB;\n"
            "    YUV.x = texture2D(uTextureY, vTextureCoord).r;\n"
            "    YUV.y = texture2D(uTextureU, vTextureCoord).r - 0.5;\n"
            "    YUV.z = texture2D(uTextureV, vTextureCoord).r - 0.5;\n"
            "    RGB = mat3(1.0, 1.0, 1.0, 0.0, -0.39465, 2.03211, 1.13983, -0.58060, 0.0) * YUV;\n"
            "    if(vPosition.x > 0.0 && vPosition.y> 0.0) {\n"
            "       gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
            "    } else {\n"
            "       gl_FragColor = vec4(RGB, 1.0);\n"
            "    }\n"
            "}\n"
    };

    ESMatrix *textureMatrix;

    void initMatrix();

    GLuint yTexture;
    GLuint uTexture;
    GLuint vTexture;
};

#endif //FFMPEG_PLAYER_BASE_FILTER_H
