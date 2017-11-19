#ifndef __MOBILE_H__
#define __MOBILE_H__

#include <map>
#include <set>

#include <boost/filesystem.hpp>

struct mobile_reset_info {
  std::string mob_id;
  int max = 1;
};

struct mobile {
  int vnum = -1;

  int id;

  std::string keywords;
  std::string short_desc;
  std::string long_desc;
  std::string description;

  bool vendor        = false;
  bool sentinel      = false;
  bool aggressive    = false;
  bool stays_in_area = false;

  int level = 1;
  int gold  = 0;

  std::map<std::string, std::string> metadata;

  std::set<int> items;
  std::map<std::string, std::string> equipment;
};
typedef std::shared_ptr<mobile> mobile_ptr;

void                             parse_mobs(std::ifstream& in, std::shared_ptr<area> area);
void                             parse_shops(std::ifstream& in);
void                             write_mobs(boost::filesystem::path& dir, std::shared_ptr<area> area);

extern std::map<int, mobile_ptr> mob_lookup;

#endif                                                // __MOBILE_H__