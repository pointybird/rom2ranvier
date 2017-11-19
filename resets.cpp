#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include <boost/algorithm/string.hpp>

#include "area.h"
#include "item.h"
#include "mobile.h"
#include "room.h"

static std::map<int, std::string> wear_slots = {
  { 0, "light" },
  { 1, "left finger" },
  { 2, "right finger" },
  { 3, "neck 1" },
  { 4, "neck 2" },
  { 5, "body" },
  { 6, "head" },
  { 7, "legs" },
  { 8, "feet" },
  { 9, "hands" },
  { 10, "arms" },
  { 11, "shield" },
  { 12, "about" },
  { 13, "waist" },
  { 14, "left wrist" },
  { 15, "right wrist" },
  { 16, "wield" },
  { 17, "hold" },
  { 18, "float" },
};

void parse_resets(std::ifstream& in)
{
  std::istringstream iss;
  std::string        line;

  char               flag;
  int                max, if_flag, position, dir, flags;
  int                mob_vnum = -1, item_vnum = -1, room_vnum = -1, container_vnum = -1;

  while (std::getline(in, line)) {
    iss.clear();
    iss.str(line);

    if (line.empty()) {
      break;
    }
    ;

    iss >> flag;

    switch (flag) {
      case 'S':                                       // End of section
        return;

      case '*':                                       // Comment
        break;

      case 'M':                                       // Mob to room
        iss >> if_flag >> mob_vnum >> max >> room_vnum;

        if (room_lookup.find(room_vnum) != room_lookup.end()) {
          auto mob_load = std::make_shared<mobile_reset_info>();
          mob_load->mob_id = get_ranvier_entity_id(mob_vnum);
          mob_load->max    = max == -1 ? 999 : max;
          room_lookup[room_vnum]->mob_loads.push_back(mob_load);
        }
        break;

      case 'E':                                       // Equip item to mob
        iss >> if_flag >> item_vnum >> max >> position;

        if (mob_vnum >= 0 && mob_lookup.find(mob_vnum) != mob_lookup.end()) {
          if (wear_slots.find(position) != wear_slots.end()) {
            mob_lookup[mob_vnum]->equipment[wear_slots[position]] = get_ranvier_entity_id(item_vnum);
          }
        }
        break;

      case 'G':                                       // Item to mob inventory
        iss >> if_flag >> item_vnum;

        if (mob_vnum >= 0 && mob_lookup.find(mob_vnum) != mob_lookup.end()) {
          mob_lookup[mob_vnum]->items.insert(item_vnum);
        }
        break;

      case 'O':                                       // Item to room
        iss >> if_flag >> item_vnum >> max >> room_vnum;

        if (room_lookup.find(room_vnum) != room_lookup.end()) {
          room_lookup[room_vnum]->item_ids.push_back(get_ranvier_entity_id(item_vnum));
        }
        break;

      case 'R':                                       // TODO - Random resets
        break;

      case 'P':                                       // Item to container
        iss >> if_flag >> item_vnum >> max >> container_vnum;

        if (item_lookup.find(container_vnum) != item_lookup.end()) {
          item_lookup[container_vnum]->contents.push_back(get_ranvier_entity_id(item_vnum));
        }
        break;

      case 'D':
        iss >> if_flag >> room_vnum >> dir >> flags;

        if (room_lookup.find(room_vnum) != room_lookup.end()) {
          auto room = room_lookup[room_vnum];
          // Find exit
          auto ex_iter = std::find_if(room->exits.begin(), room->exits.end(),
                                      [&](const exit_ptr& ex) {
          return ex->direction == directions[dir];
        });

          if (ex_iter != room->exits.end()) {
            // Find door for exit
            auto door_iter = std::find_if(room->doors.begin(), room->doors.end(),
                                          [&](const door_ptr& door) {
            return door->room_id == (*ex_iter)->room_id;
          });

            if (door_iter != room->doors.end()) {
              auto door = *door_iter;

              switch (flags) {
                case 0:
                  door->closed = door->locked = false;
                  break;
                case 1:
                  door->closed = true;
                  door->locked = false;
                  break;
                case 2:
                  door->closed = door->locked = true;
                  break;
              }
            }
          }
        }
        break;

      default:
        std::cerr << "Unknown reset code in line '" << line << "'" << std::endl;
        break;
    }
  }
}