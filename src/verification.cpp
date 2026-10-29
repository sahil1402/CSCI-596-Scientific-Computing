#include "harbortune/verification.hpp"
#include "harbortune/experiment.hpp"
#include "harbortune/solver.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
namespace harbortune { namespace {
struct Check{std::string name;double value,expected,relative_error,tolerance;bool passed;};
double relative(double a,double b){return std::abs(a-b)/std::max(std::abs(b),1e-14);}
void add(std::vector<Check>& checks,const std::string& name,double value,double expected,double tolerance){double error=relative(value,expected);checks.push_back({name,value,expected,error,tolerance,error<=tolerance});}
double sensor_peak_time(const SimulationOutput& output,const Geometry& geometry,std::size_t i,std::size_t j){double best=-1,time=0;auto k=geometry.index(i,j);for(auto& s:output.snapshots)if(s.eta[k]>best){best=s.eta[k];time=s.time;}return time;}
}
VerificationSummary run_verification(const std::filesystem::path& dir){std::filesystem::create_directories(dir);std::vector<Check> checks;
 // Empty-domain pulse: measured propagation speed must agree with sqrt(gH).
 SimulationConfig speed;speed.nx=96;speed.ny=48;speed.length_x=800;speed.length_y=400;speed.duration=75;speed.measurement_start=0;speed.snapshot_count=220;HarborConfig empty;empty.breakwater_enabled=false;WaveConfig pulse=wave_case("pulse");pulse.pulse_center=8;pulse.pulse_sigma=2.5;Simulator speed_sim(speed,empty);auto speed_out=speed_sim.run(pulse);auto i1=static_cast<std::size_t>(.25*(speed.nx-1)),i2=static_cast<std::size_t>(.55*(speed.nx-1)),j=speed.ny/2;double t1=sensor_peak_time(speed_out,speed_sim.geometry(),i1,j),t2=sensor_peak_time(speed_out,speed_sim.geometry(),i2,j);double dx=(i2-i1)*speed.length_x/(speed.nx-1),measured=dx/(t2-t1),theory=std::sqrt(speed.gravity*speed.depth);add(checks,"wave_speed",measured,theory,.12);checks.push_back({"finite_stable_solution",speed_out.metrics.stable?1.0:0.0,1.0, speed_out.metrics.stable?0.0:1.0,0.0,speed_out.metrics.stable});
 // Mirror geometry: uniform forcing should give the same scalar metrics.
 SimulationConfig sym;sym.nx=64;sym.ny=40;sym.duration=95;sym.measurement_start=15;HarborConfig left{EntrancePosition::LeftCenter,EntranceWidth::Medium,true},right{EntrancePosition::RightCenter,EntranceWidth::Medium,true};auto lm=Simulator(sym,left).run(pulse).metrics,rm=Simulator(sym,right).run(pulse).metrics;add(checks,"mirror_peak_elevation",lm.peak_abs_elevation,rm.peak_abs_elevation,1e-10);add(checks,"mirror_entrance_speed",lm.max_entrance_speed,rm.max_entrance_speed,1e-10);add(checks,"mirror_mean_energy",lm.mean_protected_energy,rm.mean_protected_energy,1e-10);
 // Sequential and OpenMP sweeps must produce the same case metrics.
 SimulationConfig parallel;parallel.nx=40;parallel.ny=26;parallel.duration=55;parallel.measurement_start=10;auto seq=run_sweep(parallel,1,false),par=run_sweep(parallel,2,true);double max_difference=0;for(std::size_t k=0;k<seq.records.size();++k){max_difference=std::max(max_difference,relative(seq.records[k].metrics.peak_abs_elevation,par.records[k].metrics.peak_abs_elevation));max_difference=std::max(max_difference,relative(seq.records[k].metrics.max_entrance_speed,par.records[k].metrics.max_entrance_speed));}add(checks,"sequential_parallel_agreement",max_difference,0.0,1e-12);
 // A simple refinement study is recorded for interpretation rather than asserted as monotonic.
 auto refinement_path=dir/"grid_refinement.csv";std::ofstream refinement(refinement_path);if(!refinement)throw std::runtime_error("could not write "+refinement_path.string());refinement<<"nx,ny,dt,peak_abs_elevation,max_entrance_speed,mean_protected_energy\n";for(auto dims:{std::pair<std::size_t,std::size_t>{48,30},{72,44},{96,58}}){SimulationConfig c;c.nx=dims.first;c.ny=dims.second;c.duration=110;c.measurement_start=20;auto m=Simulator(c,HarborConfig{}).run(pulse).metrics;refinement<<c.nx<<','<<c.ny<<','<<m.dt<<','<<m.peak_abs_elevation<<','<<m.max_entrance_speed<<','<<m.mean_protected_energy<<'\n';}
 auto path=dir/"verification.csv";std::ofstream out(path);if(!out)throw std::runtime_error("could not write "+path.string());out<<std::setprecision(12)<<"check,value,expected,relative_error,tolerance,passed\n";VerificationSummary summary;for(auto& c:checks){out<<c.name<<','<<c.value<<','<<c.expected<<','<<c.relative_error<<','<<c.tolerance<<','<<c.passed<<'\n';if(c.passed)++summary.passed;else ++summary.failed;}return summary;}
}
