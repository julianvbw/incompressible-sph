#pragma once
#include <QMainWindow>
#include <memory>
#include <QThread>
#include "viewport.h"
#include "simulator.h"
#include <QSlider>

class MainWindow: public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

    ~MainWindow() override;

public slots:
    void initScene(FluidScene scene);

private:
    void deleteSimulator();

private:
    void initToolbar();

    QAction *sim_action;
    QThread *thread;
    Simulator *sim;
    ViewportWidget *viewport;
};