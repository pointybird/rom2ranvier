#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <fstream>
#include <string>

std::string              get_mud_string(std::ifstream& in);
std::string              next_line(std::ifstream& in, char delimiter = '\n');

long                     fread_flag(std::ifstream& in);

std::vector<std::string> to_string_array(const std::string& keywords);

#endif                                                // __UTILS_H__