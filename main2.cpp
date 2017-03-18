/*
// 5971 inputs
// 25 hidden
// 2 outputs
// Question ouverte :
- Learning coefficient
- Nombre d'augmentations avant arret
- Nombre de neurones de la couche cachée
// Les données à stocker :
- Les paramètres qui ont changé et leur valeur
- TPR, FPR
- Learning time
// A afficher :
- Chaque point du ROC (TPR, FPR)
- Learning time

Ce programme calcule pour chaque paramètre,
génère un fichier contenant tous les résultats,
produit un graphique représentant les résultats.
*/
#include <fann.h>
#include <plplot.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <algorithm>
using namespace std;

#define MALWARE true
#define SAIN 	false

string outputDataFilename = "result.data";
string inputDatasetFilename = "android-features.data";
string outputSVGFilename = "roc-curve.svg";

/** Intervalle à parcourir pour faire varier un paramètre **/
struct VariationRange {
	float start;
	float end;
	float step;
};

/** Résultat d'une phase d'apprentissage/test du RdN.
**/
struct VariationResult {
	// Paramètres qui ont été passés au réseau
	map<string, float> inputParameter;

	// Couleur du point
	// Nombre d'itérations
	int timeOfLearningProcess;

	// Courbe ROC
	// Position du point sur la courbe ROC
	float truePositiveRate;
	float falsePositiveRate;

	/** Ne fait rien par défaut **/
	VariationResult() {
		inputParameter["learning_coeff"] = 0.25f;
		inputParameter["desired_error"] = 0.001f;
		inputParameter["hidden_neurons_nb"] = 25;
		inputParameter["max_successive_augmentation_number"] = 10;
	}

	/** Crée un objet à partir d'une chaîne formattée. **/
	VariationResult(string line)
	{
		stringstream(line) >> inputParameter["learning_coeff"]
            >> inputParameter["desired_error"]
            >> inputParameter["hidden_neurons_nb"]
            >> inputParameter["max_successive_augmentation_number"]
            >> timeOfLearningProcess
            >> truePositiveRate
            >> falsePositiveRate;
        cout << (*this);
	}

	friend std::ostream& operator<<(std::ostream& os, const VariationResult& obj);

};

/** Ecrit un VariationResult directement dans un flux (stdout ou fichier) **/
std::ostream& operator<<(std::ostream& os, const VariationResult& obj)
{
    // Ecrit les paramètres d'entrée en premier
    for(auto p : obj.inputParameter){
    	os << p.second << " ";
    }
    // Puis les résultats
	os  << obj.timeOfLearningProcess << " " 
	    << obj.truePositiveRate << " " 
	    << obj.falsePositiveRate << "\n";
    return os;
}


bool compare_outputs(fann_type *expected_outputs, fann_type *real_outputs);
bool interpret_outputs(fann_type *real_outputs);
fann* selectMin(vector<fann*> arr);
VariationResult RunNeuralNetwork(map<string, float> parameter, fann_train_data* train_set, fann_train_data* validation_set, fann_train_data* test_set);
void PrintParam(map<string, float> param);
void InitializeData(fann_train_data** train_set, fann_train_data** validation_set, fann_train_data** test_set);
bool FileExists(const string fileName);
void ShowResults(vector<VariationResult> learning_coeff_results, vector<VariationResult> hidden_neurons_results, vector<VariationResult> max_augmentation_results);
void WriteToFile(vector<VariationResult> learning_coeff_results, vector<VariationResult> hidden_neurons_results, vector<VariationResult> max_augmentation_results, const string filename);
void ReadFromFile(const string filename, vector<VariationResult>& learning_coeff_results, vector<VariationResult>& hidden_neurons_results, vector<VariationResult>& max_augmentation_results);
void SetMinMax(PLFLT& xMin, PLFLT& xMax, PLFLT& yMin, PLFLT& yMax, vector<VariationResult>& data);
void FillArrays(PLFLT** x, PLFLT** y, vector<VariationResult>& data);
bool ProcessParameters(int argc, char** argv);
void DisplayHelp(string programmeName);

// Résultats des différentes variations de paramètre
vector<VariationResult> learning_coeff_results, hidden_neurons_results, max_augmentation_results;


