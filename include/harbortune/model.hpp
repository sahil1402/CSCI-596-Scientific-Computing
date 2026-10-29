#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace harbortune {
enum class EntrancePosition { Left, LeftCenter, Center, RightCenter, Right };
enum class EntranceWidth { Narrow, Medium, Wide };
enum class WaveType { Periodic, Pulse };
struct HarborConfig { EntrancePosition position=EntrancePosition::Center; EntranceWidth width=EntranceWidth::Medium; bool breakwater_enabled=true; };
struct WaveConfig { std::string name="medium"; WaveType type=WaveType::Periodic; double amplitude=0.25; double period=30.0; double pulse_center=15.0; double pulse_sigma=4.0; };
struct SimulationConfig {
    std::size_t nx=120, ny=72; double length_x=1000.0, length_y=600.0;
    double depth=20.0, gravity=9.81, damping=0.0015, cfl=0.34;
    double duration=140.0, measurement_start=20.0; std::size_t snapshot_count=0;
    double breakwater_x_fraction=0.56, protected_x_fraction=0.68;
    std::size_t breakwater_thickness=2;
};
struct SimulationMetrics {
    double peak_abs_elevation=0.0, max_entrance_speed=0.0, mean_protected_energy=0.0;
    double activity_duration=0.0, final_mass_anomaly=0.0, max_abs_elevation_anywhere=0.0;
    double runtime_seconds=0.0, dt=0.0; std::size_t peak_i=0, peak_j=0, steps=0; bool stable=true;
};
struct Snapshot { double time=0.0; std::vector<double> eta,u,v; };
struct SimulationOutput { SimulationMetrics metrics; std::vector<Snapshot> snapshots; };
std::string to_string(EntrancePosition value); std::string to_string(EntranceWidth value); std::string to_string(WaveType value);
EntrancePosition parse_position(const std::string& value); EntranceWidth parse_width(const std::string& value);
WaveConfig wave_case(const std::string& name); std::vector<std::string> built_in_wave_names();
}
