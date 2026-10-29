#include "harbortune/solver.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <utility>
namespace harbortune { namespace {
constexpr double pi=3.14159265358979323846;
struct State{double eta=0,u=0,v=0;};
State add(State a,State b){return {a.eta+b.eta,a.u+b.u,a.v+b.v};} State sub(State a,State b){return {a.eta-b.eta,a.u-b.u,a.v-b.v};} State mul(double x,State a){return {x*a.eta,x*a.u,x*a.v};}
State fx(State q,double H,double g){return {H*q.u,g*q.eta,0};} State fy(State q,double H,double g){return {H*q.v,0,g*q.eta};}
State rfx(State l,State r,double H,double g,double c){return sub(mul(.5,add(fx(l,H,g),fx(r,H,g))),mul(.5*c,sub(r,l)));}
State rfy(State l,State r,double H,double g,double c){return sub(mul(.5,add(fy(l,H,g),fy(r,H,g))),mul(.5*c,sub(r,l)));}
double forcing(const WaveConfig& w,double t){if(w.type==WaveType::Pulse){double z=(t-w.pulse_center)/w.pulse_sigma;return w.amplitude*std::exp(-.5*z*z);}return w.amplitude*std::min(1.0,t/w.period)*std::sin(2*pi*t/w.period);}
State left_boundary(State q,const WaveConfig& w,double t,double H,double c){double ep=forcing(w,t),wp=2*c*ep/H,wm=q.u-c*q.eta/H;return {H*(wp-wm)/(2*c),.5*(wp+wm),q.v};}
bool finite(State q){return std::isfinite(q.eta)&&std::isfinite(q.u)&&std::isfinite(q.v);}
}
Simulator::Simulator(SimulationConfig s,HarborConfig h):simulation_(std::move(s)),harbor_(h),geometry_(simulation_,harbor_){if(simulation_.length_x<=0||simulation_.length_y<=0||simulation_.depth<=0||simulation_.gravity<=0||simulation_.duration<=0)throw std::invalid_argument("physical dimensions and duration must be positive");if(simulation_.cfl<=0||simulation_.cfl>.49)throw std::invalid_argument("CFL factor must be in (0, 0.49]");}
SimulationOutput Simulator::run(const WaveConfig& wave) const{
 auto start=std::chrono::steady_clock::now();auto n=simulation_.nx*simulation_.ny;double dx=simulation_.length_x/(simulation_.nx-1),dy=simulation_.length_y/(simulation_.ny-1),c=std::sqrt(simulation_.gravity*simulation_.depth);double stable_dt=simulation_.cfl/(c*(1/dx+1/dy));auto steps=static_cast<std::size_t>(std::ceil(simulation_.duration/stable_dt));double dt=simulation_.duration/steps;
 std::vector<State> q(n),qn(n);SimulationOutput out;out.metrics.steps=steps;out.metrics.dt=dt;auto interval=simulation_.snapshot_count>1?std::max<std::size_t>(1,steps/(simulation_.snapshot_count-1)):steps+1;
 auto snapshot=[&](double t){Snapshot s;s.time=t;s.eta.resize(n);s.u.resize(n);s.v.resize(n);for(std::size_t k=0;k<n;++k){s.eta[k]=q[k].eta;s.u[k]=q[k].u;s.v[k]=q[k].v;}out.snapshots.push_back(std::move(s));};if(simulation_.snapshot_count)snapshot(0);
 double energy_sum=0;std::size_t samples=0;double threshold=.05*wave.amplitude;
 for(std::size_t step=0;step<steps;++step){double t=step*dt;
  for(std::size_t j=0;j<simulation_.ny;++j)for(std::size_t i=0;i<simulation_.nx;++i){auto k=geometry_.index(i,j);if(geometry_.solid(i,j)){qn[k]={};continue;}State center=q[k];
   auto nxstate=[&](long ni){if(ni<0)return left_boundary(center,wave,t,simulation_.depth,c);if(ni>=static_cast<long>(simulation_.nx))return State{center.eta,-center.u,center.v};auto ii=static_cast<std::size_t>(ni);if(geometry_.solid(ii,j))return State{center.eta,-center.u,center.v};return q[geometry_.index(ii,j)];};
   auto nystate=[&](long nj){if(nj<0||nj>=static_cast<long>(simulation_.ny))return State{center.eta,center.u,-center.v};auto jj=static_cast<std::size_t>(nj);if(geometry_.solid(i,jj))return State{center.eta,center.u,-center.v};return q[geometry_.index(i,jj)];};
   State fl=rfx(nxstate(static_cast<long>(i)-1),center,simulation_.depth,simulation_.gravity,c),fr=rfx(center,nxstate(static_cast<long>(i)+1),simulation_.depth,simulation_.gravity,c),fb=rfy(nystate(static_cast<long>(j)-1),center,simulation_.depth,simulation_.gravity,c),ft=rfy(center,nystate(static_cast<long>(j)+1),simulation_.depth,simulation_.gravity,c);
   State z=sub(sub(center,mul(dt/dx,sub(fr,fl))),mul(dt/dy,sub(ft,fb)));double decay=std::max(0.0,1-simulation_.damping*dt);z.u*=decay;z.v*=decay;qn[k]=z;
  }
  q.swap(qn);double now=(step+1)*dt;if(((step+1)%interval==0||step+1==steps)&&simulation_.snapshot_count>out.snapshots.size())snapshot(now);
  double e=0,step_peak=0,mass=0;for(std::size_t j=0;j<simulation_.ny;++j)for(std::size_t i=0;i<simulation_.nx;++i){auto k=geometry_.index(i,j);if(geometry_.solid(i,j))continue;auto z=q[k];if(!finite(z)||std::abs(z.eta)>100*wave.amplitude)out.metrics.stable=false;mass+=z.eta*dx*dy;out.metrics.max_abs_elevation_anywhere=std::max(out.metrics.max_abs_elevation_anywhere,std::abs(z.eta));if(geometry_.entrance_cell(i,j))out.metrics.max_entrance_speed=std::max(out.metrics.max_entrance_speed,std::hypot(z.u,z.v));if(geometry_.protected_cell(i,j)&&now>=simulation_.measurement_start){double a=std::abs(z.eta);step_peak=std::max(step_peak,a);if(a>out.metrics.peak_abs_elevation){out.metrics.peak_abs_elevation=a;out.metrics.peak_i=i;out.metrics.peak_j=j;}e+=.5*simulation_.gravity*z.eta*z.eta+.5*simulation_.depth*(z.u*z.u+z.v*z.v);}}
  out.metrics.final_mass_anomaly=mass;if(now>=simulation_.measurement_start&&geometry_.protected_cell_count()){energy_sum+=e/geometry_.protected_cell_count();++samples;if(step_peak>=threshold)out.metrics.activity_duration+=dt;}if(!out.metrics.stable)break;
 }
 if(samples)out.metrics.mean_protected_energy=energy_sum/samples;out.metrics.runtime_seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();return out;
}
}