int main(int argc, char** argv) {
	// Set de paramètres envoyé au RdN
	map<string, float> param;
	// Set de paramètres par défaut utilisé pour restaurer 'param' après chaque plage de test
	map<string, float> defaultParam;
	defaultParam["learning_coeff"] = 0.25f;
	defaultParam["desired_error"] = 0.001f;
	defaultParam["hidden_neurons_nb"] = 25;
	defaultParam["max_successive_augmentation_number"] = 10;

	// Intervalles à parcourir pour chaque paramètre
	map<string, VariationRange> testRanges = {
		{"learning_coeff", {0.01, 1.0f, 0.2f} },
		{"hidden_neurons_nb", {1, 25, 5} },
		{"max_successive_augmentation_number", {1, 30, 10} }
	};

	map<string, vector<VariationResult>> testResults;

    fann_train_data* train_set = NULL;			// Set d'entrainement (60%)
    fann_train_data* validation_set = NULL;		// Set de validation (20%)
    fann_train_data* test_set = NULL;			// Set de test (20%)

    // Traite les paramètres passés au programme
    if(!ProcessParameters(argc, argv)){
    	cerr << "[Error] Exiting\n";
    	return -1;
    }

    // Affiche les données d'entrée du programme
	cout << "[Initializing] Input dataset file : " << inputDatasetFilename << "\n";
	cout << "[Initializing] Output result file : " << outputDataFilename << "\n";
	cout << "[Initializing] Output SVG file : " << outputSVGFilename << "\n";

    // Si le fichier de résultat existe déjà, alors on ne calcule pas
    if(!FileExists(outputDataFilename))
    {
    	cout << "[Initializing] Output file does not exist\n";
    	cout << "[Initializing] Beginning tests\n";
    	InitializeData(&train_set, &validation_set, &test_set);

    	// Teste les paramètres d'apprentissages
    	for(auto test : testRanges){
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

    			cout << "Varying '" << paramName << "' by " << range.step << " : " << i << "/" << range.end << "\n";

    			// Lance le calcul en récupérant le résultat
    			testResults[paramName].push_back(RunNeuralNetwork(param, train_set, validation_set, test_set));
    		}
	    }

		// Sauvegarde les résultats
        WriteToFile(testResults["learning_coeff"], testResults["hidden_neurons_nb"], testResults["max_successive_augmentation_number"], outputDataFilename);
    }
    else
    {
    	cout << "[Initializing] Output file already exists\n";
        ReadFromFile(outputDataFilename, testResults["learning_coeff"], testResults["hidden_neurons_nb"], testResults["max_successive_augmentation_number"]);
    }

    cout << "[Initializing] Showing data\n";
    // Affiche la courbe ROC des résultats
    ShowResults(testResults["learning_coeff"], testResults["hidden_neurons_nb"], testResults["max_successive_augmentation_number"]);
	
	return 0;
}


/** Lit un fichier de données et génère 3 sets de données (train, validation, test)
*/
void InitializeData(fann_train_data** train_set, fann_train_data** validation_set, fann_train_data** test_set)
{
	fann_train_data* total_train_set = NULL; 	// Set de données total
    fann_train_data* data_subset = NULL;		// Set de 20% des données
    int dataset_size = 0;						// Taille du set de données original
    int data_subset_size = 0;					// Taille du set de données à 20%
    int train_set_size = 0;						// Taille du set de données d'entrainement (60% des 20%)
    int validation_set_size = 0;				// Taille du set de données de validation (20% des 20%)
    int test_set_size = 0;						// Taille du set de données de test (20% des 20%)

	cout << "[Initializing]" << endl;
    cout << "[Initializing] Opening train file" << endl;

    /////////////// Set de données entier ///////////////
	total_train_set = fann_read_train_from_file(inputDatasetFilename.c_str());

    dataset_size = fann_length_train_data(total_train_set);
	cout << "[Initializing] Total dataset size : " << dataset_size << endl;

    cout << "[Initializing] Shuffling train data" << endl;
    fann_shuffle_train_data(total_train_set);

    data_subset_size = dataset_size*0.2;
    data_subset = fann_subset_train_data(total_train_set, 0, data_subset_size);
    data_subset_size = fann_length_train_data(data_subset);
    cout << "[Initializing] Subsetting 20\% of total dataset : " << data_subset_size << " elements" << endl;

    /////////////// Set de données entier ///////////////
    
    /////////////// Set de données déjà mélangées ///////////////
    /*data_subset = fann_read_train_from_file("shuffled.data");

    data_subset_size = fann_length_train_data(total_train_set);
	cout << "[Initializing] Total dataset size : " << dataset_size << endl;
    */////////////// Set de données déjà mélangées ///////////////

    cout << "[Initializing] Subsetting 60\% of the train subset to build the train set" << endl;
    *train_set = fann_subset_train_data(data_subset, 0, data_subset_size*0.6f);
    train_set_size = fann_length_train_data(*train_set);

    cout << "[Initializing] Subsetting 20\% of the train subset to build the validation set" << endl;
    *validation_set = fann_subset_train_data(data_subset, data_subset_size*0.6f, data_subset_size*0.2f);
    validation_set_size = fann_length_train_data(*validation_set);

    cout << "[Initializing] Subsetting 20\% of the train subset to build the test set" << endl;
    *test_set = fann_subset_train_data(data_subset, train_set_size+validation_set_size, data_subset_size*0.2f);
    test_set_size = fann_length_train_data(*test_set);
}

