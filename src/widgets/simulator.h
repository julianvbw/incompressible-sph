#pragma once
#include <QTimer>
#include "../utility/scenerecorder.h"
#include "../sph/fluidsolver.h"

class Simulator: public QObject {
    Q_OBJECT
public:
    Simulator(FluidScene scene, float interval);

public slots:
    void timestep();

    void start();

    void stop();

signals:
    void frameReady(const FluidFrame& frame);

private:
    QTimer* timer;
    float t, interval;
    FluidScene scene;
    FluidSolver solver;
};
