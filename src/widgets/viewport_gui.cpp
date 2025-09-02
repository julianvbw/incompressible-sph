#include "viewport.h"
#include <QKeyEvent>
#include <iostream>
#include <cmath>
#include <QFileDialog>
#include <QThread>

ViewportWidget::ViewportWidget(QWidget* parent, int tick_interval):
        QOpenGLWidget(parent),
        tick(new QTimer(this)),
        interval(tick_interval), frame(0), sceneInit(false),
        keys(static_cast<int>(Controls::enum_count), false),
        toggles(static_cast<int>(Toggles::enum_count), false) {


    QCursor c = cursor();
    c.setShape(Qt::ClosedHandCursor);
    setCursor(c);
    mouseclick = false;
    setMouseTracking(true);

    tick->setInterval(interval);
    QObject::connect(tick, SIGNAL(timeout()), this, SLOT(loop()));

    viewangle = {-90, 0};
    cam.setAngles(-90, 0);
    cam.setPosition({0, 0, 1});
    float w = width();
    float h = height();
    if (w >= h)
        cam.setProjection(80, w/h, 0.1, 1000);
    else
        cam.setProjection(80, h/w, 0.1, 1000);

    keymap = {
            {Qt::Key_W, Controls::FORWARD},
            {Qt::Key_A, Controls::LEFT},
            {Qt::Key_S, Controls::BACKWARD},
            {Qt::Key_D, Controls::RIGHT},
            {Qt::Key_Space, Controls::UP},
            {Qt::Key_Shift, Controls::DOWN}
    };

    togglemap = {
            {Qt::Key_O, Toggles::SHOW_BOUNDARY}
    };

    tick->start();
}

void ViewportWidget::resizeGL(int w, int h) {
    FluidParameters params = rec.getParameters();
    glViewport(0, 0, h, w);
    viewcenter.setX(w/2);
    viewcenter.setY(h/2);
    cam.setProjection(80, (float)w/(float)h, 0.1f * params.h0, 1000);
    update();
}

void ViewportWidget::keyPressEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat())
        keys[(int)keymap[event->key()]] = true;
}

void ViewportWidget::keyReleaseEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        toggles[(int) togglemap[event->key()]] = !toggles[(int) togglemap[event->key()]];
        keys[(int) keymap[event->key()]] = false;
    }
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event) {
    QCursor c = cursor();
    if (mouseclick) {
        c.setShape(Qt::BlankCursor);
        setCursor(c);

        QVector2D pos(event->pos());
        QVector2D d = pos - moveinit;
        d[1] = -d[1];
        viewangle += lookspeed * d;
        viewangle[1] = std::min(viewangle[1], +89.0f);
        viewangle[1] = std::max(viewangle[1], -89.0f);
        cam.setAngles(viewangle[0], viewangle[1]);

        QCursor::setPos(mapToGlobal(moveinit.toPoint()));
        update();
    } else {
        c.setShape(Qt::ClosedHandCursor);
        setCursor(c);
    }
}

void ViewportWidget::mousePressEvent(QMouseEvent* event) {
    moveinit = QVector2D(event->pos());
    mouseclick |= true;
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent* event) {
    mouseclick &= false;
}

bool ViewportWidget::updateCamera() {
    bool changed = false;
    QVector3D pos = cam.position();
    float movespeed = rec.getParameters().h0;
    if (keys[(int)Controls::FORWARD])
        pos += movespeed * cam.direction();
    if (keys[(int)Controls::BACKWARD])
        pos -= movespeed * cam.direction();
    if (keys[(int)Controls::RIGHT])
        pos += movespeed * cam.right();
    if (keys[(int)Controls::LEFT])
        pos -= movespeed * cam.right();
    if (keys[(int)Controls::UP])
        pos += movespeed * cam.up();
    if (keys[(int)Controls::DOWN])
        pos -= movespeed * cam.up();
    if (pos != cam.position()) {
        cam.setPosition(pos);
        return true;
    }
    return false;
}

void ViewportWidget::setToggle(Toggles toggle, bool state) {
    toggles[(int)toggle] = state;
}

void ViewportWidget::saveRecordingDialog() {
    QString filename = QFileDialog::getSaveFileName(this, "Save recording", "recording.viewport", "Simulation (*.viewport)");
    rec.writeToFile(filename.toStdString());
}


void ViewportWidget::handleFrame(const FluidFrame &frame) {
    rec.saveFrame(frame);
    emit recordingSizeChanged(0, rec.size() - 1);
}

void ViewportWidget::initScene(const FluidScene& scene) {
    sceneInit = true;
    rec.init(scene, 0.0f);
    emit recordingSizeChanged(0, 0);
    setToFrame(0);
    update();
}

void ViewportWidget::setToFrame(unsigned i) {
    if (i < rec.size() - 1) {
        emit frameChanged(frame);
        frame = i;
        update();
    }
}

void ViewportWidget::loop() {
    bool camera_changed = updateCamera();

    unsigned frame_ = frame;
    if (toggles[(int) Toggles::PLAY_RECORDING])
        frame = std::min(frame + 1, rec.size() - 1);

    if (frame_ != frame)
        emit frameChanged(frame);

    if (camera_changed || (frame_ != frame))
        update();
}