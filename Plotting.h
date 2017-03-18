#ifndef PLOTTING_H
#define PLOTTING_H

#include "VariationResult.h"
#include <map>
#include <string>
#include <vector>
#include <plplot.h>

void ShowResults(std::string outputSVGFilename, std::map<std::string, std::vector<VariationResult>> testResults);
void SetMinMax(PLFLT& xMin, PLFLT& xMax, PLFLT& yMin, PLFLT& yMax, std::vector<VariationResult>& data);
void FillArrays(PLFLT** x, PLFLT** y, std::vector<VariationResult>& data);

#endif