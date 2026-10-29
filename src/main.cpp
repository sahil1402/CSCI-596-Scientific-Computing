#include "harbortune/io.hpp"
#include "harbortune/model.hpp"
#include "harbortune/solver.hpp"
#include "harbortune/version.hpp"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
namespace {
using Options=std::map<std::string,std::string>;
void help(){std::cout<<"HarborTune - parallel screening of breakwater entrances\n\nUsage:\n  harbortune simulate [options]\n  harbortune list\n  harbortune --version\n\nsimulate options:\n  --position NAME     left, left-center, center, right-center, right\n  --width NAME        narrow, medium, wide\n  --wave NAME         short, medium, long, pulse\n  --output PATH       output directory (default: results/data/simulation)\n  --nx N --ny N       grid dimensions (default: 120 x 72)\n  --duration SECONDS  simulated duration (default: 140)\n  --snapshots N       exported frames (default: 40)\n";}
Options parse(int argc,char** argv,int begin){Options o;for(int i=begin;i<argc;++i){std::string k=argv[i];if(k.rfind("--",0)!=0)throw std::invalid_argument("expected an option, received: "+k);if(i+1>=argc)throw std::invalid_argument("missing value for "+k);o[k.substr(2)]=argv[++i];}return o;}
std::string opt(const Options&o,const std::string&k,const std::string&d){auto it=o.find(k);return it==o.end()?d:it->second;}
std::size_t size_opt(const Options&o,const std::string&k,std::size_t d){auto it=o.find(k);return it==o.end()?d:static_cast<std::size_t>(std::stoull(it->second));}
double double_opt(const Options&o,const std::string&k,double d){auto it=o.find(k);return it==o.end()?d:std::stod(it->second);}
int simulate(const Options&o){
 harbortune::SimulationConfig s;s.nx=size_opt(o,"nx",s.nx);s.ny=size_opt(o,"ny",s.ny);s.duration=double_opt(o,"duration",s.duration);s.measurement_start=double_opt(o,"measurement-start",std::min(20.0,.2*s.duration));s.snapshot_count=size_opt(o,"snapshots",40);
 harbortune::HarborConfig h;h.position=harbortune::parse_position(opt(o,"position","center"));h.width=harbortune::parse_width(opt(o,"width","medium"));auto w=harbortune::wave_case(opt(o,"wave","pulse"));std::filesystem::path dir=opt(o,"output","results/data/simulation");
 std::cout<<"Running "<<harbortune::to_string(h.position)<<'/'<<harbortune::to_string(h.width)<<" with "<<w.name<<" forcing on a "<<s.nx<<'x'<<s.ny<<" grid...\n";
 harbortune::Simulator simulator(s,h);auto output=simulator.run(w);harbortune::write_simulation_output(dir,s,h,w,simulator.geometry(),output);auto&m=output.metrics;
 std::cout<<"Stable: "<<(m.stable?"yes":"no")<<"\nPeak protected elevation: "<<m.peak_abs_elevation<<" m\nMaximum entrance speed: "<<m.max_entrance_speed<<" m/s\nMean protected energy: "<<m.mean_protected_energy<<" J/m^2\nRuntime: "<<m.runtime_seconds<<" s\nOutput: "<<std::filesystem::absolute(dir)<<'\n';return m.stable?0:1;
}
void list(){std::cout<<"Entrance positions: left, left-center, center, right-center, right\nEntrance widths: narrow, medium, wide\nWave cases:\n";for(auto& name:harbortune::built_in_wave_names()){auto w=harbortune::wave_case(name);std::cout<<"  "<<w.name<<" ("<<harbortune::to_string(w.type)<<", amplitude="<<w.amplitude;if(w.type==harbortune::WaveType::Periodic)std::cout<<" m, period="<<w.period<<" s";else std::cout<<" m, center="<<w.pulse_center<<" s, sigma="<<w.pulse_sigma<<" s";std::cout<<")\n";}}
}
int main(int argc,char**argv){try{if(argc==1||std::string(argv[1])=="help"||std::string(argv[1])=="--help"){help();return 0;}std::string c=argv[1];if(c=="--version"){std::cout<<"HarborTune "<<HARBORTUNE_VERSION<<'\n';return 0;}if(c=="list"){list();return 0;}if(c=="simulate")return simulate(parse(argc,argv,2));std::cerr<<"Unknown command: "<<c<<"\n\n";help();return 2;}catch(const std::exception&e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}}
