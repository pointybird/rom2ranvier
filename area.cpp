#include <regex>
#include <stdexcept>

#include <yaml-cpp/yaml.h>
#include <boost/algorithm/string.hpp>

#include "area.h"
#include "utils.h"

using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace YAML;

std::list<area_ptr> areas;

static std::string  input;

void write_manifest(const path& dir, area_ptr area)
{
  Emitter yaml;

  yaml << BeginMap;

  yaml << Key << "title" << Value << area->name;

  yaml << Key << "info";
  yaml << Value << BeginMap;
  yaml << Key << "builder" << Value << area->builder;
  yaml << Key << "levelRange" << Value << area->level_range;
  yaml << Key << "respawnInterval" << Value << 15;
  yaml << EndMap;

  yaml << EndMap;

  path          area_dir = dir / area->key;
  create_directories(area_dir);

  path          manifest = area_dir / "manifest.yml";
  std::ofstream out(manifest.c_str());
  out << yaml.c_str();
  out.close();
}

area_ptr parse_area(std::ifstream& in)
{
  auto               area = std::make_shared<struct area>();

  std::istringstream iss;

  auto               fname     = get_mud_string(in);
  path               file_path = path(fname);
//  file_
//
//  // If area file is myarea->are, create directory output_dir/myarea
//  dir = path(output_dir) / file_path.stem();
//  if (!exists(dir)) {
//    if (!create_directory(dir)) {
//      std::cerr << "Failed to create directory " << dir << std::endl;
//      exit(1);
//    }
//  }

  area->name = get_mud_string(in);
  area->key  = change_extension(fname, "").string();

  auto        line = get_mud_string(in);

  std::smatch match;
  std::regex  re("\\s*\\{\\s*(.*)\\s*\\}\\s*(\\w+)\\s+(.*)");

  if (std::regex_search(line, match, re) && match.size() > 2) {
    area->level_range = match.str(1);
    area->builder     = match.str(2);
  }
  else {
    std::cerr << "uhoh: " << line << std::endl;
    in.ignore();
  }

  in >> area->bottom >> area->top;

  save_area(area);

  return area;
}

void save_area(area_ptr area_to_save)
{
  areas.push_back(area_to_save);

  // Keep list sorted by first vnum in area
  areas.sort([](const area_ptr& a, const area_ptr& b) {
    return a->bottom > b->bottom;
  });
}

int get_ranvier_id(int vnum)
{
  auto iter = std::find_if(areas.begin(), areas.end(),
                           [&](const area_ptr& a) {
    return a->bottom <= vnum && a->top >= vnum;
  });

  if (iter != areas.end()) {
    return vnum - (*iter)->bottom;
  }

  throw std::invalid_argument("Failed to find entity id for vnum");
}

// Given a vnum, create a Ranvier id.
// Example: vnum #3000 in Midgaard will be midgaard:0, #3001 will be midgaard:1, etc.
std::string get_ranvier_entity_id(int vnum)
{
  area_ptr area;

  auto     iter = std::find_if(areas.begin(), areas.end(),
                               [&](const area_ptr& a) {
    return a->bottom <= vnum && a->top >= vnum;
  });

  if (iter != areas.end()) {
    return (*iter)->key + ':' + std::to_string(vnum - (*iter)->bottom);
  }

  throw std::invalid_argument("Failed to find entity id for vnum");
}