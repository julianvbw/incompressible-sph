#pragma once
#include "../sph/fluidscene.h"
#include <string>
#include <fstream>
#include <memory>

struct FluidFrame {
    float t;
    Fluid fluid;
};

struct FluidRecording {
//    FluidRecording(const FluidScene& scene);

    static FluidRecording fromFile(const std::string& path);

    void writeToFile(const std::string& path);

    float length;
    FluidParameters parameters;
    Boundary boundary;
    std::vector<FluidFrame> frames;
};

class SceneRecorder {
public:
    SceneRecorder() = default;

    void init(const FluidScene& scene, float t);

    void load(FluidRecording recording);

    const FluidFrame& getFrame(unsigned i);

    const Boundary& getBoundary() const;

    const FluidParameters& getParameters() const;

    unsigned size() const;

    void saveFrame(const FluidFrame &frame);

    void writeToFile(const std::string& path);

private:
    FluidRecording recording;
};
