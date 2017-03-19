# neural-network
Authors : Slim BOUAOUINA - Thomas VILLE

Learn to detect malware with neural network.

The main goal of this project is to find the best parameters for a neural network which purpose is to detect malicious software.

## Features
 * Learns to detect malware based on n-gram analysis.
 * Takes only 20% of the given dataset to speed up the process.
 * Tries several NN parameters based on predefined ranges of values.
 * Creates an SVG image containing a ROC curve showing the efficiency of the NN for the different parameters.
 * Detects inconsistent parameters.
 * Parallel processing of the test cases.

### Resource usage
![Screenshot of RAM and CPU usage](ResourceUsage.png)

### ROC curve
Here is an example of a ROC curve produced by the program :

![ROC curve example](roc-curve.png)

## Dependencies
 * FANN >= 2.2
 * plplot

## Documentation
### How to add a new test parameter
