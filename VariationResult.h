#ifndef VARIATIONRESULT_H
#define VARIATIONRESULT_H

#include <map>
#include <string>

typedef std::map<std::string, float> ParameterSet;


/** Résultat d'une phase d'apprentissage/test du RdN.
**/
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

	/** Ne fait rien par défaut **/
	VariationResult();

	/** Crée un objet à partir d'une chaîne formattée. **/
	VariationResult(std::string line);

	void WriteToStream(std::ostream& os);
};

#endif