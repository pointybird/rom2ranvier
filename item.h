#ifndef __ITEM_H__
#define __ITEM_H__

struct item {
  int vnum = -1;

  int id;
  std::string keywords;
  std::string short_desc;
  std::string long_desc;
  std::string material;
  std::string type;

  std::map<std::string, std::string> metadata;
  std::map<std::string, std::string> extra_descs;
  std::map<std::string, std::string> stats;

  void parse_type_specific_data(const std::string& line);

  // Container specific info not currently stored in metadata
  bool closeable = false;
  bool closed    = false;
  bool locked    = false;
  std::string key_id;

  std::list<std::string> contents;
};

typedef std::shared_ptr<item> item_ptr;

void                           parse_items(std::ifstream& in, std::shared_ptr<area> area);
void                           write_items(boost::filesystem::path& dir, std::shared_ptr<area> area);

extern std::map<int, item_ptr> item_lookup;

#endif                                                // __ITEM_H__