VariationResult RunNeuralNetwork(map<string, float> parameter, fann_train_data* train_set, fann_train_data* validation_set, fann_train_data* test_set)
{
	fann* ann = NULL;													// Réseau de neurones
	int i = 0;
    int train_set_size = fann_length_train_data(train_set);				// Taille du set de données d'entrainement (60% des 20%)
    int validation_set_size = fann_length_train_data(validation_set);	// Taille du set de données de validation (20% des 20%)
    int test_set_size = fann_length_train_data(test_set);				// Taille du set de données de test (20% des 20%)
    int successfuly_classified_number = 0;								// Nombre d'éléments correctement classifiés
    int max_epochs = 1000;
    int total_epochs = 0;

    float desired_error = parameter.at("desired_error");
    int epochs_between_reports = 1;
    float current_error = 0.0f;

    vector<fann*> candidates_ann;
    vector<float> learning_error;
    vector<float> validation_error;

    int successive_augmentation_number;
    int max_successive_augmentation_number = parameter.at("max_successive_augmentation_number");

    /* Vrai positif, Faux positif, Faux négatif, Vrai négatif */
    float TP = 0.0f, FP = 0.0f, FN = 0.0f, TN = 0.0f;

    
    //cout << "[Initializing] Neural network | 2. Cross Validation" << endl;
    //PrintParam(parameter);

    ann = fann_create_standard(3, 5971, (int)parameter.at("hidden_neurons_nb"), 2);
    fann_set_learning_rate(ann, parameter.at("learning_coeff"));
    fann_randomize_weights(ann, -0.77f, 0.77f);
	
    validation_set_size = fann_length_train_data(validation_set);
    test_set_size = fann_length_train_data(test_set);
    train_set_size = fann_length_train_data(train_set);
    
    
    //cout << "[Training] Training on train set" << endl;

    //cout << "Max epochs\t\t" << max_epochs << ". Desired error: " << desired_error << "." << endl;
    // Ne pas utiliser i pour accéder à un tableau ! Il va un élément trop loin !
    successive_augmentation_number = 0;
    for(i = 1; i <= max_epochs; i++){
    	// Report toutes les x itérations
    	if(i % epochs_between_reports == 0){
    		// Sauvegarde l'erreur d'apprentissage Knowledge
    		learning_error.push_back(current_error);
    		// Teste sur le set de validation et sauvegarde l'erreur Ability of Generalisation
    		validation_error.push_back(fann_test_data(ann, validation_set));

    		//cout << "Epochs\t\t" << i << ". Current error: " << current_error << ". Validation error : " << validation_error[validation_error.size()-1] << ".\n";

    		// Sauvegarde l'ANN
    		candidates_ann.push_back(fann_copy(ann));


    		// Compte les augmentations successives
    		if(validation_error.size() > 2 && validation_error[validation_error.size()-1] > validation_error[validation_error.size()-2])
    			successive_augmentation_number++;
    		else
    			successive_augmentation_number = 0;

    		// Arrete l'apprentissage après "n" augmentations successives
    		if(successive_augmentation_number == max_successive_augmentation_number){
    			break;
    		}
    	}

    	current_error = fann_train_epoch(ann, train_set);


    	if(current_error < desired_error){
    		//cout << "Epochs\t\t" << i+1 << ". Current error: " << current_error << ". Bit fail " << (int)(abs(rand()))%10000 << ".\n";
    		break;
    	}
    }

    total_epochs = i;
	
	//cout << "[Testing] Selecting best ANN based on MSE" << endl;
	fann_destroy(ann);
	ann = selectMin(candidates_ann);

    //cout << "[Testing] Running on test set" << endl;
    
    /* Les sorties attendues sont 0.05 et 0.95
    * On considère les valeurs de sortie du réseau :
    *   <0.5 comme 0.05
    *   >0.5 comme 0.95
    */
    for(int i = 0; i < test_set_size; i++){
    	fann_type *inputs = test_set->input[i];
    	fann_type *expected_outputs = test_set->output[i];
    	fann_type *real_outputs = fann_run(ann, inputs);
    	bool successfuly_classified = compare_outputs(expected_outputs, real_outputs);
    	bool real_output = interpret_outputs(real_outputs); // TRUE MALWARE ; FALSE SAIN

    	// Compte le nombre de valeurs correctement classifiées
    	successfuly_classified_number += successfuly_classified ? 1 : 0;

    	if(successfuly_classified && real_output == MALWARE) // Vrai positif
    		TP++;
    	if(successfuly_classified && real_output == SAIN) // Vrai négatif
    		TN++;
    	if(!successfuly_classified && real_output == MALWARE) // Faux positif
    		FP++;
    	if(!successfuly_classified && real_output == SAIN) // Faux négatif
    		FN++;
    }

    //cout << "[Testing] Accuracy of the ANN : " << (int)(100*successfuly_classified_number/test_set_size) << "\%" << endl;
    //cout << "[Testing] True positive : " << TP << endl;
    //cout << "[Testing] False positive : " << FP << endl;
    //cout << "[Testing] True negative : " << TN << endl;
    //cout << "[Testing] False negative : " << FN << endl;
    //cout << "[Testing] True positive rate : " << (int)(100*(float)TP/((float)TP+(float)FN)) << "\%" << endl;
    //cout << "[Testing] False positive rate : " << (int)(100*(float)FP/((float)TN+(float)FP)) << "\%" << endl;

    //cout << "[Saving] Saving the neural network" << endl;
	fann_save(ann,"saved-ann.net");
    //cout << "[Saving] Cleaning memory" << endl;
	for(std::vector<fann*>::iterator it = candidates_ann.begin(); it != candidates_ann.end(); ++it) {
		fann_destroy(*it);
	}

	// Crée le résultat
	VariationResult result;

	result.inputParameter = parameter;
	result.timeOfLearningProcess = total_epochs;
	result.truePositiveRate = (100.0f*(float)TP/((float)TP+(float)FN));
	result.falsePositiveRate = (100.0f*(float)FP/((float)TN+(float)FP));

	return result;
}

