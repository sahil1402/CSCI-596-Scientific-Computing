#include "harbortune/experiment.hpp"
#include "harbortune/geometry.hpp"
#include "harbortune/solver.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
namespace { void require(bool value,const char* message){if(!value)throw std::runtime_error(message);} }
int main(){try{
 require(harbortune::parse_position("left-center")==harbortune::EntrancePosition::LeftCenter,"position parser failed");
 require(harbortune::parse_width("wide")==harbortune::EntranceWidth::Wide,"width parser failed");
 harbortune::SimulationConfig config;config.nx=40;config.ny=26;config.duration=45;config.measurement_start=8;
 harbortune::HarborConfig harbor;harbortune::Geometry geometry(config,harbor);require(geometry.protected_cell_count()>0,"protected region is empty");require(geometry.gap_end_j()>geometry.gap_begin_j(),"entrance gap is empty");
 harbortune::Simulator simulator(config,harbor);auto result=simulator.run(harbortune::wave_case("pulse"));require(result.metrics.stable,"solver became unstable");require(std::isfinite(result.metrics.peak_abs_elevation),"non-finite elevation metric");require(result.metrics.max_abs_elevation_anywhere>0,"wave forcing produced no response");
 auto sequential=harbortune::run_sweep(config,1,false);auto parallel=harbortune::run_sweep(config,2,true);require(sequential.records.size()==60,"sweep does not contain 60 cases");require(sequential.rankings.size()==15,"ranking does not contain 15 designs");for(std::size_t i=0;i<sequential.records.size();++i)require(std::abs(sequential.records[i].metrics.peak_abs_elevation-parallel.records[i].metrics.peak_abs_elevation)<1e-13,"parallel result differs from sequential result");
 std::cout<<"All HarborTune core tests passed.\n";return 0;
 }catch(const std::exception& e){std::cerr<<"Test failure: "<<e.what()<<'\n';return 1;}}
