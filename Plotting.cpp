#include "Plotting.h"
#include <iostream>
#include <algorithm>

using namespace std;

void ShowResults(string outputSVGFilename, map<string, vector<VariationResult>> testResults)
{
	int  i, j;
    PLFLT** x; PLFLT** y;
	PLFLT xMin = 0, yMin = 0, xMax = 1.0f, yMax = 1.0f;

	// Pour chaque paramètre testé
	for(auto& testSet : testResults)
	{
		// Trie les tests dans l'ordre de leur False positive rate
		cout << "First FPR before sort : " << testSet.second[0].falsePositiveRate << "\n";
		sort(testSet.second.begin(), testSet.second.end(), [](VariationResult a, VariationResult b) {
			return a.falsePositiveRate < b.falsePositiveRate;
		});
		cout << "First FPR after sort : " << testSet.second[0].falsePositiveRate << "\n";
	}

	// Met les résultats dans des tableaux prêts à être affichés
	x = new PLFLT*[testResults.size()];
	y = new PLFLT*[testResults.size()];
	i = 0;
	for(pair<string, vector<VariationResult>> testSet : testResults)
	{
		FillArrays(&(x[i]), &(y[i]), testSet.second);
		i++;
	}

    // Initialisation
	plsfnam(outputSVGFilename.c_str());
  	plsdev("svg");
	plinit();
	plssub( 2, 2 );

	i = 0;
	for(pair<string, vector<VariationResult>> testSet : testResults)
	{
		// Valeurs minimum et maximum des axes du repère
		SetMinMax(xMin, xMax, yMin, yMax, testSet.second);
		plenv(xMin*0.99f, xMax*1.01f, yMin*0.99f, yMax*1.01f, 0, 0);
		pllab( "False Positive Rate", "True Positive Rate", testSet.first.c_str() );
		plcol0(i+2);
		plline(testSet.second.size(), x[i], y[i]);
	    plpoin(testSet.second.size(), x[i], y[i], 3);

    	// Et les labels
		//for(j = 0; j < learning_coeff_results.size(); j++)
		//	plptex(x[0][j], y[0][j], 0, 0, 0, (" " + to_string(learning_coeff_results[j].inputParameter.learning_coeff)).c_str());
	    i++;
	}
	

	// Libération de la mémoire et écriture du fichier test.svg
  	plend();

  	for(i = 0; i < 3; i++) free(x[i]);
  	for(i = 0; i < 3; i++) free(y[i]);
  	//free(x);
  	//free(y);
}

void FillArrays(PLFLT** x, PLFLT** y, vector<VariationResult>& data)
{
	// Alloue la mémoire
	(*x) = (PLFLT*)malloc(sizeof(PLFLT)*data.size());
	(*y) = (PLFLT*)malloc(sizeof(PLFLT)*data.size());

	// Remplit les tableaux
	for(int j = 0; j < data.size(); j++)
	{
		(*x)[j] = data[j].falsePositiveRate;
		(*y)[j] = data[j].truePositiveRate;
	}
}

void SetMinMax(PLFLT& xMin, PLFLT& xMax, PLFLT& yMin, PLFLT& yMax, vector<VariationResult>& data)
{
	xMin = min_element(data.begin(), data.end(), [](VariationResult& a, VariationResult& b) {
		return a.falsePositiveRate < b.falsePositiveRate;
	})->falsePositiveRate;
	xMax = max_element(data.begin(), data.end(), [](VariationResult& a, VariationResult& b) {
		return a.falsePositiveRate < b.falsePositiveRate;
	})->falsePositiveRate;
	yMin = min_element(data.begin(), data.end(), [](VariationResult& a, VariationResult& b) {
		return a.truePositiveRate < b.truePositiveRate;
	})->truePositiveRate;
	yMax = max_element(data.begin(), data.end(), [](VariationResult& a, VariationResult& b) {
		return a.truePositiveRate < b.truePositiveRate;
	})->truePositiveRate;
}