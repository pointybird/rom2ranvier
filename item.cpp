#include <regex>

#include <boost/algorithm/string.hpp>
#include <yaml-cpp/yaml.h>

#include "area.h"
#include "item.h"
#include "utils.h"
#include "flags.h"

using namespace boost::filesystem;
using namespace YAML;

std::map<int, item_ptr>            item_lookup;

static std::map<char, std::string> condition_lookup = {
  { 'P', "100" },
  { 'G', "90" },
  { 'A', "75" },
  { 'W', "50" },
  { 'D', "25" },
  { 'B', "10" },
  { 'R', "0" }
};

std::map<int, std::string>         affects = {
  { 1, "strength" },
  { 2, "dexterity" },
  { 3, "intelligence" },
  { 4, "wisdom" },
  { 5, "constitution" },
  { 6, "sex" },
  { 7, "class" },
  { 8, "level" },
  { 9, "age" },
  { 10, "height" },
  { 11, "weight" },
  { 12, "mana" },
  { 13, "health" },
  { 14, "moves" },
  { 15, "gold" },
  { 16, "exp" },
  { 17, "armor" },
  { 18, "hitroll" },
  { 19, "damroll" },
  { 20, "saveParalysis" },
  { 21, "saveRods" },
  { 22, "savePetrification" },
  { 23, "saveBreath" },
  { 24, "saveSpell" },
  { 25, "saveAffect" }
};

void write_items(path& dir, area_ptr area)
{
  Emitter yaml;

  yaml << BeginSeq;

  for (auto item : area->items) {
    yaml << BeginMap;

    yaml << Key << "id" << Value << std::to_string(item->id);
    yaml << Key << "keywords" << Value << Flow << to_string_array(item->keywords);
    yaml << Key << "name" << Value << item->short_desc;
    yaml << Key << "roomDescription" << Value << item->long_desc;
    yaml << Key << "type" << Value << boost::to_upper_copy(item->type);

    auto worn = parse_wear_flags(item);

    if (item->type == "container") {
      yaml << Key << "closeable" << Value << item->closeable;
      yaml << Key << "closed" << Value << item->closed;
      yaml << Key << "locked" << Value << item->locked;
      if (!item->key_id.empty()) {
        yaml << Key << "lockedBy" << Value << item->key_id;
      }
    }

    if (!item->contents.empty()) {
      yaml << Key << "items" << Flow << Value << item->contents;
    }

    // Extra descs
    if (!item->extra_descs.empty()) {
      yaml << Key << "extraDescriptions" << Value << BeginMap;
      for (const auto& exdesc : item->extra_descs) {
        yaml << Key << exdesc.first << Value << exdesc.second;
      }

      yaml << EndMap;
    }

    // Metadata
    if (!item->metadata.empty() || !item->stats.empty()) {
      yaml << Key << "metadata" << Value << BeginMap;

      for (const auto& meta : item->metadata) {
        auto key   = meta.first;
        auto value = meta.second;
        if (value.empty()) {
          continue;
        }

        yaml << Key << key;
        if (key == "slot") {
          yaml << Flow << Value << worn;
        }
        else if (key == "weaponFlags") {
          yaml << Flow << Value << parse_weapon_flags(value);
        }
        else if (key == "extraFlags") {
          yaml << Flow << Value << parse_extra_flags(value);
        }
        else {
          yaml << Value << value;
        }
      }

      if (!item->stats.empty()) {
        yaml << Key << "stats" << BeginMap;
        for (const auto& stat : item->stats) {
          yaml << Key << stat.first << Value << stat.second;
        }

        yaml << EndMap;
      }

      yaml << EndMap;
    }

    yaml << EndMap;
  }

  yaml << EndSeq;

  path          area_dir   = dir / area->key;
  auto          items_file = area_dir / "items.yml";
  std::ofstream out(items_file.c_str());
  out << yaml.c_str();
  out.close();
}

