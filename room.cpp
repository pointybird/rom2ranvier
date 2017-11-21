#include <regex>
#include <array>

#include <yaml-cpp/yaml.h>

#include "utils.h"
#include "area.h"
#include "mobile.h"
#include "flags.h"
#include "room.h"

using namespace boost::filesystem;
using namespace YAML;

std::map<int, room_ptr>           room_lookup;

std::array<std::string, 6>        directions = {
  { "north", "east", "south", "west", "up", "down" }
};

static std::map<int, std::string> sectors = {
  { 0, "inside" },
  { 1, "city" },
  { 2, "field" },
  { 3, "forest" },
  { 4, "hills" },
  { 5, "mountain" },
  { 6, "waterSwim" },
  { 7, "waterNoSwim" },
  { 9, "air" },
  { 10, "desert" }
};

void write_rooms(path& dir, area_ptr area)
{
  Emitter yaml;

  yaml << BeginSeq;

  for (auto room : area->rooms) {
    yaml << BeginMap;

    yaml << Key << "id" << Value << std::to_string(room->id);
    yaml << Key << "title" << Value << room->name;
    yaml << Key << "description" << Value << room->description;

    // Extra descs
    if (!room->extra_descs.empty()) {
      yaml << Key << "extraDescriptions" << Value << BeginMap;
      for (const auto& exdesc : room->extra_descs) {
        yaml << Key << exdesc.first << Value << exdesc.second;
      }

      yaml << EndMap;
    }

    // Metadata
    if (!room->metadata.empty()) {
      yaml << Key << "metadata" << Value << BeginMap;

      for (const auto& meta : room->metadata) {
        auto key   = meta.first;
        auto value = meta.second;
        if (value.empty()) {
          continue;
        }

        yaml << Key << key;
        if (key == "flags") {
          yaml << Flow << Value << parse_room_flags(value);
        }
        else {
          yaml << Value << value;
        }
      }

      yaml << EndMap;
    }

    // Exits
    if (!room->exits.empty()) {
      yaml << Key << "exits" << Value << BeginSeq;
      for (const auto& ex : room->exits) {
        yaml << BeginMap;
        yaml << Key << "roomId" << Value << ex->room_id;
        yaml << Key << "direction" << Value << ex->direction;
        yaml << EndMap;
      }

      yaml << EndSeq;
    }

    // Doors
    if (!room->doors.empty()) {
      yaml << Key << "doors" << Value << BeginMap;
      for (const auto& door : room->doors) {
        yaml << Key << door->room_id << Value << BeginMap;
        yaml << Key << "lockedBy" << Value << door->key;
        yaml << EndMap;
      }

      yaml << EndMap;
    }

    // Mobs
    if (!room->mob_loads.empty()) {
      yaml << Key << "npcs" << Value << BeginSeq;
      for (const auto& mob_load : room->mob_loads) {
        yaml << BeginMap;
        yaml << Key << "id" << Value << mob_load->mob_id;
        if (mob_load->max > 1) {
          yaml << Key << "maxLoad" << Value << mob_load->max;
        }
        yaml << EndMap;
      }

      yaml << EndSeq;
    }

    // Items
    if (!room->item_ids.empty()) {
      yaml << Key << "items" << Value << BeginSeq;
      for (const auto& item_id : room->item_ids) {
        yaml << BeginMap;
        yaml << Key << "id" << Value << item_id;
        yaml << EndMap;
      }

      yaml << EndSeq;
    }

    yaml << EndMap;
  }

  yaml << EndSeq;

  path          area_dir   = dir / area->key;
  auto          rooms_file = area_dir / "rooms.yml";
  std::ofstream out(rooms_file.c_str());
  out << yaml.c_str();
  out.close();
}

static void parse_room_extras(std::ifstream& in, room_ptr room)
{
  std::string        line;
  std::istringstream iss;
  char               flag;

  while (std::getline(in, line)) {
    iss.clear();
    iss.str(line);

    iss >> flag;

    switch (flag) {
      case 'S':
        return;

      case 'H':
      case 'M':
      {
        std::regex  re("(?:H\\s+(\\d+))?\\s*(?:M\\s+(\\d+))?");
        std::smatch match;

        if (std::regex_search(line, match, re) && match.size() > 2) {
          auto hp   = match.str(1);
          auto mana = match.str(2);

          if (!hp.empty()) {
            room->metadata["hpRate"] = hp;
          }
          if (!mana.empty()) {
            room->metadata["manaRate"] = mana;
          }
        }
      }
      break;

      case 'C':
        iss >> room->metadata["clan"];
        break;

      case 'D':
      {
        int         dir, locks, key, to_room;
        std::string desc, keyword;

        iss >> dir;
        desc    = get_mud_string(in);
        keyword = get_mud_string(in);

        iss.clear();
        iss.str(next_line(in));
        iss >> locks >> key >> to_room;

        std::string to_room_id;

        try {
          to_room_id = get_ranvier_entity_id(to_room);
        }
        catch (const std::invalid_argument& e) {
          std::cerr << to_room << ": " << e.what() << std::endl;
          break;
        }

        room->exits.push_back(
          std::make_shared<room_exit>(to_room_id, directions[dir]));

        // Make door (if needed)
        if (locks > 0) {
          // In Ranvier door is only defined on 1 of the rooms instead of both like Dikus.
          // As convention, we'll define it on the door with lower vnum
          if (room->vnum < to_room) {
            auto door = std::make_shared<struct door>();
            door->key     = key >= 0 ? get_ranvier_entity_id(key) : "false";
            door->room_id = to_room_id;

            room->doors.push_back(door);
          }
        }
      }
      break;

      case 'E':
      {
        std::string keyword = get_mud_string(in), desc = get_mud_string(in);
        room->extra_descs[keyword] = desc;
      }
      break;

      case 'O':
        iss >> room->metadata["owner"];
        break;

      default:
        std::cerr << "Unknown room extra flag '" << flag << "'" << std::endl;
        break;
    }
  }
}

static room_ptr read_room(const std::string& first_line, std::ifstream& in)
{
  auto               room = std::make_shared<struct room>();
  char               c;

  std::istringstream iss;

  iss.clear();
  iss.str(first_line);
  int num;
  iss >> c >> room->vnum;

  room->id          = get_ranvier_id(room->vnum);
  room->name        = get_mud_string(in);
  room->description = get_mud_string(in);

  int sector;
  in >> num >> room->metadata["flags"] >> sector;
  if (sectors.find(sector) != sectors.end()) {
    room->metadata["sectorType"] = sectors[sector];
  }
  in.ignore();

  parse_room_extras(in, room);

  room_lookup[room->vnum] = room;

  return room;
}

void parse_rooms(std::ifstream& in, area_ptr area)
{
  std::string line;

  while (std::getline(in, line)) {
    if (line == "#0") {
      break;
    }

    try {
      area->rooms.push_back(read_room(line, in));
    }
    catch (std::exception e) {
      std::cerr << "failed to load room: " << e.what() << std::endl;
    }
  }
}
