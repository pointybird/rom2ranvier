#ifndef __FLAGS_H__
#define __FLAGS_H__

// Mob flags
std::vector<std::string> parse_act_flags(std::shared_ptr<mobile> mob);
std::vector<std::string> parse_affected_flags(const std::string& str);
std::vector<std::string> parse_off_flags(const std::string& str);
std::vector<std::string> parse_resistance_flags(const std::string& str);

// Item flags
std::vector<std::string> parse_wear_flags(std::shared_ptr<item> item);
std::vector<std::string> parse_extra_flags(const std::string& str);
std::vector<std::string> parse_weapon_flags(const std::string& str);
void                     parse_container_flags(item& item, const std::string& flags);

// Room flags
std::vector<std::string> parse_room_flags(const std::string& str);

#endif                                                // __FLAGS_H__