/** Affiche un set de paramètres **/
void PrintParam(map<string, float> param)
{
	for(pair<string, float> p : param) {
		cout << p.first << " : " << p.second << "\n";
	}
}

/* Interprète les sorties du réseau.
* Si N0 > N1, c'est un malware
* Si N0 < N1, c'est sain
* @param real_outputs	Sorties à interpréter
*/
bool interpret_outputs(fann_type *real_outputs){
	if(real_outputs[0] > real_outputs[1]) return true;
	else return false;
}

/* Convertit un float vers un booléen.
* true = 1
* false = 0
*/
bool floatToBool(float a){
	if(a < 0.5) return false;
	else		return true;
}

/** Compare des sorties.
* Renvoie true si toutes les sorties correspondent (avec un seuil arbitraire)
* Renvoie false si au moins une sortie ne correspond pas
* @param expected_outputs	Sorties attendues
* @param real_outputs		Sorties réelles
*/
bool compare_outputs(fann_type *expected_outputs, fann_type *real_outputs){
	bool success = true;
	for(int i = 0; i < 2; i++){
		if(floatToBool(expected_outputs[i]) != floatToBool(real_outputs[i]))
			success = false;
	}

	return success;
}

fann* selectMin(vector<fann*> arr){
	fann* min = arr.at(0);

	for(std::vector<fann*>::iterator it = arr.begin(); it != arr.end(); ++it) {
		if(fann_get_MSE(*it) < fann_get_MSE(min))
			min = *it;
	}

	return min;
}

