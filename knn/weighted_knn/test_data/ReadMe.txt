Instructions

1. Download the zip file.

2. On unzipping, you will get a Visual Studio 2005 solution for Windows.

3. Open the solution in VS 2005.

4. Build the solution.

5. I have 4 UCI datasets on which you can run the algorithms on.

 Datasets: 
	Heart

	Hill Valley

	Wine

	Heart-1


 Algorithms 
	Simple KNN

	Attribute Weighted KNN

	KNN by Backward Elimination

	Instance Weighted KNN


6. By default, my code will run all the four algorithms on the HEART Dataset.

7. The data sets are in csv format.

8. Whenever you change the dataset you will have to modify code in following places: 
	TrainingExample.h 
	#define NO_OF_ATT xx      //xx is the total number of attributes in the data set including the class attribute
	#define NO_OF_CLASSES yy  //yy is the number of classes


	WeightedKNN.cpp 
		main() 
		Change the Training Data File Name 
			readData4File ("train_heart.txt", &elist, &numTrainingExamples)

		Change the Testing Data File Name 
			readData4File ("test_heart.txt", &elist, &numTrainingExamples)

9. If you do not want to run all the algorithms, comment the corresponing lines: 
	WeightedKNN.cpp 
	main() 
		SimpleKNN (&elist, numTrainingExamples, &qlist, numTestingExamples);

		AttributeWKNN (&elist, numTrainingExamples, &qlist, numTestingExamples);

		BackwardElimination (&elist, numTrainingExamples, &qlist, numTestingExamples);

		InstanceWKNN (&elist, numTrainingExamples, &qlist, numTestingExamples);


10. You can experiment with K and learning rate by changing the following variables: 
	TrainingExample.h 

	#define K 3
	#define LEARNING_RATE 0.005


11. About the dataset: 
a) The 4 datasets (heart, hill valley, wine and heart-1) are provided. 
   You just need to uncomment the lines from TrainingExample.h and WeightedKNN.cpp.

b) All the attribute values must be real or integers.

c) The class values must be integers in the format: 0,1,2....
