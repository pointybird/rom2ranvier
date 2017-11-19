#ifndef __AREA_H__
#define __AREA_H__

#include <boost/filesystem.hpp>

struct mobile;
struct item;
struct room;

struct area {
  std::string name;
  std::string key;
  std::string builder;
  std::string level_range;

  int bottom = 0;
  int top    = 0;

  std::list<std::shared_ptr<mobile> > mobs;
  std::list<std::shared_ptr<item> > items;
  std::list<std::shared_ptr<room> > rooms;
};

typedef std::shared_ptr<area> area_ptr;

area_ptr                   parse_area(std::ifstream& in);
void                       save_area(area_ptr area);

void                       write_manifest(const boost::filesystem::path& manifest, area_ptr area);

extern std::list<area_ptr> areas;

int                        get_ranvier_id(int vnum);
std::string                get_ranvier_entity_id(int vnum);

#endif                                                // __AREA_H__