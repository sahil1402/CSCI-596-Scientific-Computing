#include "harbortune/io.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
namespace harbortune { namespace { void check(const std::ofstream& f,const std::filesystem::path& p){if(!f)throw std::runtime_error("could not write "+p.string());} }
void write_simulation_output(const std::filesystem::path& dir,const SimulationConfig& s,const HarborConfig& h,const WaveConfig& w,const Geometry& g,const SimulationOutput& out){
 std::filesystem::create_directories(dir/"snapshots");auto sp=dir/"summary.csv";std::ofstream sum(sp);check(sum,sp);sum<<std::setprecision(12)<<"position,width,wave,wave_type,nx,ny,dt,steps,stable,peak_abs_elevation,max_entrance_speed,mean_protected_energy,activity_duration,final_mass_anomaly,max_abs_elevation_anywhere,peak_i,peak_j,runtime_seconds\n"<<to_string(h.position)<<','<<to_string(h.width)<<','<<w.name<<','<<to_string(w.type)<<','<<s.nx<<','<<s.ny<<','<<out.metrics.dt<<','<<out.metrics.steps<<','<<(out.metrics.stable?1:0)<<','<<out.metrics.peak_abs_elevation<<','<<out.metrics.max_entrance_speed<<','<<out.metrics.mean_protected_energy<<','<<out.metrics.activity_duration<<','<<out.metrics.final_mass_anomaly<<','<<out.metrics.max_abs_elevation_anywhere<<','<<out.metrics.peak_i<<','<<out.metrics.peak_j<<','<<out.metrics.runtime_seconds<<'\n';
 auto gp=dir/"geometry.csv";std::ofstream gf(gp);check(gf,gp);gf<<"i,j,x,y,solid,protected,entrance\n";double dx=s.length_x/(s.nx-1),dy=s.length_y/(s.ny-1);for(std::size_t j=0;j<s.ny;++j)for(std::size_t i=0;i<s.nx;++i)gf<<i<<','<<j<<','<<i*dx<<','<<j*dy<<','<<g.solid(i,j)<<','<<g.protected_cell(i,j)<<','<<g.entrance_cell(i,j)<<'\n';
 auto tp=dir/"snapshot_times.csv";std::ofstream times(tp);check(times,tp);times<<"frame,time,file\n";for(std::size_t frame=0;frame<out.snapshots.size();++frame){std::ostringstream name;name<<"frame_"<<std::setw(4)<<std::setfill('0')<<frame<<".csv";auto fp=dir/"snapshots"/name.str();std::ofstream f(fp);check(f,fp);f<<std::setprecision(10)<<"i,j,eta,u,v,speed\n";auto& snap=out.snapshots[frame];for(std::size_t j=0;j<s.ny;++j)for(std::size_t i=0;i<s.nx;++i){auto k=g.index(i,j);f<<i<<','<<j<<','<<snap.eta[k]<<','<<snap.u[k]<<','<<snap.v[k]<<','<<std::hypot(snap.u[k],snap.v[k])<<'\n';}times<<frame<<','<<snap.time<<",snapshots/"<<name.str()<<'\n';}
}
}
