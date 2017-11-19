#include <boost/algorithm/string.hpp>
#include <yaml-cpp/yaml.h>

#include "area.h"
#include "item.h"
#include "mobile.h"
#include "utils.h"
#include "room.h"
#include "resets.h"

using namespace boost::algorithm;
using namespace boost::filesystem;

static std::string input;

const char        *PROTO_FILE = "proto.are";

int main(int argc, char * *argv)
{
  const char *USAGE = "Usage: rom2ranvier <ROM area directory> <output directory>";

  if (argc < 3) {
    std::cerr << USAGE << std::endl;
  }

  std::string        line;
  area_ptr           area;

  path               rom_dir(argv[1]);
  path               ranvier_dir = path(argv[2]) / "areas";
  create_directories(ranvier_dir);

  directory_iterator end_iter;

  if (is_regular(rom_dir)) {
    std::cerr << USAGE << std::endl;
  }

  // Do a first pass to grab just the area information so enough information
  // to do cross-area references is gathered
  for (directory_iterator iter(rom_dir); iter != end_iter; ++iter) {
    auto area_file = iter->path();
    if (!is_regular_file(area_file) || !ends_with(area_file.string(), ".are")) {
      continue;
    }

    std::ifstream in(area_file.c_str());

    if (area_file.string() == PROTO_FILE) {
      continue;
    }

    if (!starts_with(next_line(in), "#AREA")) {
      continue;
    }

    parse_area(in);
  }

  for (directory_iterator iter(rom_dir); iter != end_iter; ++iter) {
    auto fname = iter->path().string();

    if (fname == PROTO_FILE) {
      continue;
    }

    std::ifstream in(fname);

    while (getline(in, line)) {
      if (starts_with(line, "#HELPS")) {
        break;                                        // Don't parse help-only files
      }

      if (starts_with(line, "#AREA")) {
        area = parse_area(in);
      }
      else if (starts_with(line, "#MOBILES")) {
        parse_mobs(in, area);
      }
      else if (starts_with(line, "#OBJECTS")) {
        parse_items(in, area);
      }
      else if (starts_with(line, "#ROOMS")) {
        parse_rooms(in, area);
      }
      else if (starts_with(line, "#RESETS")) {
        parse_resets(in);
      }
      else if (starts_with(line, "#SHOPS")) {
        parse_shops(in);
      }
    }
  }

  for (const auto& area: areas) {
    write_manifest(ranvier_dir, area);
    write_mobs(ranvier_dir, area);
    write_items(ranvier_dir, area);
    write_rooms(ranvier_dir, area);
  }

  return 0;
}