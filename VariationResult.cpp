#include "VariationResult.h"
#include "FileUtils.h"
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

VariationResult::VariationResult()
{
    inputParameter["learning_coeff"] = 0.25f;
    inputParameter["desired_error"] = 0.001f;
    inputParameter["hidden_neurons_nb"] = 25;
    inputParameter["max_successive_augmentation_number"] = 10;
}

VariationResult::VariationResult(std::string line, vector<string>& names)
{
	vector<string> values = SplitString(line);
    // Vérifie la cohérence entre le nombre de valeurs sur la ligne et le nombre de noms de paramètres
    if(values.size() != names.size() + 3) {
        cerr << "File read error\n";
    }

    // Lit les paramètres d'entrée
    for(int i = 0; i < names.size(); i++){
        inputParameter[names[i]] = stof(values[i]);
    }
    timeOfLearningProcess = stof(values[values.size()-3]);
    truePositiveRate = stof(values[values.size()-2]);
    falsePositiveRate = stof(values[values.size()-1]);
}

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