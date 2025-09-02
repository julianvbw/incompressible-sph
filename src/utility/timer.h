#pragma once
#include <chrono>
#include <unordered_map>
#include <fstream>

struct Timer {
    std::chrono::time_point<std::chrono::system_clock> s, e;

    void start(){
        s = std::chrono::system_clock::now();
    }
    void stop(){
        e = std::chrono::system_clock::now();
    }

    long long duration() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(e - s).count();
    }

};

struct CSVTable: public std::unordered_map<std::string, std::vector<float>> {
    void writeToFile(const std::string& filename){
        std::ofstream os(filename);
        std::vector<std::string> keys;
        for (const auto& pair: *this) {
            keys.push_back(pair.first);
            os << pair.first << ',';
        }
        os.seekp(-1, std::ios_base::end); os << "\n";
        unsigned n = keys.size();
        if (n > 0) for (int row = 0; row < (*this)[keys[0]].size(); ++row){
            for (const auto& col: keys){
                os << (*this)[col][row] << ',';
            }
            os.seekp(-1, std::ios_base::end); os << "\n";
        }
    }
};