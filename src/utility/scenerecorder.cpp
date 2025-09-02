#include <iostream>
#include <utility>
#include "scenerecorder.h"


std::ostream& operator<<(std::ostream& os, const Boundary& b){
    for (const QVector3D& x: b.x){
        os << "b" << " " << x.x() << " " << x.y() << " " << x.z() << "\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const FluidFrame& frame){
    os << "t" << " " << frame.t << "\n";
    for (const QVector3D& x: frame.fluid.x){
        os << "f" << " " << x.x() << " " << x.y() << " " << x.z() << "\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const FluidParameters& params){
    os << "particlesize" << " " << params.h0 << "\n";
    os << "restdensity" << " " << params.rho0 << "\n";
    return os;
}

FluidRecording FluidRecording::fromFile(const std::string &path) {
//    float particle_size, kernel_support, rest_density;
//    FluidRecording recording;
//    std::ifstream is(path);
//    std::string ignore;
//    char label;
//    float x, y, z, t;
//    unsigned current_frame = -1;
//    QVector3D vec;
//
//    is >> ignore >> particle_size;
//    is >> ignore >> kernel_support;
//    is >> ignore >> rest_density;
//
//    while (is >> label){
//        switch (label){
//            case 't':
//                is >> t;
//                recording.frames.push_back({t, std::vector<QVector3D>()});
//                current_frame++;
//                break;
//            case 'f':
//                is >> x >> y >> z;
//                vec = {x, y, z};
//                recording.frames[current_frame].particles.push_back(vec);
//                break;
//            case 'b':
//                is >> x >> y >> z;
//                vec = {x, y, z};
//                recording.boundary.particles.push_back(vec);
//                break;
//            default:
//                std::cerr << "Something went wrong... sorry" << std::endl;
//        }
//    }
//    return recording;
}

//FluidRecording::FluidRecording(const FluidScene& scene) {
//
//}

void FluidRecording::writeToFile(const std::string &path) {

}

void SceneRecorder::init(const FluidScene &scene, float t) {
    recording.length = 0.0f;
    recording.boundary = {};
    recording.frames.clear();

    recording.parameters = scene.parameters;
    recording.boundary = scene.boundary_particles;
    FluidFrame frame = {t, scene.fluid_particles};
    recording.frames.push_back(frame);
}

void SceneRecorder::saveFrame(const FluidFrame &frame) {
    recording.frames.push_back(frame);
}

void SceneRecorder::writeToFile(const std::string &path) {
    std::ofstream os(path);
    os << recording.parameters;
//    os << recording.boundary;
    for (const auto& frame: recording.frames){
//        os << frame;
    }
    os.close();
}

void SceneRecorder::load(FluidRecording recording) {
    this->recording = std::move(recording);
}

unsigned SceneRecorder::size() const {
    return recording.frames.size();
}

const FluidFrame &SceneRecorder::getFrame(unsigned i) {
    return recording.frames.at(i);
}

const Boundary& SceneRecorder::getBoundary() const {
    return recording.boundary;
}

const FluidParameters &SceneRecorder::getParameters() const {
    return recording.parameters;
}
