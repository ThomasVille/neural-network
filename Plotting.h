#ifndef PLOTTING_H
#define PLOTTING_H

#include "VariationResult.h"
#include <map>
#include <string>
#include <vector>
#include <plplot.h>

/** Génère un fichier SVG contenant les courbes ROC des résultats passés en paramètres.
 * 
 * @param[in]  outputSVGFilename  Le nom du fichier SVG à générer.
 * @param[in]  testResults        Les résultats à écrire dans le fichier.
*/
void ShowResults(std::string outputSVGFilename, std::map<std::string, std::vector<VariationResult>> testResults);

/** Récupère les limites minimum et maximum en X et en Y d'un set de données
 * 
 * @param[out]      xMin  Minimum en X
 * @param[out]      xMax  Maximum en X
 * @param[out]      yMin  Minimum en Y
 * @param[out]      yMax  Maximum en Y
 * @param[in]      data  Données à afficher.
*/
void SetMinMax(PLFLT& xMin, PLFLT& xMax, PLFLT& yMin, PLFLT& yMax, std::vector<VariationResult>& data);

/** Remplie un tableau de PLFLT avec des données issues d'un tableau de
 * VariationResult.
 * 
 * La fonction alloue la place nécessaire pour les tableaux.
 * 
 * @param[out]      x     Pointeur vers un tableau de PLFLT
 * @param[out]      y     Pointeur vers un tableau de PLFLT
 * @param[in]      data  Données à mettre dans les tableaux
*/
void FillArrays(PLFLT** x, PLFLT** y, std::vector<VariationResult>& data);

#endif