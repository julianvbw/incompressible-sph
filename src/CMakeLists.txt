cmake_minimum_required(VERSION 3.16)
project(sim)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenMP)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 20)

add_executable(sim
        resources.qrc
        main.cpp
        widgets/mainwindow.cpp widgets/mainwindow.h
        widgets/viewport_gl.cpp widgets/viewport_gui.cpp widgets/viewport.h
        sph/fluidsolver.cpp sph/fluidsolver.h
        sph/kernel.cpp sph/kernel.h
        sph/fluidscene.cpp sph/fluidscene.h
        sph/pressuresolver.cpp sph/pressuresolver.h
        utility/camera.cpp utility/camera.h
        spatialhashtable/neighbours.cpp spatialhashtable/neighbours.h
        spatialhashtable/spatialhashtable.cpp spatialhashtable/spatialhashtable.h
        utility/timer.h
        spatialhashtable/cellpool.cpp spatialhashtable/cellpool.h
        spatialhashtable/common.cpp spatialhashtable/common.h
        utility/scenerecorder.cpp utility/scenerecorder.h
        scenarios/scenario.cpp scenarios/scenario.h
        widgets/simulator.cpp widgets/simulator.h)

target_link_libraries(sim
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui)

target_link_libraries(sim OpenMP::OpenMP_CXX)