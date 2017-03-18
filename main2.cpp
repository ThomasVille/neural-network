/*
// 5971 inputs
// 25 hidden
// 2 outputs
// Question ouverte :
- Learning coefficient
- Nombre d'augmentations avant arret
- Nombre de neurones de la couche cachée
// Les données à stocker :
- Les paramètres qui ont changé et leur valeur
- TPR, FPR
- Learning time
// A afficher :
- Chaque point du ROC (TPR, FPR)
- Learning time

Ce programme calcule pour chaque paramètre,
génère un fichier contenant tous les résultats,
produit un graphique représentant les résultats.
*/
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "TestPool.h"
#include "NeuralNetwork.h"
#include "FileUtils.h"
#include "Plotting.h"
using namespace std;


string outputDataFilename = "result.data";
string inputDatasetFilename = "android-features.data";
string outputSVGFilename = "roc-curve.svg";

bool ProcessParameters(int argc, char** argv);
void DisplayHelp(string programmeName);


int main(int argc, char** argv) {
	// Set de paramètres par défaut
	ParameterSet defaultParam = VariationResult().inputParameter;

	// Intervalles à parcourir pour chaque paramètre
	map<string, VariationRange> mediumTestRanges = {
		{"learning_coeff", {0.01, 1.0f, 0.2f} },
		{"hidden_neurons_nb", {1, 100, 10} },
		{"max_successive_augmentation_number", {1, 30, 1} }
	};

	// Résultats des tests 
	map<string, vector<VariationResult>> testResults;

	// Gestionnaire de tests
    TestPool testPool;

    // Traite les paramètres passés au programme
    if(!ProcessParameters(argc, argv)){
    	cerr << "[Error] Exiting\n";
    	return -1;
    }

    // Affiche les données d'entrée du programme
	cout << "[Initializing] Input dataset file : " << inputDatasetFilename << "\n";
	cout << "[Initializing] Output result file : " << outputDataFilename << "\n";
	cout << "[Initializing] Output SVG file : " << outputSVGFilename << "\n";

    // Si le fichier de résultat existe déjà, alors on ne calcule pas
    if(!FileExists(outputDataFilename))
    {
    	cout << "[Initializing] Output file does not exist\n";
    	cout << "[Initializing] Beginning tests\n";

    	InitializeData(inputDatasetFilename, &testPool.train_set, &testPool.validation_set, &testPool.test_set);

    	testPool.SetTestRanges(mediumTestRanges, defaultParam);
    	testResults = testPool.ExecuteTests();

		// Sauvegarde les résultats
        WriteToFile(testResults, outputDataFilename);
    }
    else
    {
    	cout << "[Initializing] Output file already exists\n";
        ReadFromFile(outputDataFilename, testResults);
    }

    cout << "Please open the " << outputSVGFilename << " file to see the ROC curve\n";
    // Affiche la courbe ROC des résultats
    ShowResults(outputSVGFilename, testResults);
	
	return 0;
}



/** Traite les paramètres passés au programme.
* Vérifie l'existence des fichiers d'input.
* Renvoie true si les paramètres sont cohérents et valide, false sinon.
**/
bool ProcessParameters(int argc, char** argv)
{
	vector<string> parameters;
	vector<string>::iterator it;
	// Construit un vector à partir des paramètres
	for(int i = 0; i < argc; i++)
		parameters.push_back(argv[i]);

	// On a un paramètre
	if(argc > 1)
	{
		// Check for --help
		it = find(parameters.begin(), parameters.end(), "--help");
		if(it != parameters.end())
		{
			DisplayHelp(parameters[0]);
			parameters.erase(it);
			return false;
		}
	}
	// On a au moins deux paramètres
	if(argc > 2)
	{
		// On ajoute le -1 pour ne pas parcourir le dernier élément qui est le nom du fichier de sortie
		it = find(parameters.begin(), parameters.end()-1, "--input");
		if(it != parameters.end()-1)
		{
			inputDatasetFilename = *(it+1);
			parameters.erase(it, it+2);
		}

		it = find(parameters.begin(), parameters.end()-1, "--out-result");
		if(it != parameters.end()-1)
		{
			outputDataFilename = *(it+1);
			parameters.erase(it, it+2);
		}

		it = find(parameters.begin(), parameters.end()-1, "--out-svg");
		if(it != parameters.end()-1)
		{
			outputSVGFilename = *(it+1);
			parameters.erase(it, it+2);
		}
	}

	// Affiche une erreur si certains paramètres sont invalides
	if(parameters.size() > 1){
		cerr << "[Error] Paramètres invalides : ";
		for(auto p = parameters.begin()+1; p != parameters.end(); p++)
			cerr << *p << ", ";
		cerr << "\n";

		return false;
	}

	// Vérifie l'existence des fichiers d'entrée
	if(!FileExists(inputDatasetFilename)){
		cerr << "[Error] Unable to open input dataset " << inputDatasetFilename << "\n";
		return false;
	}

	// Si aucun paramètre n'est passé, affiche que l'aide est disponible
	if(argc == 1){
		cout << "[Information] Help available with : " << argv[0] << " --help\n";
	}

	return true;
}

void DisplayHelp(string programmeName)
{
	cout << "USAGE : " << programmeName << " --help\n"
		 << "           Displays this help.\n";
	cout << "USAGE : " << programmeName << " [--input <input_dataset_file>] [--out-result <output_result_file>] [--out-svg <output_svg_file>]\n"
		 << "           Process the given input file and produce a result file and a ROC curve. If a result file already exists, only produce the ROC curve.\n";
}

