#include "harbortune/geometry.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace harbortune { namespace {
double pos(EntrancePosition p){switch(p){case EntrancePosition::Left:return .18;case EntrancePosition::LeftCenter:return .34;case EntrancePosition::Center:return .5;case EntrancePosition::RightCenter:return .66;case EntrancePosition::Right:return .82;}return .5;}
double width(EntranceWidth w){switch(w){case EntranceWidth::Narrow:return .10;case EntranceWidth::Medium:return .17;case EntranceWidth::Wide:return .25;}return .17;}
}
Geometry::Geometry(const SimulationConfig& s,const HarborConfig& h):nx_(s.nx),ny_(s.ny),solid_(nx_*ny_),protected_(nx_*ny_),entrance_(nx_*ny_){
 if(nx_<24||ny_<20)throw std::invalid_argument("grid must be at least 24 by 20 cells");
 breakwater_i_=static_cast<std::size_t>(std::llround(s.breakwater_x_fraction*(nx_-1)));
 auto cells=std::max<std::size_t>(3,static_cast<std::size_t>(std::llround(width(h.width)*ny_)));
 auto left_begin=[&](double fraction){long value=std::lround(fraction*(ny_-1)-.5*(cells-1));return static_cast<std::size_t>(std::clamp<long>(value,1,static_cast<long>(ny_-cells-1)));};
 if(h.position==EntrancePosition::Center)gap_begin_j_=(ny_-cells)/2;
 else if(h.position==EntrancePosition::Left)gap_begin_j_=left_begin(pos(EntrancePosition::Left));
 else if(h.position==EntrancePosition::LeftCenter)gap_begin_j_=left_begin(pos(EntrancePosition::LeftCenter));
 else if(h.position==EntrancePosition::RightCenter)gap_begin_j_=ny_-left_begin(pos(EntrancePosition::LeftCenter))-cells;
 else gap_begin_j_=ny_-left_begin(pos(EntrancePosition::Left))-cells;
 gap_end_j_=gap_begin_j_+cells-1;
 if(h.breakwater_enabled){auto half=s.breakwater_thickness/2;auto begin=breakwater_i_>half?breakwater_i_-half:1;auto end=std::min(nx_-2,begin+std::max<std::size_t>(1,s.breakwater_thickness)-1);for(std::size_t j=0;j<ny_;++j)if(j<gap_begin_j_||j>gap_end_j_)for(std::size_t i=begin;i<=end;++i)solid_[index(i,j)]=1;}
 auto pb=static_cast<std::size_t>(std::llround(s.protected_x_fraction*(nx_-1)));for(std::size_t j=1;j+1<ny_;++j)for(std::size_t i=pb;i+1<nx_;++i)if(!solid(i,j)){protected_[index(i,j)]=1;++protected_cell_count_;}
 auto ib=breakwater_i_>2?breakwater_i_-2:0,ie=std::min(nx_-1,breakwater_i_+2);for(std::size_t j=gap_begin_j_;j<=gap_end_j_;++j)for(std::size_t i=ib;i<=ie;++i)if(!solid(i,j))entrance_[index(i,j)]=1;
}
}
