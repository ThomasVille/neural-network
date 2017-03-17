// 5971 inputs
// 25 hidden
// 2 outputs
#include <fann.h>
#include <iostream>
using namespace std;

#define MALWARE true
#define SAIN 	false

bool compare_outputs(fann_type *expected_outputs, fann_type *real_outputs);
bool interpret_outputs(fann_type *real_outputs);

int main() {
	fann* ann = NULL;							// Réseau de neurones
    fann_train_data* total_train_set = NULL; 	// Set de données total
    fann_train_data* data_subset = NULL;		// Set de 20% des données
    fann_train_data* train_set = NULL;			// Set d'entrainement (80%)
    fann_train_data* test_set = NULL;			// Set de test (20%)
    int dataset_size = 0;						// Taille du set de données original
    int data_subset_size = 0;					// Taille du set de données à 20%
    int train_set_size = 0;						// Taille du set de données d'entrainement (80% des 20%)
    int test_set_size = 0;						// Taille du set de données de test (20% des 20%)
    int successfuly_classified_number = 0;		// Nombre d'éléments correctement classifiés

    /* Vrai positif, Faux positif, Faux négatif, Vrai négatif */
    float TP = 0.0f, FP = 0.0f, FN = 0.0f, TN = 0.0f;
    
    cout << "[Initializing] Neural network | 1. Testing ANN performance" << endl;
    ann = fann_create_standard(3,5971,25,2);
    fann_set_learning_rate(ann, 0.25f);
    fann_randomize_weights(ann, -0.77f, 0.77f);
	
    cout << "[Initializing]" << endl;
    cout << "[Initializing] Opening train file" << endl;
	total_train_set = fann_read_train_from_file("android-features.data");

    dataset_size = fann_length_train_data(total_train_set);
	cout << "[Initializing] Total dataset size : " << dataset_size << endl;

    cout << "[Initializing] Shuffling train data" << endl;
    fann_shuffle_train_data(total_train_set);

    data_subset_size = dataset_size*0.2;
    data_subset = fann_subset_train_data(total_train_set, 0, data_subset_size);
    data_subset_size = fann_length_train_data(data_subset);
    cout << "[Initializing] Subsetting 20\% of total dataset : " << data_subset_size << " elements" << endl;

    cout << "[Initializing] Subsetting 80\% of the train subset to build the train set" << endl;
    train_set = fann_subset_train_data(data_subset, 0, data_subset_size*0.8f);
    train_set_size = fann_length_train_data(train_set);

    cout << "[Initializing] Subsetting 20\% of the train subset to build the test set" << endl;
    test_set = fann_subset_train_data(data_subset, data_subset_size*0.8f, data_subset_size*0.2f);
    test_set_size = fann_length_train_data(test_set);
    
    cout << "[Training] Training on train set" << endl;
    fann_train_on_data(ann, train_set, 1000, 10, 0.001);

    cout << "[Testing] Running on test set" << endl;
    
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

    cout << "[Testing] Accuracy of the ANN : " << (int)(100*successfuly_classified_number/train_set_size) << "\%" << endl;
    cout << "[Testing] Vrai positif : " << TP << endl;
    cout << "[Testing] Faux positif : " << FP << endl;
    cout << "[Testing] Vrai négatif : " << TN << endl;
    cout << "[Testing] Faux négatif : " << FN << endl;
    cout << "[Testing] Taux de vrai positif : " << (int)(100*(float)TP/((float)TP+(float)FN)) << "\%" << endl;
    cout << "[Testing] Taux de faux positif : " << (int)(100*(float)FP/((float)TN+(float)FP)) << "\%" << endl;


    cout << "[Saving] Saving the neural network" << endl;
	fann_save(ann,"saved-ann.net");
	fann_destroy(ann);
	
	return 0;
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