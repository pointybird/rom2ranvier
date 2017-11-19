# rom2ranvier
Utility to convert ROM .are files to yaml files for use in a [Ranvier](https://github.com/shawncplus/ranviermud) mud

## Prerequisites
* Boost, Boost.Filesystem
* A C++11 compiler

## Usage
`rom2ranvier <ROM area directory> <location to output areas directory for a Ranvier bundle>`

This code handles conversion of stock ROM area files but its main goal is to provide an example starting point to be customized for people looking to convert any Diku mud.