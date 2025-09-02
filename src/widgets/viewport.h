#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include "../sph/fluidsolver.h"
#include "../utility/camera.h"
#include "../utility/scenerecorder.h"
#include "simulator.h"

enum class Controls {
    FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, enum_count
};

enum class Toggles {
    SHOW_BOUNDARY, PLAY_RECORDING, enum_count
};

class ViewportWidget: public QOpenGLWidget, public QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public: // functions
    ViewportWidget(QWidget* parent, int tick_interval);

    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int h, int w) override;

    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

public slots:
    void loop();

    void initScene(const FluidScene& scene);

    void setToggle(Toggles toggle, bool state);

    void saveRecordingDialog();

    void setToFrame(unsigned i);

    void handleFrame(const FluidFrame& frame);

signals:
    void frameChanged(unsigned i);

    void recordingSizeChanged(unsigned min, unsigned max);

private: // functions
    void initShaders();

    void initQuadVBO();

    void initQuadVAO();

    void drawQuad(const QMatrix4x4 &model, QVector4D color);

    bool updateCamera();

private: // variables
    bool sceneInit;

    QTimer* tick;
    int interval;
    unsigned frame;

    QOpenGLShaderProgram shader;

    Camera cam;
    float lookspeed = 0.1;
    bool mouseclick;
    std::vector<bool> keys, toggles;
    std::map<int, Controls> keymap;
    std::map<int, Toggles> togglemap;
    QVector2D viewangle, viewcenter, moveinit;

    SceneRecorder rec;

    GLuint quadVAO, quadVBO;
    GLsizei quadVBOSize;
};