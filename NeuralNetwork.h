#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <fann.h>
#include <vector>
#include "VariationResult.h"


const bool MALWARE = true;
const bool SAIN = false;

bool compare_outputs(fann_type *expected_outputs, fann_type *real_outputs);
bool interpret_outputs(fann_type *real_outputs);
fann* selectMin(std::vector<fann*> arr);
VariationResult RunNeuralNetwork(ParameterSet parameter, fann_train_data* train_set, fann_train_data* validation_set, fann_train_data* test_set);
void PrintParam(ParameterSet param);
void InitializeData(std::string inputDatasetFilename, fann_train_data** train_set, fann_train_data** validation_set, fann_train_data** test_set);

#endif