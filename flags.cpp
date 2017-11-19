#include <map>

#include "area.h"
#include "item.h"
#include "mobile.h"

static bool has_flag(const std::string& str, const std::string& flag)
{
  return str.find(flag) != std::string::npos;
}

static void flag_check(const std::string& str, std::vector<std::string>& flags, const char *flag, const char *value)
{
  if (has_flag(str, flag)) {
    flags.push_back(value);
  }
}

std::vector<std::string> parse_act_flags(mobile_ptr mob)
{
  std::vector<std::string> flags;

  auto                     act = mob->metadata["actions"];

  // Flags that are handled by behaviors
  if (has_flag(act, "B")) {
    mob->sentinel = true;
  }
  if (has_flag(act, "F")) {
    mob->aggressive = true;
  }
  if (has_flag(act, "G")) {
    mob->stays_in_area = true;
  }

  // Flags to store in metadata
  flag_check(act, flags, "C", "scavenger");
  flag_check(act, flags, "I", "pet");
  flag_check(act, flags, "J", "train");
  flag_check(act, flags, "K", "practice");
  flag_check(act, flags, "O", "undead");
  flag_check(act, flags, "Q", "cleric");
  flag_check(act, flags, "R", "mage");
  flag_check(act, flags, "S", "thief");
  flag_check(act, flags, "T", "warrior");
  flag_check(act, flags, "U", "noAlign");
  flag_check(act, flags, "V", "noPurge");
  flag_check(act, flags, "W", "outdoors");
  flag_check(act, flags, "Y", "indoors");
  flag_check(act, flags, "aa", "healer");
  flag_check(act, flags, "bb", "gain");
  flag_check(act, flags, "cc", "updateAlways");
  flag_check(act, flags, "dd", "changer");

  return flags;
}

std::vector<std::string> parse_affected_flags(const std::string& str)
{
  std::vector<std::string> flags;

  flag_check(str, flags, "A", "blind");
  flag_check(str, flags, "B", "invisible");
  flag_check(str, flags, "C", "detectEvil");
  flag_check(str, flags, "D", "detectInvis");
  flag_check(str, flags, "E", "detectMagic");
  flag_check(str, flags, "F", "detectHidden");
  flag_check(str, flags, "G", "detectGood");
  flag_check(str, flags, "H", "sanctuary");
  flag_check(str, flags, "I", "faerieFire");
  flag_check(str, flags, "J", "infrared");
  flag_check(str, flags, "K", "curse");
  flag_check(str, flags, "M", "poison");
  flag_check(str, flags, "N", "protectEvil");
  flag_check(str, flags, "O", "protectGood");
  flag_check(str, flags, "P", "sneak");
  flag_check(str, flags, "Q", "hide");
  flag_check(str, flags, "R", "sleep");
  flag_check(str, flags, "S", "charm");
  flag_check(str, flags, "T", "flying");
  flag_check(str, flags, "U", "passDoor");
  flag_check(str, flags, "V", "haste");
  flag_check(str, flags, "W", "calm");
  flag_check(str, flags, "X", "plague");
  flag_check(str, flags, "Y", "weaken");
  flag_check(str, flags, "Z", "darkVision");
  flag_check(str, flags, "aa", "berserk");
  flag_check(str, flags, "bb", "swim");
  flag_check(str, flags, "cc", "regeneration");
  flag_check(str, flags, "dd", "slow");

  return flags;
}

std::vector<std::string> parse_off_flags(const std::string& str)
{
  std::vector<std::string> flags;

  flag_check(str, flags, "A", "areaAttack");
  flag_check(str, flags, "B", "backstab");
  flag_check(str, flags, "C", "bash");
  flag_check(str, flags, "D", "berserk");
  flag_check(str, flags, "E", "disarm");
  flag_check(str, flags, "F", "dodge");
  flag_check(str, flags, "G", "fade");
  flag_check(str, flags, "H", "fast");
  flag_check(str, flags, "I", "kick");
  flag_check(str, flags, "J", "kickDirt");
  flag_check(str, flags, "K", "parry");
  flag_check(str, flags, "L", "rescue");
  flag_check(str, flags, "M", "tail");
  flag_check(str, flags, "N", "trip");
  flag_check(str, flags, "O", "crush");
  flag_check(str, flags, "P", "assistAll");
  flag_check(str, flags, "Q", "assistAlign");
  flag_check(str, flags, "R", "assistRace");
  flag_check(str, flags, "S", "assistPlayers");
  flag_check(str, flags, "T", "assistGuard");
  flag_check(str, flags, "U", "assistVnum");

  return flags;
}

