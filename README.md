# neural-network
Authors : Slim BOUAOUINA - Thomas VILLE

Learn to detect malware with neural network.

The main goal of this project is to find the best parameters for a neural network which purpose is to detect malicious software.

## Features
 * Learns to detect malware based on n-gram analysis.
 * Takes only 20% of the given dataset to speed up the process.
 * Tries several NN parameters based on predefined ranges of values.
 * Creates a SVG image containing a ROC curve showing the efficiency of the NN for the different parameters.

## Dependencies
 * FANN >= 2.2
 * plplot