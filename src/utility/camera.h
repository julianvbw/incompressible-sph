#pragma once
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>

class Camera {
public:
    QMatrix4x4 projection() const;

    QMatrix4x4 view() const;

    QVector3D direction() const;

    QVector3D right() const;

    QVector3D up() const;

    QVector3D position() const;

    void setAngles(float phi, float theta);

    void setPosition(const QVector3D& x);

    void setProjection(float fov, float ratio, float near_, float far_);

private:
    QVector3D eye, dir;
    QMatrix4x4 proj;
};
