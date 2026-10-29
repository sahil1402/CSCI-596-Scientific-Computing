#include "harbortune/experiment.hpp"
#include "harbortune/solver.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <map>
#include <stdexcept>
#include <tuple>
#ifndef HARBORTUNE_NO_OPENMP
#include <omp.h>
#endif
namespace harbortune { namespace {
std::vector<HarborConfig> designs(){std::vector<HarborConfig> v;for(auto p:{EntrancePosition::Left,EntrancePosition::LeftCenter,EntrancePosition::Center,EntrancePosition::RightCenter,EntrancePosition::Right})for(auto w:{EntranceWidth::Narrow,EntranceWidth::Medium,EntranceWidth::Wide})v.push_back({p,w,true});return v;}
double ratio(double value,double reference){return std::abs(reference)>1e-14?value/reference:0.0;}
using Key=std::pair<EntrancePosition,EntranceWidth>;
void score(SweepOutput& out){
 std::map<std::string,std::tuple<double,double,double>> references;
 for(auto& r:out.records)if(r.harbor.position==EntrancePosition::Center&&r.harbor.width==EntranceWidth::Medium)references[r.wave.name]={r.metrics.peak_abs_elevation,r.metrics.max_entrance_speed,r.metrics.mean_protected_energy};
 std::map<Key,std::vector<const ExperimentRecord*>> grouped;
 for(auto& r:out.records){auto ref=references.at(r.wave.name);r.normalized_peak=ratio(r.metrics.peak_abs_elevation,std::get<0>(ref));r.normalized_speed=ratio(r.metrics.max_entrance_speed,std::get<1>(ref));r.normalized_energy=ratio(r.metrics.mean_protected_energy,std::get<2>(ref));r.score=(r.normalized_peak+r.normalized_speed+r.normalized_energy)/3.0;grouped[{r.harbor.position,r.harbor.width}].push_back(&r);}
 for(auto& item:grouped){DesignRanking d{item.first.first,item.first.second};for(auto* r:item.second){d.robust_score=std::max(d.robust_score,r->score);d.mean_score+=r->score;d.worst_peak=std::max(d.worst_peak,r->normalized_peak);d.worst_speed=std::max(d.worst_speed,r->normalized_speed);}d.mean_score/=item.second.size();out.rankings.push_back(d);}
 std::sort(out.rankings.begin(),out.rankings.end(),[](const auto&a,const auto&b){return a.robust_score<b.robust_score;});for(std::size_t i=0;i<out.rankings.size();++i)out.rankings[i].rank=i+1;
}
}
SweepOutput run_sweep(SimulationConfig simulation,int threads,bool parallel){
 if(threads<1)throw std::invalid_argument("thread count must be positive");simulation.snapshot_count=0;auto harbors=designs();auto waves=built_in_wave_names();SweepOutput out;out.requested_threads=threads;out.parallel=parallel;out.records.resize(harbors.size()*waves.size());auto start=std::chrono::steady_clock::now();
#ifndef HARBORTUNE_NO_OPENMP
 if(parallel){
  omp_set_dynamic(0);
  omp_set_num_threads(threads);
  #pragma omp parallel
  {
   #pragma omp single
   out.actual_threads=omp_get_num_threads();
  }
  #pragma omp parallel for schedule(dynamic) num_threads(threads)
  for(long index=0;index<static_cast<long>(out.records.size());++index){auto hi=static_cast<std::size_t>(index)/waves.size(),wi=static_cast<std::size_t>(index)%waves.size();Simulator simulator(simulation,harbors[hi]);auto wave=wave_case(waves[wi]);out.records[index]={harbors[hi],wave,simulator.run(wave).metrics,omp_get_thread_num()};}
 }else
#endif
 {out.actual_threads=1;for(std::size_t index=0;index<out.records.size();++index){auto hi=index/waves.size(),wi=index%waves.size();Simulator simulator(simulation,harbors[hi]);auto wave=wave_case(waves[wi]);out.records[index]={harbors[hi],wave,simulator.run(wave).metrics,0};}}
 out.runtime_seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();score(out);return out;
}
void write_sweep_output(const std::filesystem::path& dir,const SweepOutput& out){std::filesystem::create_directories(dir);auto rp=dir/"sweep_results.csv";std::ofstream r(rp);if(!r)throw std::runtime_error("could not write "+rp.string());r<<std::setprecision(12)<<"position,width,wave,wave_type,stable,peak_abs_elevation,max_entrance_speed,mean_protected_energy,activity_duration,normalized_peak,normalized_speed,normalized_energy,score,worker,runtime_seconds\n";for(auto& x:out.records)r<<to_string(x.harbor.position)<<','<<to_string(x.harbor.width)<<','<<x.wave.name<<','<<to_string(x.wave.type)<<','<<x.metrics.stable<<','<<x.metrics.peak_abs_elevation<<','<<x.metrics.max_entrance_speed<<','<<x.metrics.mean_protected_energy<<','<<x.metrics.activity_duration<<','<<x.normalized_peak<<','<<x.normalized_speed<<','<<x.normalized_energy<<','<<x.score<<','<<x.worker<<','<<x.metrics.runtime_seconds<<'\n';
 auto dp=dir/"design_rankings.csv";std::ofstream d(dp);if(!d)throw std::runtime_error("could not write "+dp.string());d<<"rank,position,width,robust_score,mean_score,worst_normalized_peak,worst_normalized_speed\n";for(auto& x:out.rankings)d<<x.rank<<','<<to_string(x.position)<<','<<to_string(x.width)<<','<<x.robust_score<<','<<x.mean_score<<','<<x.worst_peak<<','<<x.worst_speed<<'\n';
 auto pp=dir/"sweep_performance.csv";std::ofstream p(pp);if(!p)throw std::runtime_error("could not write "+pp.string());p<<"mode,requested_threads,actual_threads,experiments,runtime_seconds\n"<<(out.parallel?"parallel":"sequential")<<','<<out.requested_threads<<','<<out.actual_threads<<','<<out.records.size()<<','<<out.runtime_seconds<<'\n';}
}
