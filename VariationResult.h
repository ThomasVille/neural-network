#ifndef VARIATIONRESULT_H
#define VARIATIONRESULT_H

#include <map>
#include <vector>
#include <string>

typedef std::map<std::string, float> ParameterSet;



/**
 * @brief      Résultat d'une phase d'apprentissage/test du RdN.
 */
struct VariationResult {
	// Paramètres qui ont été passés au réseau
	ParameterSet inputParameter;

	// Couleur du point
	// Nombre d'itérations
	int timeOfLearningProcess;

	// Courbe ROC
	// Position du point sur la courbe ROC
	float truePositiveRate;
	float falsePositiveRate;

	
	/**
	 * @brief      Définit les valeurs par défaut des différents paramètres
	 */
	VariationResult();

	/**
	 * @brief      Crée un objet à partir d'une chaîne formatée.
	 *
	 * @param[in]  line   La chaîne formatée.
	 * @param[in]      names  Les noms des paramètres à associer aux valeurs de la chaîne.
	 */
	VariationResult(std::string line, std::vector<std::string>& names);

	/**
	 * @brief      Ecrit le contenu de l'objet dans un flux (fichier ou autre)
	 *
	 * @param[out]      os    Flux dans lequel écrire.
	 */
	void WriteToStream(std::ostream& os);
};

#endif