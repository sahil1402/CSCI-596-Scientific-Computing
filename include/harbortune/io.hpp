#pragma once
#include "harbortune/geometry.hpp"
#include <filesystem>
namespace harbortune {
void write_simulation_output(const std::filesystem::path&,const SimulationConfig&,const HarborConfig&,const WaveConfig&,const Geometry&,const SimulationOutput&);
}
