#include "TestPool.h"
#include <iostream>
#include "NeuralNetwork.h"
using namespace std;

void TestPool::SetTestRanges(map<string, VariationRange> testRanges, ParameterSet defaultParam)
{
	testParameters.clear();

	// Set de paramètres envoyé au RdN
	ParameterSet param;

	// Construit le set de paramètres de test
	for(pair<string, VariationRange> test : testRanges){
		// Récupère le nom du paramètre à tester et l'intervalle à parcourir
		string paramName = test.first;
		VariationRange range = test.second;

		// Remet les paramètres d'entrée à leurs valeurs d'origine
		param = defaultParam;

		// Parcours l'intervalle
		for(float i = range.start; i < range.end; i += range.step)
		{
			// Modifie le paramètre
			param.at(paramName) = i;
			testParameters.push_back(pair<string, ParameterSet>(paramName, param));
		}
    }
}

bool TestPool::GetNextTest(pair<string, ParameterSet>& outTest)
{
	parameterStackMutex.lock();

	// S'il n'y a plus rien à calculer, on renvoie false et on déverouille
	if(testParameters.size() == 0) {
		parameterStackMutex.unlock();
		return false;
	}

	outTest = testParameters.back();
	testParameters.pop_back();
	
	cout << "Testing '" << outTest.first << "' : " << outTest.second[outTest.first] << "\n";

	parameterStackMutex.unlock();

	return true;
}

void TestPool::AddTestResult(string testName, VariationResult result)
{
	testResultsMutex.lock();

	testResults[testName].push_back(result);

	testResultsMutex.unlock();
}

map<string, vector<VariationResult>> TestPool::ExecuteTests()
{
	cout << "Starting tests on " << nbThreads << " threads\n";

	// Démarre les threads
	for(int i = 0; i < nbThreads; i++)
		testThread.push_back(thread(TestThread, this));

	
	// Attend la fin des calculs
	for(int i = 0; i < nbThreads; i++)
		testThread[i].join();

	cout << "Tests finished\n";

	return testResults;
}

void TestThread(TestPool* testPool)
{
	pair<string, ParameterSet> parameter;
	cout << "Starting test thread\n";

	// Tant qu'il reste des tests à effectuer, on les effectue
	while(testPool->GetNextTest(parameter))
	{
		VariationResult result = RunNeuralNetwork(parameter.second, testPool->train_set, testPool->validation_set, testPool->test_set);
		testPool->AddTestResult(parameter.first, result);
	}
	cout << "Stopping test thread\n";

}