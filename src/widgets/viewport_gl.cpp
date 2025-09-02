//
// Created by Julian on 5/20/2022.
//

#include <iostream>
#include <QtMath>
#include "viewport.h"

void ViewportWidget::initializeGL() {
    initializeOpenGLFunctions();
    initShaders();
    initQuadVBO();
    initQuadVAO();

    glClearColor(.4,.4,.4,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ViewportWidget::initShaders() {
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex.glsl");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment.glsl");
    shader.link();
    shader.bind();
}

void ViewportWidget::initQuadVBO() {
    std::vector<QVector3D> vbo(6);
    vbo[0] = {0, 0, 0};
    vbo[1] = {1, 0, 0};
    vbo[2] = {0, 1, 0};
    vbo[3] = {0, 1, 0};
    vbo[4] = {1, 0, 0};
    vbo[5] = {1, 1, 0};

    quadVBOSize = 6;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(QVector3D), vbo.data(), GL_STATIC_DRAW);
}

void ViewportWidget::initQuadVAO() {
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QVector3D), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void ViewportWidget::drawQuad(const QMatrix4x4 &model, QVector4D color) {
    shader.bind();
    shader.setUniformValue("model", model);
    shader.setUniformValue("view", cam.view());
    shader.setUniformValue("proj", cam.projection());
    shader.setUniformValue("cam", cam.position());
    shader.setUniformValue("color", color);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, quadVBOSize);
    glBindVertexArray(0);
}

void ViewportWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Quads should always face the viewer, we therefore ignore the translational part of the view transform and invert
    QMatrix4x4 vrot = cam.view();
    vrot.setColumn(3, {0, 0, 0, 1});
    vrot = vrot.inverted();

    if (sceneInit) {
        const Fluid &f = rec.getFrame(frame).fluid;
        const Boundary &b = rec.getBoundary();

        unsigned fn = f.x.size();
        unsigned bn = b.x.size();

        FluidParameters fp = rec.getParameters();

        // Draw fluid particles
        float pmax = *std::max_element(f.p.begin(), f.p.end());
        float pmin = *std::min_element(f.p.begin(), f.p.end());
        for (unsigned i = 0; i < fn; ++i) {
            QMatrix4x4 m;
            m.setToIdentity();
            m.translate(f.x[i]);
            m.scale(2.0f * fp.h0);
            float v = f.v[i].length();
//        float pnorm = f.p[i]/10;
            float pnorm = f.p[i] / (pmax - pmin + 0.001);
//        float w = f.w[i].length();
            drawQuad(m * vrot,
//           QVector4D(1-f.rho[i]/2.0f, f.rho[i]/2.0f, 0.0f, 1)
//           QVector4D(1, 1-pnorm, 1-pnorm, 0.2 + pnorm)
                     QVector4D(v / 10, v / 10, 0.5f + v / 10, 1)
//           QVector4D(500*w, 500*w, 0.5 + 500*w, 1.0)
//           QVector4D(.0, .0, 0.4, 1.0)
            );
        }

        if (toggles[(int) Toggles::SHOW_BOUNDARY])
            // Draw boundary particles
            for (unsigned i = 0; i < bn; ++i) {
                QMatrix4x4 m;
                m.setToIdentity();
                m.translate(b.x[i]);
                m.scale(2.0f * fp.h0);
                float m0 = fp.rho0 * (fp.h0 * fp.h0 * fp.h0);
                drawQuad(m * vrot,
                 QVector4D(.6, .6, .6, 1.0)
//                         QVector4D(1 - b.m[i] / m0, b.m[i] / m0, 0.0f, 1)
                );
            }
    }
}