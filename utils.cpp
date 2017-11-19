#include <boost/algorithm/string.hpp>

#include "utils.h"

using namespace boost::algorithm;

#define DEBUG_PARSE 0

/**
 * Read from file until reaching a ~ delimiter
 */
std::string get_mud_string(std::ifstream& in)
{
  return next_line(in, '~');
}

std::string next_line(std::ifstream& in, char delimiter /* = '\n' */)
{
  std::string line;

  std::getline(in, line, delimiter);
#if DEBUG_PARSE
  std::cout << "Read line: '" << line << "'" << std::endl;
#endif
  if (delimiter != '\n') {
    in.ignore();
  }

  return line;
}

std::vector<std::string> to_string_array(const std::string& keywords)
{
  std::vector<std::string> words;

  boost::split(words, keywords, boost::is_any_of(" "));

  return words;
}