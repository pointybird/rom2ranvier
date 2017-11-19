#include <boost/algorithm/string.hpp>
#include <yaml-cpp/yaml.h>

#include "utils.h"
#include "area.h"
#include "item.h"
#include "mobile.h"
#include "flags.h"

using namespace YAML;
using namespace boost::filesystem;

std::map<int, mobile_ptr> mob_lookup;

void write_mobs(path& dir, area_ptr area)
{
  Emitter yaml;

  yaml << BeginSeq;

  for (auto mob : area->mobs) {
    yaml << BeginMap;

    yaml << Key << "id" << Value << std::to_string(mob->id);
    yaml << Key << "keywords" << Value << Flow << to_string_array(mob->keywords);
    yaml << Key << "name" << Value << mob->short_desc;
    yaml << Key << "roomDescription" << Value << boost::trim_right_copy_if(mob->long_desc, boost::is_any_of("\n"));
    yaml << Key << "description" << Value << boost::trim_right_copy_if(mob->description, boost::is_any_of("\n"));
    yaml << Key << "level" << Value << mob->level;

    // Items
    if (!mob->vendor && !mob->items.empty()) {
      std::vector<std::string> items;
      for (const auto& vnum : mob->items) {
        items.push_back(get_ranvier_entity_id(vnum));
      }

      yaml << Key << "items" << Flow << Value << items;
    }

    // Equipment
    if (!mob->equipment.empty()) {
      yaml << Key << "equipment" << BeginMap;
      for (const auto& eq : mob->equipment) {
        yaml << Key << eq.first << Value << BeginMap;
        yaml << Key << "entityReference" << Value << eq.second;
        yaml << EndMap;
      }

      yaml << EndMap;
    }

    auto actFlags = parse_act_flags(mob);

    // Behaviors
    yaml << Key << "behaviors" << Value << BeginMap;
    // Have every mob create a corpse when killed
    yaml << Key << "lootable" << Value << BeginMap;
    if (mob->gold) {
      yaml << Key << "currencies" << Value << BeginMap;
      yaml << Key << "gold" << Value << BeginMap;
      yaml << Key << "min" << Value << mob->gold;
      yaml << Key << "max" << Value << mob->gold;
      yaml << EndMap;
      yaml << EndMap;
    }
    yaml << EndMap;
    // Give every mob ability to fight
    yaml << Key << "combat" << Value << true;
    if (mob->aggressive) {
      yaml << Key << "ranvier-aggro" << Value << BeginMap;
      yaml << Key << "delay" << Value << 5;
      yaml << EndMap;
    }
    // Let mobs wander
    if (!mob->sentinel) {
      yaml << Key << "ranvier-wander" << Value << BeginMap;
      yaml << Key << "areaRestricted" << Value << mob->stays_in_area;
      yaml << Key << "interval" << Value << 20;
      yaml << EndMap;
    }
    if (mob->vendor) {
      yaml << Key << "vendor" << Value << BeginMap;
      yaml << Key << "items" << Value << BeginMap;
      for (const auto& vnum : mob->items) {
        if (item_lookup.find(vnum) != item_lookup.end()) {
          auto item = item_lookup[vnum];
          yaml << Key << get_ranvier_entity_id(vnum) << Value << BeginMap;
          yaml << Key << "cost" << Value << item->metadata["cost"];
          yaml << Key << "currency" << Value << "gold";
          yaml << EndMap;
        }
      }

      yaml << EndMap;
      yaml << EndMap;
    }
    yaml << EndMap;

    // Metadata
    if (!mob->metadata.empty()) {
      yaml << Key << "metadata" << Value << BeginMap;

      for (const auto& meta : mob->metadata) {
        auto key   = meta.first;
        auto value = meta.second;
        if (value.empty()) {
          continue;
        }

        yaml << Key << key;
        if (key == "actions") {
          yaml << Flow << Value << actFlags;
        }
        else if (key == "affected") {
          yaml << Flow << Value << parse_affected_flags(value);
        }
        else if (key == "offense") {
          yaml << Flow << Value << parse_off_flags(value);
        }
        else if (key == "immunity" || key == "resistance" || key == "vulnerability") {
          yaml << Flow << Value << parse_resistance_flags(value);
        }
        else {
          yaml << Value << value;
        }
      }

      yaml << EndMap;
    }

    yaml << EndMap;
  }

  yaml << EndSeq;

  path          area_dir  = dir / area->key;
  auto          npcs_file = area_dir / "npcs.yml";
  std::ofstream out(npcs_file.c_str());
  out << yaml.c_str();
  out.close();
}

