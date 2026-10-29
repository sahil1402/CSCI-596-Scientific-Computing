#pragma once
#include "harbortune/model.hpp"
#include <filesystem>
#include <vector>
namespace harbortune {
struct ExperimentRecord {
    HarborConfig harbor; WaveConfig wave; SimulationMetrics metrics; int worker=0;
    double normalized_peak=0,normalized_speed=0,normalized_energy=0,score=0;
};
struct DesignRanking {
    EntrancePosition position; EntranceWidth width; double robust_score=0,mean_score=0;
    double worst_peak=0,worst_speed=0; std::size_t rank=0;
};
struct SweepOutput {
    std::vector<ExperimentRecord> records; std::vector<DesignRanking> rankings;
    double runtime_seconds=0; int requested_threads=1,actual_threads=1; bool parallel=false;
};
SweepOutput run_sweep(SimulationConfig simulation,int threads,bool parallel);
void write_sweep_output(const std::filesystem::path& directory,const SweepOutput& output);
}
