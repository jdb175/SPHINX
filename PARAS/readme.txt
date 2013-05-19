1. apriori232 is an c++ implementation of Apriori algorithm which I took from http://www.cs.bme.hu/~bodon/en/.
I use this code to generate frequent itemsets and association rules. And store the result into a file for PARAS. 


Example:
Now I am using mushroom dataset(data is included in folder "apriori232/mushroom.dat"),I wrote the command in file, e.g."apriori232/Apriori12.dat".

the template is: apriori <data file> <output file> <support> <confidence>
e.g. apriori mushroom.dat mushroom-output-00805.txt 0.08 0.5


2. apriori-v7 is the one I created(I should rename it as PARAS). It takes the files generated from step 1 as input, parse it, and process it as the PARAS paper indicated. 
For step 2, there are several parameters that you should change in the code when you work with different datasets.

A. input the file
e.g. I use mushroom dataset, I took the file(frequent itemsets and association rules) generated from step 1 as input as follows: 

under main function:
apriori->loadRules("c:\\cpp\\apriori232\\mushroom-output-03506.txt");


B. change the number of instance for each dataset.

e.g. I use mushroom dataset.so  I set numInstance=8124.

under function "InitializeInstanceFields()"
numInstance = 8124;//chess=3196, mushroom=8124,T100k=100000, T5000k=5000000, webdocs=1692082


(The numInstance of other datasets is also listed in comment.)

C. change the number of attributes.

e.g. mushroom dataset has 119 attributes. so I set 119 as follows:

under function "loadDomain()"
for (int i = 1; i <= 119; i++)//change "119" to the number of attributes of the test dataset, T10=1000, T5000k=1000, chess=76,mushroom=119,webdocs=5267656


D. set minSupport and minConfidence

e.g.I set the minSupport =0.4, minConfidence =0.6.

Under function "loadRules(String^ fileName)"
double dMinSupport = 0.4;
double dMinConfidence = 0.6;