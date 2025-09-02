#include "camera.h"
#include <qmath.h>

QMatrix4x4 Camera::view() const {
    QMatrix4x4 view;
    view.setToIdentity();
    view.lookAt(eye, eye + dir, up());
    return view;
}

QVector3D Camera::direction() const {
    return dir;
}

void Camera::setAngles(float yaw, float pitch) {
    qreal phi = qDegreesToRadians(yaw);
    qreal theta = qDegreesToRadians(pitch);
    dir = QVector3D(qCos(phi) * qCos(theta), qSin(theta), qSin(phi) * qCos(theta)).normalized();
}

void Camera::setPosition(const QVector3D &x) {
    eye = x;
}

QVector3D Camera::right() const {
    QVector3D local_up = {0, 1, 0};
    return QVector3D::crossProduct(dir, local_up).normalized();
}

QVector3D Camera::up() const {
    return QVector3D::crossProduct(right(), dir);
}

QVector3D Camera::position() const {
    return eye;
}

void Camera::setProjection(float fov, float ratio, float near_, float far_) {
    proj.setToIdentity();
    proj.perspective(fov, ratio, near_, far_);
}

QMatrix4x4 Camera::projection() const {
    return proj;
}
