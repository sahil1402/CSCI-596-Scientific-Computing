#include "harbortune/benchmark.hpp"
#include "harbortune/experiment.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <stdexcept>
namespace harbortune {
std::vector<BenchmarkRecord> run_benchmark(const SimulationConfig& simulation,const std::vector<int>& counts,int repetitions){if(counts.empty()||repetitions<1)throw std::invalid_argument("benchmark requires thread counts and repetitions");std::vector<int> threads=counts;if(std::find(threads.begin(),threads.end(),1)==threads.end())threads.insert(threads.begin(),1);for(int t:threads)if(t<1)throw std::invalid_argument("thread counts must be positive");std::sort(threads.begin(),threads.end());threads.erase(std::unique(threads.begin(),threads.end()),threads.end());std::vector<BenchmarkRecord> records;for(int t:threads){std::vector<double> samples;for(int rep=0;rep<repetitions;++rep)samples.push_back(run_sweep(simulation,t,t>1).runtime_seconds);double mean=std::accumulate(samples.begin(),samples.end(),0.0)/samples.size(),variance=0;for(double sample:samples)variance+=(sample-mean)*(sample-mean);variance/=samples.size();records.push_back({t,repetitions,mean,std::sqrt(variance),*std::min_element(samples.begin(),samples.end()),1,1});}double baseline=records.front().mean_seconds;for(auto& r:records){r.speedup=baseline/r.mean_seconds;r.efficiency=r.speedup/r.threads;}return records;}
void write_benchmark(const std::filesystem::path& path,const SimulationConfig& simulation,const std::vector<BenchmarkRecord>& records){std::filesystem::create_directories(path.parent_path());std::ofstream out(path);if(!out)throw std::runtime_error("could not write "+path.string());out<<std::setprecision(12)<<"nx,ny,duration,threads,repetitions,mean_seconds,stddev_seconds,min_seconds,speedup,efficiency\n";for(auto&r:records)out<<simulation.nx<<','<<simulation.ny<<','<<simulation.duration<<','<<r.threads<<','<<r.repetitions<<','<<r.mean_seconds<<','<<r.stddev_seconds<<','<<r.min_seconds<<','<<r.speedup<<','<<r.efficiency<<'\n';}
}
