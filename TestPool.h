#ifndef TESTPOOL_H
#define TESTPOOL_H

#include <fann.h>
#include <thread>
#include <mutex>
#include <vector>
#include "VariationResult.h"

/**
 * @brief      	Intervalle à parcourir pour faire varier un paramètre.
 * 
 * Se retrouve typiquement dans une boucle sous cette forme : for(float i = start; i < end; i += step)
 */
struct VariationRange {
	float start;
	float end;
	float step;
};

/** Set de paramètres utilisé par la fonction RunNeuralNetwork pour configurer le RdN. **/
typedef std::map<std::string, float> ParameterSet;

/** Autorise la réalisation des tests en multithread.
* Utilisation typique :
* SetTestRanges();
* results = ExecuteTests();
**/
class TestPool {
private:
	// Nombre de threads à utiliser en simultané
	int nbThreads = 8;
	// Tableau de threads nécessaire pour les synchroniser à la fin des calculs
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

	/**
	 * @brief      Construit une liste de paramètres à tester à partir d'intervalles de valeurs à tester.
	 *
	 * @param[in]  testRanges    Association {"nom du paramètre", "intervalle de valeurs"}
	 * @param[in]  defaultParam  Paramètres par défaut à fournir au réseau.
	 */
	void SetTestRanges(std::map<std::string, VariationRange> testRanges, ParameterSet defaultParam);

	/**
	 * @brief      Execute les tests préalablement fournis.
	 *
	 * @return  testResults 	    Résultat des tests
	 */
	std::map<std::string, std::vector<VariationResult>> ExecuteTests();

	/**
	 * @brief      Récupère le prochain test à exécuter. Appelé uniquement par un TestThread lorsqu'il a fini un test.
	 *
	 * @param[out]      outTest		Le test à exécuter s'il en reste, inchangé sinon.
	 *
	 * @return     True s'il restait des tests à exécuter, false sinon.
	 */
	bool GetNextTest(std::pair<std::string, ParameterSet>& outTest);

	/**
	 * @brief      Ajoute un résultat de test. Appelé uniquement par un TestThread lorsqu'il a fini un test.
	 *
	 * @param[in]  testName  Nom du test terminé.
	 * @param[in]  result    Résultat du test
	 */
	void AddTestResult(std::string testName, VariationResult result);


};

/**
 * @brief      Thread d'exécution de tests.
 * 
 * Exécute en boucle des tests en les récupérant depuis le TestPool.
 * Une fois terminé, envoie le résultat du test au TestPool.
 *
 * @param      testPool  Le TestPool duquel tirer les tests et auquel envoyer les résultats.
 * @see        TestPool::GetNextTest, TestPool::AddTestResult
 */
void TestThread(TestPool* testPool);


#endif