void item::parse_type_specific_data(const std::string& line)
{
  std::istringstream iss(line);
  int                num;

  if (this->type == "weapon") {
    iss >> metadata["weaponType"];
    iss >> metadata["weaponDiceNum"];
    iss >> metadata["weaponDiceSize"];
    iss >> metadata["damageType"];
    iss >> metadata["weaponFlags"];
  }
  else if (this->type == "container") {
    std::string containerFlags;
    int         key;

    iss >> metadata["capacity"];
    iss >> containerFlags;
    iss >> key;
    iss >> metadata["maxWeight"];
    iss >> metadata["weightMultiplier"];

    if (key > 0) {
      this->key_id = get_ranvier_entity_id(key);
    }
    parse_container_flags(*this, containerFlags);
  }
  else if (this->type == "drink" || this->type == "fountain") {
    std::string type;
    int         poisoned;

    iss >> metadata["capacity"];
    iss >> metadata["filled"];
    iss >> type;
    metadata["liquidType"] = boost::erase_all_copy(type, "'");
    iss >> poisoned >> num;
    if (poisoned) {
      metadata["poisoned"] = "true";
    }
  }
  else if (this->type == "wand" || this->type == "staff") {
    std::string spell;

    iss >> metadata["spellLevel"];
    iss >> metadata["charges"];
    iss >> metadata["maxCharges"];
    iss >> spell;
    while (!boost::ends_with(spell, "'")) {
      // handle multi-word spells like magic missile
      spell.append(std::string(1, iss.get()));
    }

    metadata["spell"] = boost::erase_all_copy(spell, "'");
  }
  else if (this->type == "armor") {
    iss >> metadata["acPierce"];
    iss >> metadata["acBase"];
    iss >> metadata["acSlash"];
    iss >> metadata["acMagic"];
  }
  else if (this->type == "potion" || this->type == "pill" || this->type == "scroll") {
    std::regex  re("(\\d+)\\s+'(.*)'\\s+'(.*)'\\s+'(.*)'\\s+'(.*)'");
    std::smatch match;

    if (std::regex_search(line, match, re) && match.size() > 1) {
      metadata["spellLevel"] = match.str(1);
      switch (match.size()) {
        // Exploit fallthrough to populate as many spells as are defined
        case 6:
          metadata["spell4"] = match.str(5);
        case 5:
          metadata["spell3"] = match.str(4);
        case 4:
          metadata["spell2"] = match.str(3);
        case 3:
          metadata["spell1"] = match.str(2);
          break;
      }
    }
  }
  else if (this->type == "money") {
    iss >> metadata["silver"];
    iss >> metadata["gold"];
  }
  else if (this->type == "map") {
    int saveable;

    iss >> saveable;
    if (saveable) {
      metadata["saveable"] = "true";
    }
  }
  else if (this->type == "light") {
    iss >> num >> num >> metadata["remaining"];
  }
//  else {
//    iss >> metadata["val0"] >> metadata["val1"] >> metadata["val2"] >> metadata["val3"] >> metadata["val4"];
//  }
}

static item_ptr read_item(const std::string& first_line, std::ifstream& in)
{
  auto               item = std::make_shared<struct item>();
  char               c;

  std::istringstream iss;

  iss.clear();
  iss.str(first_line);
  iss >> c >> item->vnum;

  item->id         = get_ranvier_id(item->vnum);
  item->keywords   = get_mud_string(in);
  item->short_desc = get_mud_string(in);
  item->long_desc  = get_mud_string(in);
  item->material   = get_mud_string(in);

  std::string extraFlags;
  in >> item->type >> extraFlags >> item->metadata["slot"];
  if (extraFlags != "0") {
    item->metadata["extraFlags"] = extraFlags;
  }

  in.ignore();

  item->parse_type_specific_data(next_line(in));

  in >> item->metadata["level"] >> item->metadata["weight"] >> item->metadata["cost"] >> c;
  if (condition_lookup.find(c) != condition_lookup.end()) {
    item->metadata["condition"] = condition_lookup[c];
  }
  in.ignore();

  char flag;
  while ((c = in.peek()) != '#') {
    switch (c) {
      case 'E':
      {
        next_line(in);
        std::string keyword = get_mud_string(in), desc = get_mud_string(in);
        item->extra_descs[keyword] = desc;
      }
      break;

      case 'A':
      {
        int loc, mod;
        in >> flag >> loc >> mod;
        if (affects.find(loc) != affects.end()) {
          item->stats[affects[loc]] = std::to_string(mod);
        }
        in.ignore();
      }
      break;

      case 'F':
        next_line(in);
        next_line(in);

        break;

      case '#':
        break;

      default:
        break;
    }
  }

  item_lookup[item->vnum] = item;

  return item;
}

void parse_items(std::ifstream& in, area_ptr area)
{
  std::string line;

  while (std::getline(in, line)) {
    if (line == "#0") {
      break;
    }

    try {
      area->items.push_back(read_item(line, in));
    }
    catch (std::exception e) {
      std::cerr << "failed to load item: " << e.what() << std::endl;
    }
  }
}