std::vector<std::string> parse_resistance_flags(const std::string& str)
{
  std::vector<std::string> flags;

  flag_check(str, flags, "A", "summon");
  flag_check(str, flags, "B", "charm");
  flag_check(str, flags, "C", "magic");
  flag_check(str, flags, "D", "weapon");
  flag_check(str, flags, "E", "bash");
  flag_check(str, flags, "F", "pierce");
  flag_check(str, flags, "G", "slash");
  flag_check(str, flags, "H", "fire");
  flag_check(str, flags, "I", "cold");
  flag_check(str, flags, "J", "lightning");
  flag_check(str, flags, "K", "acid");
  flag_check(str, flags, "L", "poison");
  flag_check(str, flags, "M", "negative");
  flag_check(str, flags, "N", "holy");
  flag_check(str, flags, "O", "energy");
  flag_check(str, flags, "P", "mental");
  flag_check(str, flags, "Q", "disease");
  flag_check(str, flags, "R", "drowning");
  flag_check(str, flags, "S", "light");
  flag_check(str, flags, "T", "sound");
  flag_check(str, flags, "X", "wood");
  flag_check(str, flags, "Y", "silver");
  flag_check(str, flags, "Z", "iron");

  return flags;
}

std::vector<std::string> parse_wear_flags(item_ptr item)
{
  std::vector<std::string> worn;

  auto                     wearFlags = item->metadata["slot"];

  if (!has_flag(wearFlags, "A")) {
    item->metadata["noPickup"] = "true";
  }

  flag_check(wearFlags, worn, "B", "finger");
  flag_check(wearFlags, worn, "C", "neck");
  flag_check(wearFlags, worn, "D", "body");
  flag_check(wearFlags, worn, "E", "head");
  flag_check(wearFlags, worn, "F", "legs");
  flag_check(wearFlags, worn, "G", "feet");
  flag_check(wearFlags, worn, "H", "hands");
  flag_check(wearFlags, worn, "I", "arms");
  flag_check(wearFlags, worn, "J", "shield");
  flag_check(wearFlags, worn, "K", "about");
  flag_check(wearFlags, worn, "L", "waist");
  flag_check(wearFlags, worn, "M", "wrist");
  flag_check(wearFlags, worn, "N", "wield");
  flag_check(wearFlags, worn, "O", "hold");
  flag_check(wearFlags, worn, "P", "noSac");
  flag_check(wearFlags, worn, "Q", "float");

  if (wearFlags == "A") {
    // If pick up flag was only thing on item, don't save out slot info
    item->metadata.erase("slot");
  }

  return worn;
}

std::vector<std::string> parse_extra_flags(const std::string& str)
{
  std::vector<std::string> flags;

  flag_check(str, flags, "A", "glow");
  flag_check(str, flags, "B", "hum");
  flag_check(str, flags, "C", "dark");
  flag_check(str, flags, "D", "lock");
  flag_check(str, flags, "E", "evil");
  flag_check(str, flags, "F", "invis");
  flag_check(str, flags, "G", "magic");
  flag_check(str, flags, "H", "noDrop");
  flag_check(str, flags, "I", "bless");
  flag_check(str, flags, "J", "antiGood");
  flag_check(str, flags, "K", "antiEvil");
  flag_check(str, flags, "L", "antiNeutral");
  flag_check(str, flags, "M", "noRemove");
  flag_check(str, flags, "N", "inventory");
  flag_check(str, flags, "O", "noPurge");
  flag_check(str, flags, "P", "rotDeath");
  flag_check(str, flags, "Q", "visDeath");
  flag_check(str, flags, "S", "nonMetal");
  flag_check(str, flags, "T", "noLocate");
  flag_check(str, flags, "U", "meltDrop");
  flag_check(str, flags, "V", "hadTimer");
  flag_check(str, flags, "W", "sellExtract");
  flag_check(str, flags, "Y", "burnproof");
  flag_check(str, flags, "Z", "noUncurse");

  return flags;
}

std::vector<std::string> parse_weapon_flags(const std::string& str)
{
  std::vector<std::string> flags;

  flag_check(str, flags, "A", "flaming");
  flag_check(str, flags, "B", "frost");
  flag_check(str, flags, "C", "vampiric");
  flag_check(str, flags, "D", "sharp");
  flag_check(str, flags, "E", "vorpal");
  flag_check(str, flags, "F", "twoHands");
  flag_check(str, flags, "G", "shocking");
  flag_check(str, flags, "H", "poison");

  return flags;
}

void parse_container_flags(item& item, const std::string& flags)
{
  if (has_flag(flags, "A")) {
    item.closeable = true;
  }
  if (has_flag(flags, "B")) {
    item.metadata["pickproof"] = "true";
  }
  if (has_flag(flags, "C")) {
    item.closed = true;
  }
  if (has_flag(flags, "D")) {
    item.locked = true;
  }
}

std::vector<std::string> parse_room_flags(const std::string& str)
{
  std::vector<std::string> flags;

  flag_check(str, flags, "A", "dark");
  flag_check(str, flags, "C", "noMob");
  flag_check(str, flags, "D", "indoors");
  flag_check(str, flags, "J", "private");
  flag_check(str, flags, "K", "safe");
  flag_check(str, flags, "L", "solitary");
  flag_check(str, flags, "M", "petShop");
  flag_check(str, flags, "N", "noRecall");
  flag_check(str, flags, "O", "impOnly");
  flag_check(str, flags, "P", "godsOnly");
  flag_check(str, flags, "Q", "heroesOnly");
  flag_check(str, flags, "R", "newbiesOnly");
  flag_check(str, flags, "S", "law");
  flag_check(str, flags, "T", "nowhere");

  return flags;
}