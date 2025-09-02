//
// Created by Julian on 5/20/2022.
//

#include <QToolBar>
#include <QPushButton>
#include <QMenuBar>
#include <QSlider>
#include <QLabel>
#include "mainwindow.h"
#include "../scenarios/scenario.h"
#include <iostream>

MainWindow::MainWindow(): QMainWindow(nullptr) {
    viewport = new ViewportWidget(this, 20);
    viewport->setFocusPolicy(Qt::StrongFocus);
    setCentralWidget(viewport);

    thread = nullptr;
    sim = nullptr;
    initToolbar();

//    auto* mbar = new QMenuBar(this);
//    auto* menu = new QMenu("file", mbar);
//    auto* save = new QAction("Save recording", menu);
//    connect(save, &QAction::triggered, this, [=]{
//        viewport->saveRecordingDialog();
//    });
//    mbar->addMenu(menu);
//    menu->addAction(save);
//    setMenuBar(mbar);
    initScene(scenario::createFluidColumn());
}

void MainWindow::initToolbar() {
    auto* toolbar = new QToolBar(this);

    sim_action = toolbar->addAction("Simulate");
    sim_action->setCheckable(true);
    sim_action->setChecked(false);

    toolbar->addSeparator();

    auto* dam = toolbar->addAction("Breaking dam");
    connect(dam, &QAction::triggered, this, [=]{
        initScene(scenario::createDamBreak());
    });
    auto* droplet = toolbar->addAction("Droplet");
    connect(droplet, &QAction::triggered, this, [=]{
        initScene(scenario::createDroplet());
    });
    auto* column = toolbar->addAction("Fluid column");
    connect(column, &QAction::triggered, this, [=]{
        initScene(scenario::createFluidColumn());
    });

    toolbar->addSeparator();

    auto* play = toolbar->addAction("Play");
    play->setCheckable(true);
    play->setChecked(false);
    QObject::connect(play, &QAction::triggered, viewport, [=](bool checked){
        viewport->setToggle(Toggles::PLAY_RECORDING, checked);
    });

    auto* slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setMaximum(0);
    slider->setSliderPosition(0);
    slider->setMaximumWidth(100);
    QObject::connect(slider, &QSlider::sliderMoved, viewport, &ViewportWidget::setToFrame);
    QObject::connect(viewport, &ViewportWidget::frameChanged, slider, &QSlider::setValue);
    QObject::connect(viewport, &ViewportWidget::recordingSizeChanged, slider, &QSlider::setRange);
    toolbar->addWidget(slider);

    toolbar->addWidget(new QLabel(" Frame: ", this));

    auto* frame_label = new QLabel(this);
    frame_label->setNum(0);
    QObject::connect(viewport, &ViewportWidget::frameChanged, frame_label, [=](unsigned i) {
        frame_label->setNum((int)i);
    });
    toolbar->addWidget(frame_label);

    toolbar->addWidget(new QLabel("/", this));

    auto* size_label = new QLabel(this);
    size_label->setNum(0);
    QObject::connect(viewport, &ViewportWidget::recordingSizeChanged, size_label, [=](unsigned min, unsigned max){
        size_label->setNum((int)max);
    });
    toolbar->addWidget(size_label);

    toolbar->setAllowedAreas(Qt::BottomToolBarArea);
    addToolBar(toolbar);
}

MainWindow::~MainWindow() {
    deleteSimulator();
}

void MainWindow::deleteSimulator() {
    if (thread){
        thread->requestInterruption();
        thread->quit();
        thread->wait();
        thread->deleteLater();
    }
    if (sim_action) sim_action->setChecked(false);
    if (sim) sim->deleteLater();
}

void MainWindow::initScene(FluidScene scene) {
    deleteSimulator();
    viewport->initScene(scene);
    thread = new QThread(this);
    sim = new Simulator(std::move(scene), 20.f / 1000.f);
    sim->moveToThread(thread);
    QObject::connect(sim, &Simulator::frameReady, viewport, &ViewportWidget::handleFrame, Qt::BlockingQueuedConnection);
    QObject::connect(sim_action, &QAction::triggered, sim, [=](bool checked){
        if (checked) sim->start(); else sim->stop();
    });
    thread->start();
}