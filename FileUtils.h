#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "VariationResult.h"
#include <map>
#include <string>
#include <vector>

bool FileExists(const std::string fileName);
void WriteToFile(std::map<std::string, std::vector<VariationResult>> testResults, const std::string filename);
void ReadFromFile(const std::string filename, std::map<std::string, std::vector<VariationResult>>& testResults);
std::vector<std::string> SplitString(std::string line, std::string delimiters = " ");

#endif