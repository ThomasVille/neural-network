#include "NeuralNetwork.h"
#include <iostream>
using namespace std;

void InitializeData(string inputDatasetFilename, fann_train_data** train_set, fann_train_data** validation_set, fann_train_data** test_set)
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

VariationResult RunNeuralNetwork(ParameterSet parameter, fann_train_data* train_set, fann_train_data* validation_set, fann_train_data* test_set)
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

void PrintParam(ParameterSet param)
{
	for(pair<string, float> p : param) {
		cout << p.first << " : " << p.second << "\n";
	}
}

bool interpret_outputs(fann_type *real_outputs){
	if(real_outputs[0] > real_outputs[1]) return true;
	else return false;
}

bool floatToBool(float a){
	if(a < 0.5) return false;
	else		return true;
}

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