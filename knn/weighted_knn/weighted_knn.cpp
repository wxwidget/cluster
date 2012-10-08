#include <list>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "weighted_knn.h"
using namespace std;
KNNClassifier::KNNClassifier(int num_att, int num_class, int k , float learningR):
    mNumOfAttr(num_att), mNumOfClasses(num_class),
    mK(k), mLearningRate(learningR)
{
    mIsBEAttIncluded = new bool[mNumOfAttr - 1];
    mAttWeights = new double[mNumOfAttr - 1];
}
KNNClassifier::~KNNClassifier()
{ 
    if (mIsBEAttIncluded)
    {
        delete[] mIsBEAttIncluded;
        mIsBEAttIncluded = NULL;
    }
    if (mAttWeights)
    {
        delete[] mAttWeights;
        mAttWeights = NULL;
    }
}
//Backward Elimination
/*-----------------------------------------------------------------*/
/*                                                                 */
/*-----------------------------------------------------------------*/
/* filename - File from which the training/testing data is read    */
/* rlist - The data structure that holds the training/test data    */
/* rlistExamples - # of training/test examples                     */
/*-----------------------------------------------------------------*/
bool KNNClassifier::readData4File(char *filename, TrainingDataList *rlist,
                   int *rlistExamples)
{
    FILE *fp = NULL;
    int len = 0;
    char line[LINE_MAX + 1];
    int lineSize = LINE_MAX;
    TrainingExample *TEObj;
    int index = 0;
    int numExamples = 0;
    *rlistExamples = 0;
    line[0] = 0;
    if((fp = fopen(filename, "r")) == NULL)
    {
        cout << "Error in opening file." << endl;
        return false;
    }
    //Initialize weights to random .value()s
    srand(time(NULL));
    char *tmp;
    int tmpParams = 0; //mNumOfAttr;
    double cd = 0.0;
    /* Read the data file line by line */
    while((len = GetLine(line, lineSize, fp)) != 0)
    {
        TEObj = new TrainingExample(mNumOfAttr);
        tmp = strtok(line, ",");
        while(tmp != NULL)
        {
            cd = atof(tmp);
            TEObj->value()[tmpParams] = cd;
            tmpParams ++;
            tmp = strtok(NULL, ",");
            if(tmpParams == mNumOfAttr)
            {
                tmpParams = 0;
                cd = 0.0;
                line[0] = 0;
                numExamples ++;
                //Not using this normalization anymore.
                // N(y) = y/(1+y)
                // Doing normalization by standard deviation and mean
                //TEObj->NormalizeVals ();
                /* Generating random weights for instances. */
                /* These weights are used in instance WKNN  */
                double rno = (double)(rand() % 100 + 1);
                TEObj->weight = rno / 100;
                TEObj->index = index++;
                TEObj->isNearest2AtleastSome = false;
                break;
            }
        }
        rlist->insert(rlist->end(), *TEObj);
        delete TEObj;
    }
    /* Normalize .value()s using standard deviation */
    NormalizeByStandardDeviation(rlist, numExamples);
    *rlistExamples = numExamples;
    return true;
}
/*-----------------------------------------------------------------*/
/* trainList - List of training examples                           */
/* trainExamples - # of training examples                          */
/* testList - List of testing examples                             */
/* testExamples - # of testing examples                            */
/*-----------------------------------------------------------------*/
float  KNNClassifier::SimpleKNN(TrainingDataList *trainList, int trainExamples,
                TrainingDataList *testList, int testExamples)
{
    bool isInstanceWKNN = false;
    MODE mode			= TESTING;
    bool isBackwardElim = false;
    bool isAttrWKNN		= false;
    cout << endl << "Testing Simple KNN(Without Weights)." << endl;
    /* Test Simple KNN */
    int CCI = TestKNN(trainList, *testList, isInstanceWKNN, mode,
                      isBackwardElim, isAttrWKNN);
    float accuracy = (float)(((float)CCI / (float)testExamples) * 100);
    cout << "----------------------KNN----------------------" << endl;
    cout << "Number of Training Examples      # " << trainExamples << endl;
    cout << "Number of Testing Examples       # " << testExamples << endl;
    cout << "K used                           = " << mK << endl;
    cout << "Correctly Classified Instances   # " << CCI << endl;
    cout << "Incorrectly Classified Instances # " << testExamples - CCI << endl;
    cout << "Accuracy (%)                     = " << accuracy << endl;
    cout << "-----------------------------------------------" << endl << endl;
    return accuracy;
}
/*-----------------------------------------------------------------*/
/* trainList - List of training examples                           */
/* trainExamples - # of training examples                          */
/* testList - List of testing examples                             */
/* testExamples - # of testing examples                            */
/*-----------------------------------------------------------------*/
float  KNNClassifier::InstanceWKNN(TrainingDataList *trainList, int trainExamples,
                   TrainingDataList *testList, int testExamples)
{
    bool isInstanceWKNN		= true;
    bool isBackwardElim		= false;
    bool isAttrWKNN			= false;
    int no_of_iterations	= 25;
    int desiredAccuracy		= 85;
    cout << endl << "Starting Instance Weighted KNN.." << endl;
    /* Learn weights by cross validation (3 fold) on training set */
    float accuracy = CrossValidate(trainList, no_of_iterations,
                                   trainExamples, isAttrWKNN);
    /* Learn weights on the training set */
    LearnWeights(trainList, *trainList, no_of_iterations, trainExamples,
                 TRAINING, desiredAccuracy, isAttrWKNN);
    /* Test the trained weights on test set */
    int CCI = TestKNN(trainList, *testList, isInstanceWKNN,
                      TESTING, isBackwardElim, isAttrWKNN);
    accuracy = (float)(((float)CCI / (float)testExamples) * 100);
    cout << "-------Instance Weighted-KNN----------------------------" << endl;
    cout << "Number of Training Examples      # " << trainExamples << endl;
    cout << "Number of Testing Examples       # " << testExamples << endl;
    cout << "K used                           = " << mK << endl;
    cout << "Correctly Classified Instances   # " << CCI << endl;
    cout << "Incorrectly Classified Instances # " << testExamples - CCI << endl;
    cout << "Accuracy (%)                     = " << accuracy << endl;
    cout << "--------------------------------------------------------" << endl;
    return accuracy;
}
/*-----------------------------------------------------------------*/
/* trainList - List of training examples                           */
/* trainExamples - # of training examples                          */
/* testList - List of testing examples                             */
/* testExamples - # of testing examples                            */
/*-----------------------------------------------------------------*/
float KNNClassifier::BackwardElimination(TrainingDataList *trainList,
                          int trainExamples,
                          TrainingDataList *testList,
                          int testExamples)
{
    float accuracy = 0.0f;
    int CCI = 0;
    int noOfAttDeleted = 0;
    bool isInstanceWKNN		= false;
    MODE mode				= TESTING;
    bool isBackwardElim		= true;
    bool isAttrWKNN			= false;
    cout << endl << "Starting Backward Elimination.." << endl;
    cout << "--------------------Backward Elimination--------------------" << endl;
    /* Initially all the attributes will be included in KNN */
    BackwardEliminationInit();
    /* Test KNN with all the attributes */
    CCI = TestKNN(trainList, *trainList, isInstanceWKNN,
                  mode, isBackwardElim, isAttrWKNN);
    accuracy = (float)(((float)CCI / (float)trainExamples) * 100);
    cout << "Initial Accuracy on training data with " << mNumOfAttr - 1
         << " attributes = " << accuracy << "%" << endl;
    for(int i = 0; i < mNumOfAttr - 1; i++)
    {
        /* Delete one attribute at a time.                      */
        /* If the accuracy has decreased restore the attribute  */
        /* else let the attribute remain deleted.               */
        mIsBEAttIncluded[i] = false;
        CCI = TestKNN(trainList, *trainList, isInstanceWKNN,
                      mode, isBackwardElim, isAttrWKNN);
        float tmpAcc = (float)(((float)CCI / (float)trainExamples) * 100);
        if(tmpAcc >= accuracy)
        {
            accuracy = tmpAcc;
            noOfAttDeleted++;
        }
        else
            mIsBEAttIncluded[i] = true;
    }
    cout << "Backward Elimination achieves " << accuracy << "% accuracy with "
         << mNumOfAttr - 1 - noOfAttDeleted << " attributes \n on training data." << endl;
    /* Test KNN again with eliminated attributes on the test data. */
    CCI = TestKNN(trainList, *testList, isInstanceWKNN,
                  mode, isBackwardElim, isAttrWKNN);
    accuracy = (float)(((float)CCI / (float)testExamples) * 100);
    cout << "Number of Testing Examples       # " << testExamples << endl;
    cout << "K used                           = " << mK << endl;
    cout << "Correctly Classified Instances   # " << CCI << endl;
    cout << "Incorrectly Classified Instances # " << testExamples - CCI << endl;
    cout << "Backward Elimination achieves " << accuracy << "% accuracy with "
         << mNumOfAttr - 1 - noOfAttDeleted << " attributes \n on testing data." << endl;
    cout << "------------------------------------------------------------" << endl;
    return accuracy;
}
/*-----------------------------------------------------------------*/
/* trainList - List of training examples                           */
/* trainExamples - # of training examples                          */
/* testList - List of testing examples                             */
/* testExamples - # of testing examples                            */
/*-----------------------------------------------------------------*/
float KNNClassifier::AttributeWKNN(TrainingDataList *trainList, int trainExamples,
                    TrainingDataList *testList, int testExamples)
{
    int no_of_iterations	= 25;
    int desiredAccuracy		= 85;
    bool isAttrWKNN			= true;
    bool isInstanceWKNN		= false;
    bool isBackwardElim		= false;
    cout << endl << "Starting Attribute Weighted KNN.." << endl;
    /* Every attribute is associated with a weight. */
    /* Initialize the weights with random .value()s.   */
    InitAttWeights();
    /* Learn weights by cross validation (3 fold) on training set */
    float accuracy = CrossValidate(trainList, no_of_iterations,
                                   trainExamples, isAttrWKNN);
    /* Learn weights on the whole training set */
    no_of_iterations = 100;
    LearnWeights(trainList, *trainList, no_of_iterations, trainExamples,
                 TRAINING, desiredAccuracy, isAttrWKNN);
    /* Test the trained weights with the test set. */
    int CCI = TestKNN(trainList, *testList, isInstanceWKNN, TESTING,
                      isBackwardElim, isAttrWKNN);
    accuracy = (float)(((float)CCI / (float)testExamples) * 100);
    cout << "-------Attribute Weighted-KNN---------------------------" << endl;
    cout << "Number of Training Examples      # " << trainExamples << endl;
    cout << "Number of Testing Examples       # " << testExamples << endl;
    cout << "K used                           = " << mK << endl;
    cout << "Correctly Classified Instances   # " << CCI << endl;
    cout << "Incorrectly Classified Instances # " << testExamples - CCI << endl;
    cout << "Accuracy (%)                     = " << accuracy << endl;
    cout << "--------------------------------------------------------" << endl;
    return accuracy;
}
/*------------------------------------------------------------------*/
/* Test Simple KNN, Instance WeightedKNN, Attribute WeightedKNN and */
/* KNN by Backward Elimination)                                     */
/* tlist - training list                                            */
/* data - testing list                                              */
/* isInstanceWeighted - Instance WKNN                               */
/* mode - Training/Testing                                          */
/* isBackwardElimination - KNN by Backward Elimination              */
/* isAttWKNN - Attribute WKNN                                       */
/*------------------------------------------------------------------*/
int KNNClassifier::TestKNN(TrainingDataList *tlist, TrainingDataList data,
            bool isInstanceWeighted, MODE mode,
            bool isBackwardElimination, bool isAttWKNN)
{
    int correctlyClassifiedInstances = 0;
    TrainingDataList::iterator testIter;
    uint32_t index[mK];
    for(testIter = data.begin(); testIter != data.end(); ++testIter)
    {
        TrainingExample&  tmpTestObj = *testIter;
        /* Predict the class for the query point */
        int predictedClass = PredictByKNN(tlist, tmpTestObj.value(),
                                          isInstanceWeighted,
                                          index, mode, isBackwardElimination,
                                          isAttWKNN);
        /* Count the number of correctly classified instances */
        if(((int)(tmpTestObj.value()[mNumOfAttr - 1])) == predictedClass)
            correctlyClassifiedInstances ++;
    }
    return correctlyClassifiedInstances;
}
/*------------------------------------------------------------------*/
/* Initialize attribute weights to random .value()s                    */
/*------------------------------------------------------------------*/
void KNNClassifier::InitAttWeights()
{
    srand(time(NULL));
    for(int i = 0; i < mNumOfAttr - 1; i++)
        mAttWeights[i] = ((double)(rand() % 100 + 1)) / 100;
}
/*------------------------------------------------------------------*/
/* Normalize .value()s by using mean and standard deviation            */
/*------------------------------------------------------------------*/
void KNNClassifier::NormalizeByStandardDeviation(TrainingDataList *trainList,
                                  int trainExamples)
{
    for(int i = 0; i < mNumOfAttr - 1; i++)
    {
        /* Find the mean */
        double mean = 0.0;
        TrainingDataList::iterator iter;
        for(iter = trainList->begin(); iter != trainList->end(); ++iter)
        {
            mean += iter->value()[i];
        }
        mean = (mean / (double)trainExamples);
        /* Find the deviation */
        double deviation = 0.0;
        for(iter = trainList->begin(); iter != trainList->end(); ++iter)
        {
            double val = mean - iter->value()[i];
            deviation += val * val;
        }
        deviation = (deviation / (double)trainExamples);
        deviation = sqrt(deviation);
        /* Normalize the .value()s */
        for(iter = trainList->begin(); iter != trainList->end(); ++iter)
        {
            double val = (iter->value()[i] - mean) / deviation;
            iter->value()[i] = val;
        }
    }
}
/*------------------------------------------------*/
/* Initialize the array to include all attributes */
/*------------------------------------------------*/
void KNNClassifier::BackwardEliminationInit()
{
    for(int i = 0; i < mNumOfAttr - 1; i++)
        mIsBEAttIncluded[i] = true;
}
/*---------------------------------------------------------------------------*/
/* Predict Class by KNN (Simple KNN, Instance WeightedKNN,                   */
/*                       Attribute WeightedKNN, KNN by Backward Elimination) */
/* tlist - training list                                                     */
/* query - query instance to be classified                                   */
/* isWeightedKNN - Instance Weighted KNN                                     */
/* index - Indices of the K nearest neighbors will be stored in index array  */
/* mode - Specifies whether we are training or testing                       */
/* isBE - Backward Elimination                                               */
/* isAttWeightedKNN - Attribute WeightedKNN                                  */
/*---------------------------------------------------------------------------*/
int KNNClassifier::PredictByKNN(TrainingDataList *tlist, double *query,
                 bool isWeightedKNN, uint32_t *index, MODE mode,
                 bool isBE, bool isAttWeightedKNN)
{
    double distance = 0.0;
    TrainingDataList::iterator iter;
    TrainingDataList elistWithD;
    if(!elistWithD.empty())
        elistWithD.clear();
    /* If we are in for backward elimination or attribute WKNN */
    /* then Instance WKNN has to be false                      */
    if(isBE || isAttWeightedKNN)
        isWeightedKNN = false;
    /* Calculate the distance of the query */
    /* point from all training instances   */
    /* using the euclidean distance        */
    for(iter = tlist->begin(); iter != tlist->end(); ++iter)
    {
        TrainingExample& tmpObj = *iter;
        distance = 0.0;
        for(int j = 0; j < mNumOfAttr - 1; j++)
        {
            if(isAttWeightedKNN)
            {
                distance += (abs(query[j] - tmpObj.value()[j]) *
                             abs(query[j] - tmpObj.value()[j])) *
                            (mAttWeights[j] * mAttWeights[j]);
            }
            else
            {
                if(isBE)
                {
                    if(mIsBEAttIncluded[j])
                        distance += abs(query[j] - tmpObj.value()[j]) *
                                    abs(query[j] - tmpObj.value()[j]);
                }
                else
                {
                    if(isWeightedKNN)
                    {
                        if(mIsBEAttIncluded[j])
                            distance += abs(query[j] - tmpObj.value()[j]) *
                                        abs(query[j] - tmpObj.value()[j]);
                    }
                    else
                        distance += abs(query[j] - tmpObj.value()[j]) *
                                    abs(query[j] - tmpObj.value()[j]);
                }
            }
        }
        distance = sqrt(distance);
        /* If the distance is zero then set it to some high .value() */
        /* since it the query point itself                        */
        if((int)(distance * 1000) == 0)
            distance = 999999999999999.9;
        tmpObj.distance = distance;
        elistWithD.insert(elistWithD.end(), tmpObj);
    }
    /* Sort the points on distance in ascending order */
    elistWithD.sort(compare);
    if(!isWeightedKNN)
    {
        /* Simple KNN, Attribute Weighted KNN, Backward Elimination */
        int classCount[mNumOfClasses];
        for(int i = 0; i < mNumOfClasses; i++)
            classCount[i] = 0;
        int knn = mK;
        for(iter = elistWithD.begin(); iter != elistWithD.end(); ++iter)
        {
            /* Calculate how the K nearest neighbors are classified */
            TrainingExample& tmpObj = *iter;
            classCount[(int)tmpObj.value()[mNumOfAttr - 1]]++;
            knn--;
            if(!knn)
                break;
        }
        int maxClass = 0;
        int maxCount = 0;
        /* Find the class represented maximum number of times */
        /* among the k neighbors                              */
        for(int i = 0; i < mNumOfClasses; i++)
        {
            if(classCount[i] > maxCount)
            {
                maxClass = i;
                maxCount = classCount[i];
            }
        }
        return maxClass;
    }
    else
    {
        /*Instance Weighted KNN */
        int knn = mK;
        double pclass = 0.0;
        int i = 0;
        int maxClass = 0;
        /* Calulate the class by multiplying the K nearest  */
        /* neighbor weights by the class .value()s.            */
        for(iter = elistWithD.begin(); iter != elistWithD.end(); ++iter)
        {
            TrainingExample& tmpObj = *iter;
            /* While testing, do not use the training examples   */
            /* which were not near any instance during training. */
            if(mode == TESTING && tmpObj.isNearest2AtleastSome == false)
                continue;
            pclass += tmpObj.weight * tmpObj.value()[mNumOfAttr - 1];
            /* Store the indices of the K nearest neighbors */
            index[i++] = tmpObj.index;
            knn--;
            if(!knn)
                break;
        }
        /* Mark an instance near when it is near to some query instance */
        for(iter = tlist->begin(); iter != tlist->end(); ++iter)
        {
            TrainingExample& tmpObj = *iter;
            for(int i = 0; i < mK; i++)
            {
                if(index[i] == tmpObj.index)
                {
                    iter->isNearest2AtleastSome = true;
                    break;
                }
            }
        }
        maxClass = (int)pclass;
        return maxClass;
    }
}
/*---------------------------------------------------------------------------*/
/* 3 Fold Cross Validation                                                   */
/* data - training data                                                      */
/* iterations - learn weights for number of iterations on a given cross fold */
/* numExamples - # of examples in the training set                           */
/* isAttWKNN = true (Learn attribute weights)                                */
/*           = false (Learn instance weights)                                */
/*---------------------------------------------------------------------------*/
float KNNClassifier::LearnWeights(TrainingDataList *tlist,  //Training Data
                   TrainingDataList data,   //Testing Data
                   //Train data = Test data
                   int iterations,				   //Learn for # of iterations
                   int numExamples,			   //# of examples
                   MODE mode,					   // mode = TRAINING
                   int desiredAccuracy,		   //Desired accuracy in %
                   bool isAttWeightedKNN)	   //Attribute or Instance Weighted
{
    TrainingDataList::iterator iter;
    uint32_t index[mK];
    int CCI;
    float accuracy = 0.0;
    /* Learn weights for number of iterations  */
    /* or till the desired accuracy is reached */
    int tmp = 0;
    while(tmp != iterations)
    {
        for(iter = data.begin(); iter != data.end(); ++iter)
        {
            TrainingExample& TEObj = *iter;
            /* Predict the class */
            int predictedClass = PredictByKNN(tlist, TEObj.value(), true, index,
                                              mode, false, isAttWeightedKNN);
            int actualClass = (int)(TEObj.value()[mNumOfAttr - 1]);
            if(actualClass != predictedClass)
            {
                /* Calculate the error */
                double error = actualClass - predictedClass;
                /* Adjust weights by Gradient Descent */
                AdjustWeightsByGradientDescent(TEObj.value(), tlist, error,
                                               index, isAttWeightedKNN);
            }
        }
        CCI = TestKNN(tlist, data, true, mode, false, false);
        accuracy = (float)(((float)CCI / (float)numExamples) * 100);
        tmp++;
        int iacc = (int)accuracy;
        if(iacc > desiredAccuracy)
            break;
    }
    return accuracy;
}
/*---------------------------------------------------------------------------*/
/* 3 Fold Cross Validation                                                   */
/* data - training data                                                      */
/* iterations - learn weights for number of iterations on a given cross fold */
/* numExamples - # of examples in the training set                           */
/* isAttWKNN = true (Learn attribute weights)                                */
/*           = false (Learn instance weights)                                */
/*---------------------------------------------------------------------------*/
float KNNClassifier::CrossValidate(TrainingDataList *data, int iterations,
                    int numExamples, bool isAttWKNN)
{
    /* Divide the data into three equal sets N1,N2,N3   */
    /* First Cross Fold:  Training = N1+N2 Testing = N3 */
    /* Second Cross Fold: Training = N2+N3 Testing = N1 */
    /* Third Cross Fold:  Training = N1+N3 Testing = N2 */
    int N = numExamples / 3;
    int first = N;
    int second = 2 * N;
    int i = 0;
    double accuracy = 0.0;
    TrainingDataList trainList, testList;
    TrainingDataList::iterator iter;
    /* first cross fold validation */
    i = 0;
    for(iter = data->begin(); iter != data->end(); ++iter)
    {
        TrainingExample& TEObj = *iter;
        if(i < second)
            trainList.insert(trainList.end(), TEObj);
        else
            testList.insert(testList.end(), TEObj);
        i++;
    }
    /* Learn Weights (Test and adjust by gradient descent) */
    accuracy = LearnWeights(&trainList, testList, iterations,
                            second + 1, TRAINING, 95, isAttWKNN);
    data->clear();
    for(iter = trainList.begin(); iter != trainList.end(); ++iter)
    {
        data->insert(data->end(), *iter);
    }
    for(iter = testList.begin(); iter != testList.end(); ++iter)
    {
        data->insert(data->end(), *iter);
    }
    /* second cross fold validation */
    trainList.clear();
    testList.clear();
    i = 0;
    for(iter = data->begin(); iter != data->end(); ++iter)
    {
        if(i >= first)
            trainList.insert(trainList.end(), *iter);
        else
            testList.insert(testList.end(), *iter);
        i++;
    }
    /* Learn Weights (Test and adjust by gradient descent) */
    accuracy = LearnWeights(&trainList, testList, iterations,
                            numExamples - first + 1, TRAINING, 95, isAttWKNN);
    data->clear();
    for(iter = testList.begin(); iter != testList.end(); ++iter)
    {
        data->insert(data->end(), *iter);
    }
    for(iter = trainList.begin(); iter != trainList.end(); ++iter)
    {
        data->insert(data->end(), *iter);
    }
    /* third fold cross validation */
    trainList.clear();
    testList.clear();
    i = 0;
    for(iter = data->begin(); iter != data->end(); ++iter)
    {
        TrainingExample& TEObj = *iter;
        if(i < first)
            trainList.insert(trainList.end(), TEObj);
        else if(i >= first && i < second)
            testList.insert(testList.end(), TEObj);
        else if(i >= second && i < numExamples)
            trainList.insert(trainList.end(), TEObj);
        i++;
    }
    /* Learn Weights (Test and adjust by gradient descent) */
    accuracy = LearnWeights(&trainList, testList, iterations,
                            first + numExamples - second + 1, TRAINING, 95, isAttWKNN);
    data->clear();
    for(iter = trainList.begin(); iter != trainList.end(); ++iter)
    {
        data->insert(data->end(), *iter);
    }
    for(iter = testList.begin(); iter != testList.end(); ++iter)
    {
        data->insert(data->end(), *iter);
    }
    return (float)accuracy;
}
/*-------------------------------------------------------------------------*/
/* q.value() - .value() of a particular attribute                                */
/* tlist  - List of training examples                                      */
/* error  - Error in prediction of class for an instance                   */
/* index  - indices corresponding to the K nearest neighbors(instances)    */
/* isAttWeightedKNN = true (Adjust attribute weights)                      */
/*                  = false (Adjust instance weights)                      */
/*-------------------------------------------------------------------------*/
void KNNClassifier::AdjustWeightsByGradientDescent(double *q,
                                    TrainingDataList *tlist,
                                    double error, uint32_t *index,
                                    bool isAttWeightedKNN)
{
    if(isAttWeightedKNN)
    {
        /* Adjust attribute weights by gradient descent*/
        for(int i = 0; i < mNumOfAttr - 1; i++)
        {
            mAttWeights[i] = mAttWeights[i] + mLearningRate * error * q[i];
        }
    }
    else
    {
        /* Adjust instance weights by gradient descent.     */
        /* We adjust the weights of the K nearest neighbors */
        /* for a query instance                             */
        TrainingDataList::iterator iter;
        int k = mK;
        for(iter = tlist->begin(); iter != tlist->end(); ++iter)
        {
            TrainingExample& TEObj = *iter;
            for(int i = 0; i < mK; i++)
            {
                if(TEObj.index == index[i])
                {
                    double wt = TEObj.weight;
                    wt = wt + mLearningRate * error;
                    iter->weight = wt;
                    k--;
                    break;
                }
            }
            if(k == 0)
                break;
        }
    }
}
