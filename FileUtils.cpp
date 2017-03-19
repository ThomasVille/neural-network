#include "FileUtils.h"
#include <fstream>
using namespace std;

bool FileExists(const string fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}


void WriteToFile(map<string, vector<VariationResult>> testResults, const string filename)
{
    // Fichier pour stocker les différents résultats
    ofstream file;
    file.open(filename, ios::app); // Ouvre le fichier en écriture

	// Ecrit le nom de chaque paramètre (même les non-testés)
    for(auto p : VariationResult().inputParameter){
    	file << p.first << " ";
    }
    file.seekp(-2, ios_base::cur); // Se décale à gauche pour enlever le dernier espace
    file << "\n";

    // Ecrit le nombre de tests par paramètre
    for(auto p : testResults){
    	file << p.second.size() << " ";
    }
    file.seekp(-2, ios_base::cur); // Se décale à gauche pour enlever le dernier espace
    file << "\n";

    // Pour chaque paramètre testé
    for(auto p : testResults){
    	vector<VariationResult>& results = p.second;

    	// Ecrit les résultats de chacun de ses tests
    	for(int i = 0; i < results.size(); i++){
	        results[i].WriteToStream(file); 
	    }
    }
}
void ReadFromFile(const string filename, map<string, vector<VariationResult>>& testResults)
{
    ifstream file;
    string line;
    vector<int> sizes;
    vector<string> names;

    file.open(filename, ios::in); // Ouvre le fichier en lecture

    // Récupère le nom de chaque paramètre testé
    getline(file, line);
    // Remarque : la fonction n'est appelée qu'une seule fois (voir implémentation standard)
    for(string paramName : SplitString(line))
    {
    	names.push_back(paramName);
    }

    // Récupère la taille de chaque plage de tests
    getline(file, line);
    // Remarque : la fonction n'est appelée qu'une seule fois (voir implémentation standard)
    for(string size : SplitString(line))
    {
    	sizes.push_back(stoi(size));
    }

    // Pour chaque paramètre
    for(int i = 0; i < sizes.size(); i++)
    {
    	// Pour chaque test du paramètre
    	for(int j = 0; j < sizes[i]; j++)
    	{
    		getline(file, line);
    		// Ajoute le résultat du test
        	testResults[names[i]].push_back(VariationResult(line, names));
    	}
    }
}

/** Sépare une chaîne de caractère en fonction d'un ou plusieurs délimiteurs.
* 
**/
vector<string> SplitString(string line, string delimiters)
{
    // Position actuelle dans la chaîne (début du token)
    size_t current;
    // Position du prochain delimiter (fin du token)
    size_t next = -1;
    // Liste de tokens à renvoyer
    vector<string> tokens;

    // Enlève les espaces de fin de ligne
    for(int i = line.size()-1; i >= 0 && line[i] == ' '; i--){
        line.erase(line.begin()+i);
    }

    do
    {
        current = next + 1;
        next = line.find_first_of( delimiters, current );
        tokens.push_back(line.substr( current, next - current ));
    }
    while (next != string::npos);

    return tokens;
}