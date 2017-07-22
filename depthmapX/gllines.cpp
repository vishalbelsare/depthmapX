// depthmapX - spatial network analysis platform
// Copyright (C) 2017, Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "gllines.h"
#include <qmath.h>
#include <genlib/exceptions.h>

static const char *vertexShaderSourceCore =
    "#version 150\n"
    "in vec4 vertex;\n"
    "in vec3 colour;\n"
    "out vec3 col;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   col = colour.xyz;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore =
    "#version 150\n"
    "in vec3 col;\n"
    "out highp vec3 fragColor;\n"
    "void main() {\n"
    "   fragColor = col;\n"
    "}\n";

static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 colour;\n"
    "varying vec3 col;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   col = colour.xyz;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying vec3 col;\n"
    "void main() {\n"
    "   gl_FragColor = vec4(col, 1.0);\n"
    "}\n";

/**
 * @brief GLLines::GLLines
 * This class is an OpenGL representation of  multiple lines of uniform colour
 */

GLLines::GLLines()
    : m_count(0),
      m_program(0)
{

}

void GLLines::loadLineData(const std::vector<std::pair<SimpleLine, PafColor>> &colouredLines)
{
    built = false;
    m_data.resize(colouredLines.size() * 2 * DATA_DIMENSIONS);
    std::vector<std::pair<SimpleLine, PafColor>>::const_iterator iter = colouredLines.begin(), end =
    colouredLines.end();
    for ( ; iter != end; ++iter )
    {
        const SimpleLine &line = iter->first;
        const PafColor &colour = iter->second;

        QVector3D colourVector(colour.redf(), colour.greenf(), colour.bluef());
        add(QVector3D(line.start().x, line.start().y, 0.0f), colourVector);
        add(QVector3D(line.end().x, line.end().y, 0.0f), colourVector);
    }
}

void GLLines::setupVertexAttribs()
{
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_DIMENSIONS * sizeof(GLfloat),
                             0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_DIMENSIONS * sizeof(GLfloat),
                             reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_vbo.release();
}

void GLLines::initializeGL(bool m_core)
{
    if(m_data.size() == 0) return;
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("colour", 1);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(constData(), m_count * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();
    m_program->release();
    built = true;
}

void GLLines::cleanup()
{
    if(!built) return;
    m_vbo.destroy();
    delete m_program;
    m_program = 0;
}

void GLLines::paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView, const QMatrix4x4 &m_mModel)
{
    if(!built) return;
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_mProj);
    m_program->setUniformValue(m_mvMatrixLoc, m_mView * m_mModel);

    glDrawArrays(GL_LINES, 0, vertexCount());

    m_program->release();
}

void GLLines::add(const QVector3D &v, const QVector3D &c)
{
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = c.x();
    *p++ = c.y();
    *p++ = c.z();
    m_count += DATA_DIMENSIONS;
}
