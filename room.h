#ifndef __ROOM_H__
#define __ROOM_H__

struct area;

struct room_exit {
  room_exit(std::string room_id, std::string direction) : room_id(room_id), direction(direction) {}

  std::string room_id;
  std::string direction;
};
typedef std::shared_ptr<room_exit> exit_ptr;

struct door {
  std::string room_id;
  std::string key;
  bool closed = false;
  bool locked = false;
};
typedef std::shared_ptr<door> door_ptr;

struct room {
  int vnum = -1;

  int id;
  std::string name;
  std::string description;

  std::map<std::string, std::string> metadata;
  std::map<std::string, std::string> extra_descs;
  std::list<exit_ptr> exits;
  std::list<door_ptr> doors;

  std::list<std::shared_ptr<mobile_reset_info> > mob_loads;
  std::list<std::string> item_ids;
};

typedef std::shared_ptr<room> room_ptr;

void                              write_rooms(boost::filesystem::path& dir, area_ptr area);
void                              parse_rooms(std::ifstream& in, area_ptr area);

extern std::map<int, room_ptr>    room_lookup;
extern std::array<std::string, 6> directions;

#endif                                                // __ROOM_H__