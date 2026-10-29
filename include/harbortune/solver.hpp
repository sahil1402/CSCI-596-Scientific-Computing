#pragma once
#include "harbortune/geometry.hpp"
namespace harbortune {
class Simulator {
public:
    Simulator(SimulationConfig simulation, HarborConfig harbor);
    SimulationOutput run(const WaveConfig& wave) const;
    const Geometry& geometry() const{return geometry_;} const SimulationConfig& config() const{return simulation_;}
    const HarborConfig& harbor() const{return harbor_;}
private: SimulationConfig simulation_; HarborConfig harbor_; Geometry geometry_;
};
}
