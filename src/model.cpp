#include "harbortune/model.hpp"
#include <algorithm>
#include <cctype>
#include <stdexcept>
namespace harbortune { namespace {
std::string lower(std::string s){std::transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});return s;}
}
std::string to_string(EntrancePosition p){switch(p){case EntrancePosition::Left:return "left";case EntrancePosition::LeftCenter:return "left-center";case EntrancePosition::Center:return "center";case EntrancePosition::RightCenter:return "right-center";case EntrancePosition::Right:return "right";}throw std::logic_error("invalid position");}
std::string to_string(EntranceWidth w){switch(w){case EntranceWidth::Narrow:return "narrow";case EntranceWidth::Medium:return "medium";case EntranceWidth::Wide:return "wide";}throw std::logic_error("invalid width");}
std::string to_string(WaveType t){return t==WaveType::Periodic?"periodic":"pulse";}
EntrancePosition parse_position(const std::string& s){auto v=lower(s);if(v=="left")return EntrancePosition::Left;if(v=="left-center"||v=="left_center")return EntrancePosition::LeftCenter;if(v=="center"||v=="centre")return EntrancePosition::Center;if(v=="right-center"||v=="right_center")return EntrancePosition::RightCenter;if(v=="right")return EntrancePosition::Right;throw std::invalid_argument("unknown entrance position: "+s);}
EntranceWidth parse_width(const std::string& s){auto v=lower(s);if(v=="narrow")return EntranceWidth::Narrow;if(v=="medium")return EntranceWidth::Medium;if(v=="wide")return EntranceWidth::Wide;throw std::invalid_argument("unknown entrance width: "+s);}
WaveConfig wave_case(const std::string& s){auto v=lower(s);if(v=="short")return {"short",WaveType::Periodic,0.25,20,15,4};if(v=="medium")return {"medium",WaveType::Periodic,0.25,30,15,4};if(v=="long")return {"long",WaveType::Periodic,0.25,45,15,4};if(v=="pulse"||v=="tsunami")return {"pulse",WaveType::Pulse,0.35,30,16,4.5};throw std::invalid_argument("unknown wave case: "+s);}
std::vector<std::string> built_in_wave_names(){return {"short","medium","long","pulse"};}
}
