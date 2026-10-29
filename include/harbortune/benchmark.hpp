#pragma once
#include "harbortune/model.hpp"
#include <filesystem>
#include <vector>
namespace harbortune {
struct BenchmarkRecord { int threads=1,repetitions=1; double mean_seconds=0,stddev_seconds=0,min_seconds=0,speedup=1,efficiency=1; };
std::vector<BenchmarkRecord> run_benchmark(const SimulationConfig&,const std::vector<int>& thread_counts,int repetitions);
void write_benchmark(const std::filesystem::path&,const SimulationConfig&,const std::vector<BenchmarkRecord>&);
}