bool FileExists(const string fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}


void WriteToFile(vector<VariationResult> learning_coeff_results, vector<VariationResult> hidden_neurons_results, vector<VariationResult> max_augmentation_results, const string filename)
{
    // Fichier pour stocker les différents résultats
    ofstream file;
    file.open(filename, ios::app); // Ouvre le fichier en écriture

    file << learning_coeff_results.size() << " " << hidden_neurons_results.size() << " " << max_augmentation_results.size() << "\n";
    for(int i = 0; i < learning_coeff_results.size(); i++){
        file << learning_coeff_results[i]; 
    }
    for(int i = 0; i < hidden_neurons_results.size(); i++){
        file << hidden_neurons_results[i]; 
    }
    for(int i = 0; i < max_augmentation_results.size(); i++){
        file << max_augmentation_results[i];
    }
}
void ReadFromFile(const string filename, vector<VariationResult>& learning_coeff_results, vector<VariationResult>& hidden_neurons_results, vector<VariationResult>& max_augmentation_results)
{
    ifstream file;
    string line;
    int size1, size2, size3;

    file.open(filename, ios::in); // Ouvre le fichier en lecture

    learning_coeff_results.clear();
    hidden_neurons_results.clear();
    max_augmentation_results.clear();

    // Récupère la taille de chaque plage de tests
    getline(file, line);
    stringstream(line) >> size1 >> size2 >> size3;

    for(int i = 0; i < size1; i++)
    {
    	getline(file, line);
        learning_coeff_results.push_back(VariationResult(line));
    }

    for(int i = 0; i < size2; i++)
    {
    	getline(file, line);
        hidden_neurons_results.push_back(VariationResult(line));
    }

    for(int i = 0; i < size3; i++)
    {
    	getline(file, line);
        max_augmentation_results.push_back(VariationResult(line));
    }

}

void ShowResults(vector<VariationResult> learning_coeff_results, vector<VariationResult> hidden_neurons_results, vector<VariationResult> max_augmentation_results)
{
	int  i, j;
    PLFLT* x[3]; PLFLT* y[3];
	PLFLT xMin = 0, yMin = 0, xMax = 1.0f, yMax = 1.0f;
	vector<string> parameterNames = {"Learning coefficient", "Hidden neurons number", "Max successive augmentation number"};

	// Trie chaque tableau
	sort(learning_coeff_results.begin(), learning_coeff_results.end(), [](VariationResult a, VariationResult b) {
		return a.falsePositiveRate < b.falsePositiveRate;
	});
	sort(hidden_neurons_results.begin(), hidden_neurons_results.end(), [](VariationResult a, VariationResult b) {
		return a.falsePositiveRate < b.falsePositiveRate;
	});
	sort(max_augmentation_results.begin(), max_augmentation_results.end(), [](VariationResult a, VariationResult b) {
		return a.falsePositiveRate < b.falsePositiveRate;
	});


	// Met les résultats dans des tableaux prêts à être affichés
	FillArrays(&(x[0]), &(y[0]), learning_coeff_results);
	FillArrays(&(x[1]), &(y[1]), hidden_neurons_results);
	FillArrays(&(x[2]), &(y[2]), max_augmentation_results);

    // Initialisation
	plsfnam(outputSVGFilename.c_str());
  	plsdev("svg");
	plinit();
	plssub( 2, 2 );
	
	// Valeurs minimum et maximum des axes du repère
	SetMinMax(xMin, xMax, yMin, yMax, learning_coeff_results);
	plenv(xMin, xMax, yMin, yMax, 0, 0);
	pllab( "False Positive Rate", "True Positive Rate", "Learning coefficient" );
	plcol0(2);
	plline(learning_coeff_results.size(), x[0], y[0]);
    plpoin(learning_coeff_results.size(), x[0], y[0], 3);

	// Et les labels
	//for(j = 0; j < learning_coeff_results.size(); j++)
	//	plptex(x[0][j], y[0][j], 0, 0, 0, (" " + to_string(learning_coeff_results[j].inputParameter.learning_coeff)).c_str());

	SetMinMax(xMin, xMax, yMin, yMax, hidden_neurons_results);
	plenv(xMin, xMax, yMin, yMax, 0, 0);
	pllab( "False Positive Rate", "True Positive Rate", "Number of hidden neurons" );
	plcol0(3);
    plline(hidden_neurons_results.size(), x[1], y[1]);
	plpoin(hidden_neurons_results.size(), x[1], y[1], 3);
	// Et les labels
	//for(j = 0; j < hidden_neurons_results.size(); j++)
	//	plptex(x[1][j], y[1][j], 0, 0, 0, (" " + to_string(hidden_neurons_results[j].inputParameter.hidden_neurons_nb)).c_str());

	SetMinMax(xMin, xMax, yMin, yMax, max_augmentation_results);
	plenv(xMin, xMax, yMin, yMax, 0, 0);
	pllab( "False Positive Rate", "True Positive Rate", "Max successive augmentation before early-stopping" );
	plcol0(4);
    plline(max_augmentation_results.size(), x[2], y[2]);
	plpoin(max_augmentation_results.size(), x[2], y[2], 3);
	// Et les labels
	//for(j = 0; j < max_augmentation_results.size(); j++)
    //  plptex(x[2][j], y[2][j], 0, 0, 0, (" " + to_string(max_augmentation_results[j].inputParameter.max_successive_augmentation_number)).c_str());


	// Libération de la mémoire et écriture du fichier test.svg
  	plend();

  	for(i = 0; i < 3; i++) free(x[i]);
  	for(i = 0; i < 3; i++) free(y[i]);
}

