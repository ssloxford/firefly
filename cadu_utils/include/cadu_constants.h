#include <map>

// TODO: should be constexpr, but there's no constexpr constructor for std::map
std::map<std::string, int> SCIDs {
  {"terra", 42},
  {"aqua", 154}
};

// TODO: should be constexpr, but there's no constexpr constructor for std::map
std::map<std::string, int> VCIDs {
  {"aqua_gbad", 3},
  {"aqua_ceres_10", 10},
  {"aqua_ceres_15", 15},
  {"aqua_amsu_20", 20},
  {"aqua_amsu_25", 25},
  {"aqua_modis", 30},
  {"aqua_airs", 35},
  {"aqua_amsr", 40},
  {"aqua_hsb", 45}
};
