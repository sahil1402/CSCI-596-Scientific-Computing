#include "harbortune/experiment.hpp"
#include "harbortune/io.hpp"
#include "harbortune/solver.hpp"
#include "harbortune/verification.hpp"
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
void help(){std::cout<<"HarborTune - parallel screening of breakwater entrances\n\nUsage:\n  harbortune simulate [options]\n  harbortune sweep [options]\n  harbortune verify [--output PATH]\n  harbortune list\n  harbortune --version\n\nCommon options:\n  --nx N --ny N       grid dimensions (default: 120 x 72)\n  --duration SECONDS  simulated duration (default: 140)\n  --output PATH       output directory\n\nsimulate options:\n  --position NAME     left, left-center, center, right-center, right\n  --width NAME        narrow, medium, wide\n  --wave NAME         short, medium, long, pulse\n  --snapshots N       exported frames (default: 40)\n\nsweep options:\n  --threads N         OpenMP threads (default: 1)\n  --mode MODE         sequential or parallel (default: parallel)\n";}
Options parse(int argc,char**argv,int begin){Options o;for(int i=begin;i<argc;++i){std::string k=argv[i];if(k.rfind("--",0)!=0)throw std::invalid_argument("expected an option, received: "+k);if(i+1>=argc)throw std::invalid_argument("missing value for "+k);o[k.substr(2)]=argv[++i];}return o;}
std::string opt(const Options&o,const std::string&k,const std::string&d){auto it=o.find(k);return it==o.end()?d:it->second;}
std::size_t size_opt(const Options&o,const std::string&k,std::size_t d){auto it=o.find(k);return it==o.end()?d:static_cast<std::size_t>(std::stoull(it->second));}
double real_opt(const Options&o,const std::string&k,double d){auto it=o.find(k);return it==o.end()?d:std::stod(it->second);}
harbortune::SimulationConfig simulation_options(const Options&o){harbortune::SimulationConfig s;s.nx=size_opt(o,"nx",s.nx);s.ny=size_opt(o,"ny",s.ny);s.duration=real_opt(o,"duration",s.duration);s.measurement_start=real_opt(o,"measurement-start",std::min(20.0,.2*s.duration));return s;}

int simulate(const Options&o){auto s=simulation_options(o);s.snapshot_count=size_opt(o,"snapshots",40);harbortune::HarborConfig h;h.position=harbortune::parse_position(opt(o,"position","center"));h.width=harbortune::parse_width(opt(o,"width","medium"));auto w=harbortune::wave_case(opt(o,"wave","pulse"));std::filesystem::path dir=opt(o,"output","results/data/simulation");std::cout<<"Running "<<harbortune::to_string(h.position)<<'/'<<harbortune::to_string(h.width)<<" with "<<w.name<<" forcing on a "<<s.nx<<'x'<<s.ny<<" grid...\n";harbortune::Simulator simulator(s,h);auto output=simulator.run(w);harbortune::write_simulation_output(dir,s,h,w,simulator.geometry(),output);auto&m=output.metrics;std::cout<<"Stable: "<<(m.stable?"yes":"no")<<"\nPeak protected elevation: "<<m.peak_abs_elevation<<" m\nMaximum entrance speed: "<<m.max_entrance_speed<<" m/s\nMean protected energy: "<<m.mean_protected_energy<<" J/m^2\nRuntime: "<<m.runtime_seconds<<" s\nOutput: "<<std::filesystem::absolute(dir)<<'\n';return m.stable?0:1;}

int sweep(const Options&o){auto s=simulation_options(o);int threads=static_cast<int>(size_opt(o,"threads",1));bool parallel=opt(o,"mode","parallel")!="sequential";std::filesystem::path dir=opt(o,"output","results/data/sweep");std::cout<<"Running 60-case "<<(parallel?"parallel":"sequential")<<" sweep on a "<<s.nx<<'x'<<s.ny<<" grid with "<<threads<<" requested thread(s)...\n";auto output=harbortune::run_sweep(s,threads,parallel);harbortune::write_sweep_output(dir,output);auto&best=output.rankings.front();std::cout<<"Completed in "<<output.runtime_seconds<<" s using "<<output.actual_threads<<" thread(s).\nBest robust design: "<<harbortune::to_string(best.position)<<'/'<<harbortune::to_string(best.width)<<" (score "<<best.robust_score<<")\nOutput: "<<std::filesystem::absolute(dir)<<'\n';return 0;}

void list(){std::cout<<"Entrance positions: left, left-center, center, right-center, right\nEntrance widths: narrow, medium, wide\nWave cases:\n";for(auto& name:harbortune::built_in_wave_names()){auto w=harbortune::wave_case(name);std::cout<<"  "<<w.name<<" ("<<harbortune::to_string(w.type)<<", amplitude="<<w.amplitude;if(w.type==harbortune::WaveType::Periodic)std::cout<<" m, period="<<w.period<<" s";else std::cout<<" m, center="<<w.pulse_center<<" s, sigma="<<w.pulse_sigma<<" s";std::cout<<")\n";}}
int verify(const Options&o){std::filesystem::path dir=opt(o,"output","results/data/verification");std::cout<<"Running numerical verification suite...\n";auto result=harbortune::run_verification(dir);std::cout<<result.passed<<" checks passed; "<<result.failed<<" checks failed.\nOutput: "<<std::filesystem::absolute(dir)<<'\n';return result.failed==0?0:1;}
}

int main(int argc,char**argv){try{if(argc==1||std::string(argv[1])=="help"||std::string(argv[1])=="--help"){help();return 0;}std::string c=argv[1];if(c=="--version"){std::cout<<"HarborTune "<<HARBORTUNE_VERSION<<'\n';return 0;}if(c=="list"){list();return 0;}if(c=="simulate")return simulate(parse(argc,argv,2));if(c=="sweep")return sweep(parse(argc,argv,2));if(c=="verify")return verify(parse(argc,argv,2));std::cerr<<"Unknown command: "<<c<<"\n\n";help();return 2;}catch(const std::exception&e){std::cerr<<"Error: "<<e.what()<<'\n';return 1;}}
