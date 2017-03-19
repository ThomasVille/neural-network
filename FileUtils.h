#ifndef FILEUTILS_H
#define FILEUTILS_H
#include "VariationResult.h"
#include <map>
#include <string>
#include <vector>

/**
 * @brief      Vérifie l'existence d'un fichier
 *
 * @param[in]  fileName  Nom du fichier à tester.
 *
 * @return     True si le fichier existe, False sinon.
 */
bool FileExists(const std::string fileName);

/**
 * @brief      Ecrit les résultats d'une série de tests dans un fichier.
 *
 * @param[in]  testResults  Résultats des tests.
 * @param[in]  filename     Nom du fichier dans lequel écrire.
 */
void WriteToFile(std::map<std::string, std::vector<VariationResult>> testResults, const std::string filename);

/**
 * @brief      Lit les résultats d'une série de tests depuis un fichier.
 *
 * @param[in]  		filename     Nom du fichier à lire.
 * @param[out]      testResults  Résultats des tests que la fonction va remplir.
 */
void ReadFromFile(const std::string filename, std::map<std::string, std::vector<VariationResult>>& testResults);

/**
 * @brief      Sépare une chaîne de caractère en fonction d'un ou plusieurs délimiteurs.
 *
 * @param[in]  line        Chaîne de caractères à découper.
 * @param[in]  delimiters  Délimiteurs possibles.
 *
 * @return     Liste de chaînes de caractères.
 */
std::vector<std::string> SplitString(std::string line, std::string delimiters = " ");

#endif