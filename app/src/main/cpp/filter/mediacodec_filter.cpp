//
// Created by templechen on 2019-10-17.
//

#include "mediacodec_filter.h"
#include "../common/gl_utils.h"

#define GET_STR(x) #x

static const char *fragNV12 = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTextureCoord;     //顶点着色器传递的坐标
        uniform sampler2D uTextureY; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uTextureUV;
        void main(){
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(uTextureY,vTextureCoord).r;
            yuv.g = texture2D(uTextureUV,vTextureCoord).r - 0.5;
            yuv.b = texture2D(uTextureUV,vTextureCoord).a - 0.5;
            rgb = mat3(1.0,     1.0,    1.0,
                       0.0,-0.39465,2.03211,
                       1.13983,-0.58060,0.0)*yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb,1.0);
        }
);

static const char *fragNV21 = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTextureCoord;     //顶点着色器传递的坐标
        uniform sampler2D uTextureY; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uTextureUV;
        void main(){
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(uTextureY,vTextureCoord).r;
            yuv.g = texture2D(uTextureUV,vTextureCoord).a - 0.5;
            yuv.b = texture2D(uTextureUV,vTextureCoord).r - 0.5;
            rgb = mat3(1.0,     1.0,    1.0,
                       0.0,-0.39465,2.03211,
                       1.13983,-0.58060,0.0)*yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb,1.0);
        }
);

void mediacodec_filter::initFragmentShader() {
    fragment_shader_string = fragNV12;
}

void mediacodec_filter::createTextures() {
    yTexture = createTexture(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, yTexture);
    uvTexture = createTexture(GL_TEXTURE_2D);

    ALOGD("mediacodec, %d %d %d", program,yTexture, uvTexture);
}

void mediacodec_filter::drawTextures(AVFrame *avFrame) {
    uTextureYLocation = glGetUniformLocation(program, uTextureY);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, avFrame->width, avFrame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 avFrame->data[0]);
    glUniform1i(uTextureYLocation, 0);

    uTextureUVLocation = glGetUniformLocation(program, uTextureUV);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uvTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, avFrame->width / 2, avFrame->height / 2, 0, GL_LUMINANCE_ALPHA,
                 GL_UNSIGNED_BYTE,
                 avFrame->data[1]);
    glUniform1i(uTextureUVLocation, 1);
}

void mediacodec_filter::releaseTextures() {
    auto textures = new GLuint[2];
    textures[0] = yTexture;
    textures[1] = uvTexture;
    glDeleteTextures(2, textures);
}
