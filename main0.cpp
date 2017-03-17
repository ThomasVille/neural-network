 
// 5971 inputs
// 2000000000000000 hidden
// 2 outputs
#include <fann.h>
#include <iostream>
using namespace std;

int main() {
    cout << "Neural network" << endl;
	struct fann* ann = fann_create_standard(3,5971,25,2);
    fann_set_learning_rate(ann, 0.25f);
    fann_randomize_weights(ann, -0.77f, 0.77f);
	
	fann_train_on_file(ann,"android-features.data",1000,10,0.001);
	fann_save(ann,"saved-ann.net");
	fann_destroy(ann);
	
	return 0;
}
