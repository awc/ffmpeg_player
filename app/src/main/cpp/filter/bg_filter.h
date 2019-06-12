//
// Created by templechen on 2019-06-12.
//

#ifndef FFMPEG_PLAYER_BG_FILTER_H
#define FFMPEG_PLAYER_BG_FILTER_H

#include <GLES3/gl3.h>
#include "../common/matrix_util.h"

extern "C" {
#include <libavutil/frame.h>
};

class bg_filter {

public:
    bg_filter();

    virtual ~bg_filter();

    virtual void init_program();

    void drawFrame(AVFrame *avFrame);

    int screen_width = 0;
    int screen_height = 0;

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
    const GLchar *uCoordinateMatrix = "uCoordMatrix";
    GLint aPositionLocation = -1.0;
    GLint aTextureCoordinateLocation = -1.0;
    GLint uTextureMatrixLocation = -1.0;
    GLint uTextureYLocation = -1.0;
    GLint uTextureULocation = -1.0;
    GLint uTextureVLocation = -1.0;
    GLint uCoordMatrixLocation = -1.0;

    const char *vertex_shader_string = {
            "uniform mat4 uTextureMatrix;\n"
            "attribute vec4 aPosition;\n"
            "attribute vec4 aTextureCoordinate;\n"
            "varying vec2 vTextureCoord;\n"
            "uniform mat4 uCoordMatrix;\n"
            "void main()\n"
            "{\n"
            "    vTextureCoord = (uTextureMatrix * aTextureCoordinate).xy;\n"
            "    gl_Position = uCoordMatrix * aPosition;\n"
            "}\n"
    };
//    const char *fragment_shader_string = {
//            "precision mediump float;\n"
//            "uniform sampler2D uTextureY;\n"
//            "uniform sampler2D uTextureU;\n"
//            "uniform sampler2D uTextureV;\n"
//            "varying vec2 vTextureCoord;\n"
//            "void main()\n"
//            "{\n"
//            "    vec3 YUV;\n"
//            "    vec3 RGB;\n"
//            "    YUV.x = texture2D(uTextureY, vTextureCoord).r;\n"
//            "    YUV.y = texture2D(uTextureU, vTextureCoord).r - 0.5;\n"
//            "    YUV.z = texture2D(uTextureV, vTextureCoord).r - 0.5;\n"
//            "    RGB = mat3(1.0, 1.0, 1.0, 0.0, -0.39425, 2.03211, 1.13983, -0.5806, 0.0) * YUV;\n"
//            "    gl_FragColor = vec4(RGB, 1.0);\n"
//            "}\n"
//    };

    const char *fragment_shader_string = {
            "precision highp float;\n"
            "uniform sampler2D uTextureY;\n"
            "uniform sampler2D uTextureU;\n"
            "uniform sampler2D uTextureV;\n"
            "varying vec2 vTextureCoord;\n"
            "void main()\n"
            "{\n"
            "float y = texture2D(uTextureY, vTextureCoord).r;\n"
            "float u = texture2D(uTextureU, vTextureCoord).r - 0.5;\n"
            "float v = texture2D(uTextureV, vTextureCoord).r - 0.5;\n"
            "float r = y + 1.402 * v;\n"
            "float g = y - 0.344 * u - 0.714 * v;\n"
            "float b = y + 1.772 * u;\n"
            "gl_FragColor = vec4(r,g,b,1.0);\n"
            "}\n"
    };

    ESMatrix *textureMatrix;

    void initMatrix();

    GLuint yTexture;
    GLuint uTexture;
    GLuint vTexture;

    void checkVideoSize(AVFrame *frame);

    ESMatrix *uCoordMatrix;

    void initCoordMatrix();
};


#endif //FFMPEG_PLAYER_BG_FILTER_H
