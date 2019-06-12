//
// Created by templechen on 2019-04-28.
//

#include "base_filter.h"
#include "../common/gl_utils.h"

extern "C" {
#include <libavutil/frame.h>
}

base_filter::base_filter() {

}

base_filter::~base_filter() {
    auto textures = new GLuint[3];
    textures[0] = yTexture;
    textures[1] = uTexture;
    textures[2] = vTexture;
    glDeleteTextures(3, textures);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(program);
}

void base_filter::init_program() {
    vertexShader = loadShader(GL_VERTEX_SHADER, vertex_shader_string);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragment_shader_string);
    program = createShaderProgram(vertexShader, fragmentShader);
    initMatrix();

    yTexture = createTexture(GL_TEXTURE_2D);
    uTexture = createTexture(GL_TEXTURE_2D);
    vTexture = createTexture(GL_TEXTURE_2D);
}

void base_filter::drawFrame(AVFrame *avFrame) {

    checkVideoSize(avFrame);

    glUseProgram(program);

    GLint vertexCount = sizeof(vertex) / (sizeof(vertex[0]) * 2);
    aPositionLocation = glGetAttribLocation(program, aPosition);
    glEnableVertexAttribArray(aPositionLocation);
    glVertexAttribPointer(aPositionLocation, 2, GL_FLOAT, GL_FALSE, 8, vertex);

    aTextureCoordinateLocation = glGetAttribLocation(program, aTextureCoordinate);
    glEnableVertexAttribArray(aTextureCoordinateLocation);
    glVertexAttribPointer(aTextureCoordinateLocation, 2, GL_FLOAT, GL_FALSE, 8, texture);

    uTextureMatrixLocation = glGetUniformLocation(program, uTextureMatrix);
    glUniformMatrix4fv(uTextureMatrixLocation, 1, GL_FALSE, this->textureMatrix->m);

    uCoordMatrixLocation = glGetUniformLocation(program, uCoordinateMatrix);
    glUniformMatrix4fv(uCoordMatrixLocation, 1, GL_FALSE, this->uCoordMatrix->m);

    uTextureYLocation = glGetUniformLocation(program, uTextureY);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width, avFrame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 avFrame->data[0]);
    glUniform1i(uTextureYLocation, 0);

    uTextureULocation = glGetUniformLocation(program, uTextureU);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width / 2, avFrame->height / 2, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 avFrame->data[1]);
    glUniform1i(uTextureULocation, 1);

    uTextureVLocation = glGetUniformLocation(program, uTextureV);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width / 2, avFrame->height / 2, 0, GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 avFrame->data[2]);
    glUniform1i(uTextureVLocation, 2);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDisableVertexAttribArray(aPositionLocation);
    glDisableVertexAttribArray(aTextureCoordinateLocation);
    glBindTexture(GL_TEXTURE_2D, 0);

    av_frame_free(&avFrame);
}

void base_filter::initMatrix() {
    textureMatrix = new ESMatrix();
    textureMatrix->m[0] = 1.0f;
    textureMatrix->m[1] = 0.0f;
    textureMatrix->m[2] = 0.0f;
    textureMatrix->m[3] = 0.0f;
    textureMatrix->m[4] = 0.0f;
    textureMatrix->m[5] = -1.0f;
    textureMatrix->m[6] = 0.0f;
    textureMatrix->m[7] = 0.0f;
    textureMatrix->m[8] = 0.0f;
    textureMatrix->m[9] = 0.0f;
    textureMatrix->m[10] = 1.0f;
    textureMatrix->m[11] = 0.0f;
    textureMatrix->m[12] = 0.0f;
    textureMatrix->m[13] = 1.0f;
    textureMatrix->m[14] = 0.0f;
    textureMatrix->m[15] = 1.0f;
}

void base_filter::checkVideoSize(AVFrame *frame) {
    int width = frame->width;
    int height = frame->height;
    if (this->width != width || this->height != height) {
        this->width = width;
        this->height = height;
        initCoordMatrix();
        if (screen_width > 0 && screen_height > 0) {
            float screen_ratio = screen_width * 1.0f / screen_height;
            float video_ratio = width * 1.0f / height;
            if (video_ratio > screen_ratio) {
                scaleM(uCoordMatrix, 0, 1.0f, screen_ratio / video_ratio, 1.0f);
            } else {
                scaleM(uCoordMatrix, 0, 1.0f, video_ratio / screen_ratio, 1.0f);
            }
        }
    }
}

void base_filter::initCoordMatrix() {
    uCoordMatrix = new ESMatrix();
    setIdentityM(uCoordMatrix);
}
