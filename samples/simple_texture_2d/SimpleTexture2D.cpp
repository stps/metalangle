//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

//            Based on Simple_Texture2D.c from
// Book:      OpenGL(R) ES 2.0 Programming Guide
// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
// ISBN-10:   0321502795
// ISBN-13:   9780321502797
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780321563835
//            http://www.opengles-book.com

#include "SampleApplication.h"

#include "texture_utils.h"
#include "util/shader_utils.h"

class SimpleTexture2DSample : public SampleApplication
{
  public:
    SimpleTexture2DSample(int argc, char **argv) : SampleApplication("SimpleTexture2D", argc, argv)
    {}

    bool initialize() override
    {
        constexpr char kVS[] = R"(attribute vec4 a_position;
attribute vec2 a_texCoord;
varying vec2 v_texCoord;
void main()
{
    gl_Position = a_position;
    v_texCoord = a_texCoord;
})";

        constexpr char kFS[] = R"(precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_texture;
void main()
{
    gl_FragColor = texture2D(s_texture, v_texCoord);
})";

        mProgram = CompileProgram(kVS, kFS);
        if (!mProgram)
        {
            return false;
        }

        // Get the attribute locations
        mPositionLoc = glGetAttribLocation(mProgram, "a_position");
        mTexCoordLoc = glGetAttribLocation(mProgram, "a_texCoord");

        // Get the sampler location
        mSamplerLoc = glGetUniformLocation(mProgram, "s_texture");

        // Load the texture
        mTexture = CreateSimpleTexture2D();

        // Create buffers
        GLfloat vertices[] = {
            -0.5f, 0.5f,  0.0f,  // Position 0
            0.0f,  0.0f,         // TexCoord 0
            -0.5f, -0.5f, 0.0f,  // Position 1
            0.0f,  1.0f,         // TexCoord 1
            0.5f,  -0.5f, 0.0f,  // Position 2
            1.0f,  1.0f,         // TexCoord 2
            0.5f,  0.5f,  0.0f,  // Position 3
            1.0f,  0.0f          // TexCoord 3
        };
        GLushort indices[] = {0, 1, 2, 0, 2, 3};

        glGenBuffers(2, mBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mBuffer[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        return true;
    }

    void destroy() override
    {
        glDeleteProgram(mProgram);
        glDeleteTextures(1, &mTexture);
        glDeleteBuffers(2, mBuffer);
    }

    void draw() override
    {
        // Set the viewport
        glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

        // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the program object
        glUseProgram(mProgram);

        glBindBuffer(GL_ARRAY_BUFFER, mBuffer[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer[1]);
        // Load the vertex position
        glVertexAttribPointer(mPositionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
        // Load the texture coordinate
        glVertexAttribPointer(mTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                              reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        glEnableVertexAttribArray(mPositionLoc);
        glEnableVertexAttribArray(mTexCoordLoc);

        // Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        // Set the texture sampler to texture unit to 0
        glUniform1i(mSamplerLoc, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    }

  private:
    // Handle to a program object
    GLuint mProgram;

    // Attribute locations
    GLint mPositionLoc;
    GLint mTexCoordLoc;

    // Sampler location
    GLint mSamplerLoc;

    // Texture handle
    GLuint mTexture;

    // Buffer handle
    GLuint mBuffer[2];
};

int main(int argc, char **argv)
{
    SimpleTexture2DSample app(argc, argv);
    return app.run();
}