/** Remplie un tableau de PLFLT avec des données issues d'un tableau de VariationResult **/
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

/** Récupère les limites minimum et maximum en X et en Y d'un set de données **/
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

/** Traite les paramètres passés au programme.
* Vérifie l'existence des fichiers d'input.
* Renvoie true si les paramètres sont cohérents et valide, false sinon.
**/
bool ProcessParameters(int argc, char** argv)
{
	vector<string> parameters;
	vector<string>::iterator it;
	// Construit un vector à partir des paramètres
	for(int i = 0; i < argc; i++)
		parameters.push_back(argv[i]);

	// On a un paramètre
	if(argc > 1)
	{
		// Check for --help
		it = find(parameters.begin(), parameters.end(), "--help");
		if(it != parameters.end())
		{
			DisplayHelp(parameters[0]);
			parameters.erase(it);
			return false;
		}
	}
	// On a au moins deux paramètres
	if(argc > 2)
	{
		// On ajoute le -1 pour ne pas parcourir le dernier élément qui est le nom du fichier de sortie
		it = find(parameters.begin(), parameters.end()-1, "--input");
		if(it != parameters.end()-1)
		{
			inputDatasetFilename = *(it+1);
			parameters.erase(it, it+2);
		}

		it = find(parameters.begin(), parameters.end()-1, "--out-result");
		if(it != parameters.end()-1)
		{
			outputDataFilename = *(it+1);
			parameters.erase(it, it+2);
		}

		it = find(parameters.begin(), parameters.end()-1, "--out-svg");
		if(it != parameters.end()-1)
		{
			outputSVGFilename = *(it+1);
			parameters.erase(it, it+2);
		}
	}

	// Affiche une erreur si certains paramètres sont invalides
	if(parameters.size() > 1){
		cerr << "[Error] Paramètres invalides : ";
		for(auto p = parameters.begin()+1; p != parameters.end(); p++)
			cerr << *p << ", ";
		cerr << "\n";

		return false;
	}

	// Vérifie l'existence des fichiers d'entrée
	if(!FileExists(inputDatasetFilename)){
		cerr << "[Error] Unable to open input dataset " << inputDatasetFilename << "\n";
		return false;
	}

	// Si aucun paramètre n'est passé, affiche que l'aide est disponible
	if(argc == 1){
		cout << "[Information] Help available with : " << argv[0] << " --help\n";
	}

	return true;
}

void DisplayHelp(string programmeName)
{
	cout << "USAGE : " << programmeName << " --help\n"
		 << "           Displays this help.\n";
	cout << "USAGE : " << programmeName << " [--input <input_dataset_file>] [--out-result <output_result_file>] [--out-svg <output_svg_file>]\n"
		 << "           Process the given input file and produce a result file and a ROC curve. If a result file already exists, only produce the ROC curve.\n";
}