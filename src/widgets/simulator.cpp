//
// Created by Julian on 11/4/2022.
//

#include <iostream>
#include "simulator.h"
#include <QThread>
#include <QApplication>

Simulator::Simulator(FluidScene scene, float interval):
    solver(100000),
    scene(std::move(scene)),
    t(0.0f), interval(interval) {
    timer = new QTimer(this);
    timer->setInterval(0);
    QObject::connect(timer, &QTimer::timeout, this, &Simulator::timestep);
    solver.initBoundaryMasses(scene);
}

void Simulator::timestep() {
    float t_ = t;
    do {
        if (QThread::currentThread()->isInterruptionRequested()){
            timer->stop();
            return;
        }
//        float dt = solver.timestepCFL(scene, 0.4);
        solver.integrate(scene, scene.parameters.dt);
        t += scene.parameters.dt;
    } while (t - t_ < interval);
    emit frameReady({t, scene.fluid_particles});
}

void Simulator::start() {
    timer->start();
}

void Simulator::stop() {
    timer->stop();
}


