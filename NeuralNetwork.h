#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <fann.h>
#include <vector>
#include "VariationResult.h"


const bool MALWARE = true;
const bool SAIN = false;

/**
 * @brief      Compare deux sorties du réseau.
 *
 * @param[in]      expected_outputs  La sortie attendue.
 * @param[in]      real_outputs      La vraie sortie.
 *
 * @return     True si toutes les sorties sont équivalentes (si les résultats sont 2-à-2 égaux après passage dans floatToBool), False sinon
 * @see floatToBool
 */
bool compare_outputs(fann_type *expected_outputs, fann_type *real_outputs);

/**
 * @brief      Convertit un float [0,1] en bool [True, False].
 * 
 * Si a > 0.5 alors True
 * Sinon si a < 0.5 alors False
 *
 * @param[in]  a     Valeur comprise entre 0 et 1 à convertir.
 *
 * @return     Résultat de la conversion.
 */
bool floatToBool(float a);

/**
 * @brief      Interprète une sortie du RdN en termes de MALWARE et SAIN.
 * 
 * Si N0 > N1, c'est un malware
 * Si N0 < N1, c'est sain
 * 
 * @param[int]      real_outputs  Sortie du RdN.
 *
 * @return     Type de programme correspondant à la sortie.
 */
bool interpret_outputs(fann_type *real_outputs);

/**
 * @brief      Selectionne le RdN qui a le plus petit MSE.
 *
 * @param[in]  arr   Liste de RdN
 *
 * @return     RdN qui a le plus petit MSE
 */
fann* selectMin(std::vector<fann*> arr);

/**
 * @brief      Lance tout un cycle apprentissage/test sur un set de données en suivant les paramètres fournis.
 *
 * @param[in]  	   parameter       Paramètres à utiliser lors de la création du RdN et de l'exécution des tests
 * @param[in]      train_set       Set de données d'entraînement
 * @param[in]      validation_set  Set de données de validation
 * @param[in]      test_set        Set de données de test
 *
 * @return     Résultat de l'exécution.
 */
VariationResult RunNeuralNetwork(ParameterSet parameter, fann_train_data* train_set, fann_train_data* validation_set, fann_train_data* test_set);

/**
 * @brief      Affiche tous les éléments d'un set de paramètres.
 *
 * @param[in]  param  Set de paramètres à afficher.
 */
void PrintParam(ParameterSet param);

/**
 * @brief      Lit un fichier de données et génère 3 sets de données test/apprentissage/validation
 *
 * @param[in] 		inputDatasetFilename  Nom du fichier de données.
 * @param[out]      train_set             Set de données d'entraînement
 * @param[out]      validation_set        Set de données de validation
 * @param[out]      test_set              Set de données de test
 */
void InitializeData(std::string inputDatasetFilename, fann_train_data** train_set, fann_train_data** validation_set, fann_train_data** test_set);


#endif