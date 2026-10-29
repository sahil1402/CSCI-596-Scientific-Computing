#pragma once
#include "harbortune/model.hpp"
#include <cstdint>
#include <vector>
namespace harbortune {
class Geometry {
public:
    Geometry(const SimulationConfig&, const HarborConfig&);
    std::size_t index(std::size_t i,std::size_t j) const { return j*nx_+i; }
    bool solid(std::size_t i,std::size_t j) const { return solid_[index(i,j)]!=0; }
    bool protected_cell(std::size_t i,std::size_t j) const { return protected_[index(i,j)]!=0; }
    bool entrance_cell(std::size_t i,std::size_t j) const { return entrance_[index(i,j)]!=0; }
    const std::vector<std::uint8_t>& solid_mask() const { return solid_; }
    std::size_t nx() const{return nx_;} std::size_t ny() const{return ny_;}
    std::size_t breakwater_i() const{return breakwater_i_;} std::size_t gap_begin_j() const{return gap_begin_j_;}
    std::size_t gap_end_j() const{return gap_end_j_;} std::size_t protected_cell_count() const{return protected_cell_count_;}
private:
    std::size_t nx_,ny_,breakwater_i_=0,gap_begin_j_=0,gap_end_j_=0,protected_cell_count_=0;
    std::vector<std::uint8_t> solid_,protected_,entrance_;
};
}
