#include <list>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "weighted_knn.h"
using namespace std;
/* Using a list to store the training and testing examples. */
//The Heart Data Set
//#define NO_OF_ATT 14
//#define NO_OF_CLASSES 2

//The Hill Valley Data Set
//#define NO_OF_ATT 101
//#define NO_OF_CLASSES 2

//Wine Data Set
//#define NO_OF_ATT 14
//#define NO_OF_CLASSES 3

//Heart-1 Data Set
//#define NO_OF_ATT 14
//#define NO_OF_CLASSES 5
int main(int argc, char** argv[])
{
    /* Training Examples */
    TrainingDataList elist;
    int numTrainingExamples = 0;
    /* Testing Examples */
    TrainingDataList qlist;
    int numTestingExamples = 0;
    KNNClassifier classifer(14, 2);
    //Read Data from File
    if(!classifer.readData4File("train_heart.txt", &elist, &numTrainingExamples))
        //if(!readData4File ("train_hillValley.txt", &elist, &numTrainingExamples))
        //if(!readData4File ("train_wine.txt", &elist, &numTrainingExamples))
        //if(!readData4File ("train_heart-1.txt", &elist, &numTrainingExamples))
    {
        cout << "Error in Reading Training Data File." << endl;
        return 0;
    }
    cout << "Training Data Uploaded." << endl;

    if(!classifer.readData4File("test_heart.txt", &qlist, &numTestingExamples))
        //if(!readData4File ("test_hillValley.txt", &qlist, &numTestingExamples))
        //if(!readData4File ("test_wine.txt", &qlist, &numTestingExamples))
        //if(!readData4File ("test_heart-1.txt", &qlist, &numTestingExamples))
    {
        cout << "Error in Reading Testing Data File." << endl;
        return 0;
    }
    cout << "Testing Data Uploaded." << endl;

    /* Simple KNN */
    classifer.SimpleKNN(&elist, numTrainingExamples, &qlist, numTestingExamples);
    /* Attribute Weighted KNN with gradient descent and cross validation */
    classifer.AttributeWKNN(&elist, numTrainingExamples, &qlist, numTestingExamples);
    /* KNN with Backward Elimination */
    classifer.BackwardElimination(&elist, numTrainingExamples,
                        &qlist, numTestingExamples);
    /* Instance Weighted KNN with gradient descent and cross validation */
    classifer.InstanceWKNN(&elist, numTrainingExamples, &qlist, numTestingExamples);

    return 0;
}
