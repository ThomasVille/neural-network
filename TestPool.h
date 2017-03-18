#ifndef TESTPOOL_H
#define TESTPOOL_H

#include <fann.h>
#include <thread>
#include <mutex>
#include <vector>
#include "VariationResult.h"

/** Intervalle à parcourir pour faire varier un paramètre **/
struct VariationRange {
	float start;
	float end;
	float step;
};

typedef std::map<std::string, float> ParameterSet;

/** Autorise la réalisation des tests en multithread.
* 
**/
class TestPool {
private:
	int nbThreads = 8;
	std::vector<std::thread> testThread;

	// Liste de paramètres à tester
	std::mutex parameterStackMutex;
	std::vector<std::pair<std::string, ParameterSet>> testParameters;
	// Liste de résultats d'exécution
	std::mutex testResultsMutex;
	std::map<std::string, std::vector<VariationResult>> testResults;

public:
	// Données
	fann_train_data* train_set = nullptr;		// Set d'entrainement (60%)
	fann_train_data* validation_set = nullptr;	// Set de validation (20%)
	fann_train_data* test_set = nullptr;		// Set de test (20%)

	/** Construit une liste de paramètres à tester à partir d'intervalles de valeurs à tester.
	* \param testRanges 	Association ("nom du test", "intervalle de valeurs")
	* \param defaultParam	Paramètres par défaut à fournir au réseau
	**/
	void SetTestRanges(std::map<std::string, VariationRange> testRanges, ParameterSet defaultParam);

	bool GetNextTest(std::pair<std::string, ParameterSet>& outTest);

	void AddTestResult(std::string testName, VariationResult result);

	std::map<std::string, std::vector<VariationResult>> ExecuteTests();

};

void TestThread(TestPool* testPool);


#endif