static mobile_ptr read_mobile(const std::string& first_line, std::ifstream& in)
{
  auto               mob = std::make_shared<mobile>();
  char               c;

  std::istringstream iss;

  iss.clear();
  iss.str(first_line);
  iss >> c >> mob->vnum;

  mob->id = get_ranvier_id(mob->vnum);

  mob->keywords   = get_mud_string(in);
  mob->short_desc = get_mud_string(in);
  mob->long_desc  = get_mud_string(in);
  if (!mob->long_desc.empty()) {
    mob->long_desc[0] = toupper(mob->long_desc[0]);
  }
  mob->description = get_mud_string(in);
  if (!mob->description.empty()) {
    mob->description[0] = toupper(mob->description[0]);
  }

  mob->metadata["race"] = get_mud_string(in);

  iss.clear();
  iss.str(next_line(in));

  int alignment, group;
  iss >> mob->metadata["actions"] >> mob->metadata["affected"] >> alignment >> group;
  mob->metadata["alignment"] = std::to_string(alignment);
  mob->metadata["group"]     = std::to_string(group);

  iss.clear();
  iss.str(next_line(in));

  iss >> mob->level >> mob->metadata["hitroll"];
  iss >> mob->metadata["hitDice"];
  iss >> mob->metadata["manaDice"];
  iss >> mob->metadata["damDice"];
  iss >> mob->metadata["damType"];

  iss.clear();
  iss.str(next_line(in));

  int ac[4] = { 0 };
  iss >> ac[0] >> ac[1] >> ac[2] >> ac[3];
  mob->metadata["acPierce"] = std::to_string(10 * ac[0]);
  mob->metadata["acBash"]   = std::to_string(10 * ac[1]);
  mob->metadata["acSlash"]  = std::to_string(10 * ac[2]);
  mob->metadata["acExotic"] = std::to_string(10 * ac[3]);

  iss.clear();
  iss.str(next_line(in));
  iss >> mob->metadata["offense"] >> mob->metadata["immunity"] >> mob->metadata["resistance"] >> mob->metadata["vulnerability"];

  iss.clear();
  iss.str(next_line(in));
  iss >> mob->metadata["startPosition"] >> mob->metadata["defaultPosition"] >> mob->metadata["sex"] >> mob->gold;

  iss.clear();
  iss.str(next_line(in));
  iss >> mob->metadata["form"] >> mob->metadata["parts"] >> mob->metadata["size"] >> mob->metadata["material"];

  // Skip to end of mobile
  char flag;
  while ((c = in.peek()) != '#') {
    switch (c) {
      case 'F':
      {
        std::string type;
        in >> c >> type >> flag;
        next_line(in);
      }
      break;

      case '#':
        break;

      default:
        break;
    }
  }

  mob_lookup[mob->vnum] = mob;
  return mob;
}

void parse_mobs(std::ifstream& in, area_ptr area)
{
  std::list<mobile> mobs;
  std::string       line;

  while (std::getline(in, line)) {
    if (line == "#0") {
      break;
    }

    try {
      area->mobs.push_back(read_mobile(line, in));
    }
    catch (std::exception e) {
      std::cerr << "failed to load mob: " << e.what() << std::endl;
    }
  }
}

void parse_shops(std::ifstream& in)
{
  int         vnum;
  char        c;

  std::string line;

  while ((c = in.peek()) != '0') {
    in >> vnum;

    if (mob_lookup.find(vnum) != mob_lookup.end()) {
      mob_lookup[vnum]->vendor = true;
    }

    std::getline(in, line);
  }
}