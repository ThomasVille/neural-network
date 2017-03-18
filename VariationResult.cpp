#include "VariationResult.h"
#include <sstream>
#include <iostream>
using namespace std;

VariationResult::VariationResult()
{
    inputParameter["learning_coeff"] = 0.25f;
    inputParameter["desired_error"] = 0.001f;
    inputParameter["hidden_neurons_nb"] = 25;
    inputParameter["max_successive_augmentation_number"] = 10;
}

VariationResult::VariationResult(std::string line)
{
	//vector<string> values = SplitString(line);
	std::stringstream(line) >> inputParameter["learning_coeff"]
        >> inputParameter["desired_error"]
        >> inputParameter["hidden_neurons_nb"]
        >> inputParameter["max_successive_augmentation_number"]
        >> timeOfLearningProcess
        >> truePositiveRate
        >> falsePositiveRate;
    WriteToStream(cout);
}

/** Ecrit un VariationResult directement dans un flux (stdout ou fichier) **/
void VariationResult::WriteToStream(std::ostream& os)
{
    // Ecrit les paramètres d'entrée en premier
    for(auto p : inputParameter){
        os << p.second << " ";
    }
    // Puis les résultats
    os  << timeOfLearningProcess << " " 
        << truePositiveRate << " " 
        << falsePositiveRate << "\n";
}