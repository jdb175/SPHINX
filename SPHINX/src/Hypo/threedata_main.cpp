/*
 * threedata-main.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: amukherji
 */

/**
 * Dempster-Shafer Model for Evidence-Theory
 * Abhishek Mukherji
  * > lab exercise, main file
 */

#include "dempstershafer.hpp"
#include "learningclassificator.hpp"
#include "csvreader.hpp"
#include "sleep.cpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <cmath>
#include <algorithm>    // std::max

//#define EXTERNAL_DATA
//#define IRIS_DATA

using namespace std;

string *frame_hypotheseses;

string frame_hypothesis_to_string_function(void* element);

void create_bar_for_emotion(string emotion, Evidence& combined);

void print_frame_stats(int frame_no,
				int eye_value,
				int mouth_value,
				int furrow_value,
				double eye_classification,
				double mouth_classification,
				double furrow_classification,
				Evidence& eye_evidence,
				Evidence& mouth_evidence,
				Evidence& furrow_evidence,
				Evidence& combined);

// hypotheses
string fear("fear");
string surprise("surprise");
string disdain("disdain");
string disgust("disgust");
string anger("anger");

int process_sleep_data(CSVReader csv);
int process_iris_data(CSVReader csv);

//int main(int argc, char** argv) {
int main(int argc, char** argv) {

	// create the CSV reader
		if(argc < 2) {
			cerr << "Provide .csv file with video data as argument!" << endl;
			return EXIT_FAILURE;
		}

		//CSVReader csv(argv[1]);

#ifdef EXTERNAL_DATA
#ifdef IRIS_DATA

		//call iris data wrapper and hypotheses solver.
		CSVReader csv("iris.csv");
		process_iris_data(csv);
#else // SLEEP_DATA

//call sleep data wrapper and hypotheses solver.
		CSVReader csv("sleep1.csv");
process_sleep_data(csv);
#endif
#else

	CSVReader csv("E_019b.csv");
	//CSVReader csv("test_data.csv");
	// create Dempster-Shafer universe
	DempsterShaferUniverse universe;
	universe.add_hypotheseses(&fear, &surprise, &disdain, &disgust, &anger, NULL);

	// create the feature classificator with online learning
	LearningClassificator classificator(0.05, 3);
	const int EYE_APERTURE = classificator.add_feature(18.0);
	const int MOUTH_OPENING = classificator.add_feature(23.0);
	const int FURROW_COUNT = classificator.add_feature(527.0);

	// create bitset representations of emotions sets to save performance while classifying video frames
	bitset<MAX_HYPOTHESESES> fear_and_surprise = universe.bitset_representation(&fear, &surprise, NULL);
	bitset<MAX_HYPOTHESESES> disdain_and_disgust = universe.bitset_representation(&disdain, &disgust, NULL);
	bitset<MAX_HYPOTHESESES> only_surprise = universe.bitset_representation(&surprise, NULL);
	bitset<MAX_HYPOTHESESES> anger_and_disgust = universe.bitset_representation(&anger, &disgust, NULL);
	bitset<MAX_HYPOTHESESES> only_anger = universe.bitset_representation(&anger, NULL);

	ofstream outFile;
		outFile.open("evidence_frames.csv", ios::app);
		outFile << "Hypotheses:, fear, surprise, disdain, disgust, anger" << endl;
		outFile << "Evidence:, eye_aperture, mouth_opening, furrow_count" << endl;
		outFile << "Frame, hypothesis, evidencetype, belief, plausability, maxclassification, ea_value, mo_value, fc_value, ea_classification, mo_classification, fc_classification" << endl;


	// classify the frames
	for(int i=0; i<csv.number_of_rows(); i++) {
		vector<int> frame = csv.row(i);

		// evidence for eye aperture
		Evidence eye_aperture = universe.add_evidence();
		double eye_aperture_classification = classificator.classify(EYE_APERTURE, frame.at(1));
		eye_aperture_classification *= 0.9; // we don't want 1.0 as mass
		if(eye_aperture_classification >= 0.0) {
			// large eye aperture
			eye_aperture.add_focal_set(eye_aperture_classification, fear_and_surprise);
		} else {
			// small eye aperture
			eye_aperture.add_focal_set(-eye_aperture_classification, disdain_and_disgust);
		}
		eye_aperture.add_omega_set();

		// evidence for mouth opening
		Evidence mouth_opening = universe.add_evidence();
		double mouth_opening_classification = classificator.classify(MOUTH_OPENING, frame.at(2));
		mouth_opening_classification *= 0.9; // we don't want 1.0 as mass
		if(mouth_opening_classification >= 0.0) {
			// large mouth opening
			mouth_opening.add_focal_set(mouth_opening_classification, only_surprise);
		} else {
			// small mouth opening
			mouth_opening.add_focal_set(-mouth_opening_classification, anger_and_disgust);
		}
		mouth_opening.add_omega_set();

		// evidence for furrow count
		Evidence furrow_count = universe.add_evidence();
		double furrow_count_classification = classificator.classify(FURROW_COUNT, frame.at(3));
		furrow_count_classification *= 0.9; // we don't want 1.0 as mass
		if(furrow_count_classification >= 0.0) {
			// many furrows
			furrow_count.add_focal_set(furrow_count_classification, fear_and_surprise);
		} else {
			// few furrows
			furrow_count.add_focal_set(-furrow_count_classification, only_anger);
		}
		furrow_count.add_omega_set();

		// combine the features
		Evidence combined_features = eye_aperture & mouth_opening & furrow_count;

		//writing to csv output file.

				string evidence_type;
				double max_classification = max(max(abs(eye_aperture_classification), abs(mouth_opening_classification)), abs(furrow_count_classification));
				if(max_classification == abs(eye_aperture_classification)){
					evidence_type = "eye_aperture";
					max_classification = eye_aperture_classification;
				}
				else if(max_classification == abs(mouth_opening_classification)){
					evidence_type = "mouth_opening";
					max_classification = mouth_opening_classification;
				}
				else {
					evidence_type = "furrow_count";
					max_classification = furrow_count_classification;
				}

				// find the most plausible emotion
				string* emotion = (string*) combined_features.best_match();
				// << "Frame , hypothesis, evidencetype, belief, plausability \n";
				outFile << i << ", " <<  *emotion << ", " <<  evidence_type << ", " << combined_features.belief(&(*emotion),NULL) << ", " << combined_features.plausability(&(*emotion),NULL) << ", " << max_classification << ", " << frame.at(1) << ", " << frame.at(2) << ", " << frame.at(3) << ", " << eye_aperture_classification << ", " << mouth_opening_classification << ", " << furrow_count_classification << endl;

				cout << "\n written to file: " << i << endl;


		// find the most plausible emotion
		print_frame_stats(i,
						frame.at(1),
						frame.at(2),
						frame.at(3),
						eye_aperture_classification,
						mouth_opening_classification,
						furrow_count_classification,
						eye_aperture,
						mouth_opening,
						furrow_count,
						combined_features);

		// frame could be classified here to remove the ugly print function
		//string* emotion = (string*) combined_features.best_match();
		//cout << "Frame: " << frame.at(0) << " classified as " << *emotion << "." << endl;
	}

	outFile.close();


#endif

	return EXIT_SUCCESS;
}

string frame_hypothesis_to_string_function(void* element) {
	string *s = (string*) element;
	return *s;
}

// very ugly code for debugging + demonstration below here
void print_frame_stats(int frame_no,
				int eye_value,
				int mouth_value,
				int furrow_value,
				double eye_classification,
				double mouth_classification,
				double furrow_classification,
				Evidence& eye_evidence,
				Evidence& mouth_evidence,
				Evidence& furrow_evidence,
				Evidence& combined)
{
	printf("---------------------------------\n");
	printf("### Frame: %03d ###\n", frame_no);
	printf("---------------------------------\n");
	printf("(-1.0: far below average, +1.0 far above average)\n");
	printf("Eye Aperture:  %3d -> %04.2f\n", eye_value, eye_classification);
	printf("Mouth Opening: %3d -> %04.2f\n", mouth_value, mouth_classification);
	printf("Furrow Count:  %3d -> %04.2f\n", furrow_value, furrow_classification);
	printf("---------------------------------\n");
	printf("(#: Belief, /: Plausability, -: nothing)\n");

	string bar("");
	int count;
	int belief;
	int add_plaus;
	const int BAR_LENGTH = 50;

	/*create_bar_for_emotion("fear", combined);

	create_bar_for_emotion("surprise", combined);

	create_bar_for_emotion("disdain", combined);

	create_bar_for_emotion("disgust", combined);

	create_bar_for_emotion("anger", combined);*/

	// fear
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&fear, NULL);
	add_plaus = (int) 100*combined.plausability(&fear, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Fear     | %s \n", bar.c_str());

	// surprise
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&surprise, NULL);
	add_plaus = (int) 100*combined.plausability(&surprise, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Surprise | %s \n", bar.c_str());

	// disdain
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&disdain, NULL);
	add_plaus = (int) 100*combined.plausability(&disdain, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Disdain  | %s \n", bar.c_str());

	// disgust
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&disgust, NULL);
	add_plaus = (int) 100*combined.plausability(&disgust, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Disgust  | %s \n", bar.c_str());

	// anger
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&anger, NULL);
	add_plaus = (int) 100*combined.plausability(&anger, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Anger    | %s \n", bar.c_str());

	printf("---------------------------------\n");
	cout << "classified as: " << frame_hypothesis_to_string_function(combined.best_match()) << endl;
	printf("---------------------------------\n");

	cout << endl;
}

void create_bar_for_emotion(string emotion, Evidence& combined){

	string bar("");
	int count;
	int belief;
	int add_plaus;
	const int BAR_LENGTH = 50;

	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&emotion, NULL);
	add_plaus = (int) 100*combined.plausability(&emotion, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("%s     | %s \n", emotion, bar.c_str());

}


/*
 * SLEEP APNEA DATASET
 */
string *sleep_hypotheseses;

string sleep_hypothesis_to_string_function(void* element);
void print_sleep_stats(int ID,
				double heart_rate,
				long chest_volume,
				long blood_oxygen,
				double HR_classification,
				double CV_classification,
				double BO_classification,
				Evidence& HR_evidence,
				Evidence& CV_evidence,
				Evidence& BO_evidence,
				Evidence& combined);

// hypotheseses
/*
 * congestive heart failure, sleep apnea, sudden cardiac death, myocardial infraction, and aging
 */
string congestive_heart_failure("congestive_heart_failure");
string sleep_apnea("sleep_apnea");
string cardiac_death("cardiac_death");
string myocardial_infraction("myocardial_infraction");
string aging("aging");

int process_sleep_data(CSVReader csv) {

	// create Dempster-Shafer universe
	DempsterShaferUniverse universe;
	universe.add_hypotheseses(&congestive_heart_failure, &sleep_apnea, &cardiac_death, &myocardial_infraction, &aging, NULL);

	// create the feature classificator with online learning
	LearningClassificator classificator(0.05, 3);
	const int HEART_RATE = classificator.add_feature(74.90);
	const int CHEST_VOLUME = classificator.add_feature(5379);
	const int BLOOD_OXYGEN = classificator.add_feature(6109);

	// create bitset representations of emotions sets to save performance while classifying video frames
	bitset<MAX_HYPOTHESESES> congestive_heart_failure_and_sleep_apnea = universe.bitset_representation(&congestive_heart_failure, &sleep_apnea, NULL);
	bitset<MAX_HYPOTHESESES> cardiac_death_and_myocardial_infraction = universe.bitset_representation(&cardiac_death, &myocardial_infraction, NULL);
	bitset<MAX_HYPOTHESESES> only_sleep_apnea = universe.bitset_representation(&sleep_apnea, NULL);
	bitset<MAX_HYPOTHESESES> aging_and_myocardial_infraction = universe.bitset_representation(&aging, &myocardial_infraction, NULL);
	bitset<MAX_HYPOTHESESES> only_aging = universe.bitset_representation(&aging, NULL);

	//writing out to a file.
	ofstream outFile;
			outFile.open("evidence_sleep.csv", ios::app);
			outFile << "Hypotheses:, congestive_heart_failure, sleep_apnea, cardiac_death, myocardial_infraction, aging" << endl;
			outFile << "Evidence:, heart_rate, chest_volume, blood_oxygen" << endl;
			outFile << "Frame, hypothesis, evidencetype, belief, plausability, maxclassification, hr_value, cv_value, bo_value, hr_classification, cv_classification, bo_classification" << endl;

		// classify the patients
	for(int i=0; i<csv.number_of_rows(); i++) {
		vector<int> frame = csv.row(i);

		// evidence for heart rate
		Evidence heart_rate = universe.add_evidence();
		double heart_rate_classification = classificator.classify(HEART_RATE, frame.at(0));
		heart_rate_classification *= 0.9; // we don't want 1.0 as mass
		if(heart_rate_classification >= 0.0) {
			// large heart rate
			heart_rate.add_focal_set(heart_rate_classification, congestive_heart_failure_and_sleep_apnea);
		} else {
			// small heart rate
			heart_rate.add_focal_set(-heart_rate_classification, cardiac_death_and_myocardial_infraction);
		}
		heart_rate.add_omega_set();

		// evidence for chest volume
		Evidence chest_volume = universe.add_evidence();
		double chest_volume_classification = classificator.classify(CHEST_VOLUME, frame.at(1));
		chest_volume_classification *= 0.9; // we don't want 1.0 as mass
		if(chest_volume_classification >= 0.0) {
			// large chest volume
			chest_volume.add_focal_set(chest_volume_classification, only_sleep_apnea);
		} else {
			// small chest volume
			chest_volume.add_focal_set(-chest_volume_classification, aging_and_myocardial_infraction);
		}
		chest_volume.add_omega_set();

		// evidence for blood oxygen
		Evidence blood_oxygen = universe.add_evidence();
		double blood_oxygen_classification = classificator.classify(BLOOD_OXYGEN, frame.at(2));
		blood_oxygen_classification *= 0.9; // we don't want 1.0 as mass
		if(blood_oxygen_classification >= 0.0) {
			// large blood oxygen
			blood_oxygen.add_focal_set(blood_oxygen_classification, congestive_heart_failure_and_sleep_apnea);
		} else {
			// small blood oxygen
			blood_oxygen.add_focal_set(-blood_oxygen_classification, only_aging);
		}
		blood_oxygen.add_omega_set();

		// combine the features
		Evidence combined_features = heart_rate & chest_volume & blood_oxygen;

		cout << "\n writing to file..... \n";
		//writing to csv output file.

				string evidence_type;
				double max_classification = max(max(abs(heart_rate_classification), abs(chest_volume_classification)), abs(blood_oxygen_classification));
				if(max_classification == abs(heart_rate_classification)){
					evidence_type = "heart_rate";
					max_classification = heart_rate_classification;
				}
				else if(max_classification == abs(chest_volume_classification)){
					evidence_type = "chest_volume";
					max_classification = chest_volume_classification;
				}
				else {
					evidence_type = "blood_oxygen";
					max_classification = blood_oxygen_classification;
				}

				// find the most plausible emotion
				string* hypothesis = (string*) combined_features.best_match();

				outFile << i << ", " <<  *hypothesis << ", " <<  evidence_type << ", " << combined_features.belief(&(*hypothesis),NULL) << ", " << combined_features.plausability(&(*hypothesis),NULL) << ", " << max_classification << ", " << frame.at(1) << ", " << frame.at(2) << ", " << frame.at(3) << ", " << heart_rate_classification << ", " << chest_volume_classification << ", " << blood_oxygen_classification << endl;

				cout << "\n written to file: " << i << endl;




		// find the most plausible emotion
		print_sleep_stats(i,
						frame.at(0),
						frame.at(1),
						frame.at(2),
						heart_rate_classification,
						chest_volume_classification,
						blood_oxygen_classification,
						heart_rate,
						chest_volume,
						blood_oxygen,
						combined_features);

		// frame could be classified here to remove the ugly print function
		//string* emotion = (string*) combined_features.best_match();
		//cout << "Frame: " << frame.at(0) << " classified as " << *emotion << "." << endl;
	}

	outFile.close();

	return EXIT_SUCCESS;
}

string sleep_hypothesis_to_string_function(void* element) {
	string *s = (string*) element;
	return *s;
}

// very ugly code for debugging + demonstration below here
void print_sleep_stats(int ID,
		double heart_rate,
		long chest_volume,
		long blood_oxygen,
		double HR_classification,
		double CV_classification,
		double BO_classification,
		Evidence& HR_evidence,
		Evidence& CV_evidence,
		Evidence& BO_evidence,
		Evidence& combined)
{
	printf("---------------------------------\n");
	printf("### PATIENT: %03d ###\n", ID);
	printf("---------------------------------\n");
	printf("(-1.0: far below average, +1.0 far above average)\n");
	printf("heart_rate:  %3d -> %04.2f\n", heart_rate, HR_classification);
	printf("chest_volume: %3d -> %04.2f\n", chest_volume, CV_classification);
	printf(" blood_oxygen:  %3d -> %04.2f\n", blood_oxygen, BO_classification);
	printf("---------------------------------\n");
	printf("(#: Belief, /: Plausability, -: nothing)\n");

	string bar("");
	int count;
	int belief;
	int add_plaus;
	const int BAR_LENGTH = 50;

	// congestive_heart_failure
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&congestive_heart_failure, NULL);
	add_plaus = (int) 100*combined.plausability(&congestive_heart_failure, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Congestive Heart Failure     | %s \n", bar.c_str());

	// sleep_apnea
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&sleep_apnea, NULL);
	add_plaus = (int) 100*combined.plausability(&sleep_apnea, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Sleep Apnea | %s \n", bar.c_str());

	// cardiac_death
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&cardiac_death, NULL);
	add_plaus = (int) 100*combined.plausability(&cardiac_death, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Cardiac Death  | %s \n", bar.c_str());

	// myocardial_infraction
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&myocardial_infraction, NULL);
	add_plaus = (int) 100*combined.plausability(&myocardial_infraction, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Myocardial Infraction  | %s \n", bar.c_str());

	// aging
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&aging, NULL);
	add_plaus = (int) 100*combined.plausability(&aging, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Aging   | %s \n", bar.c_str());

	printf("---------------------------------\n");
	cout << "classified as: " << sleep_hypothesis_to_string_function(combined.best_match()) << endl;
	printf("---------------------------------\n");

	cout << endl;
}

/**
 * IRIS DATASET
 */




/**
 * Dempster-Shafer Model for Evidence-Theory
 * Abhishek Mukherji
 *  DS Model, working with iris csv data.
*/
string *iris_hypotheseses;

string iris_hypothesis_to_string_function(void* element);
void print_iris_stats(int ID,
				double sepal_length,
				double sepal_width,
				double petal_length,
				double petal_width,
				double SL_classification,
				double SW_classification,
				double PL_classification,
				double PW_classification,
				Evidence& SL_evidence,
				Evidence& SW_evidence,
				Evidence& PL_evidence,
				Evidence& PW_evidence,
				Evidence& combined);

// hypotheseses
 // setosa, versicolor, virginica

string setosa("setosa");
string versicolor("versicolor");
string virginica("virginica");

int process_iris_data(CSVReader csv) {
	// create the CSV reader
	/*if(argc < 2) {
		cerr << "Provide .csv file with video data as argument!" << endl;
		return EXIT_FAILURE;
	}*/

	//CSVReader csv(argv[1]);
	//CSVReader csv("iris.csv");

	// create Dempster-Shafer universe
	DempsterShaferUniverse universe;
	universe.add_hypotheseses(&setosa, &versicolor, &virginica);

	// create the feature classificator with online learning
	LearningClassificator classificator(0.05, 4);
	const int SEPAL_LENGTH = classificator.add_feature(5.84);
	const int SEPAL_WIDTH = classificator.add_feature(3.05);
	const int PETAL_LENGTH = classificator.add_feature(3.75);
	const int PETAL_WIDTH = classificator.add_feature(1.198);

	// create bitset representations of hypotheses sets to save performance while classifying video frames
	bitset<MAX_HYPOTHESESES> only_setosa = universe.bitset_representation(&setosa, NULL);
	bitset<MAX_HYPOTHESESES> setosa_and_versicolor = universe.bitset_representation(&setosa, &versicolor, NULL);
	bitset<MAX_HYPOTHESESES> versicolor_and_virginica = universe.bitset_representation(&versicolor, &virginica, NULL);
	bitset<MAX_HYPOTHESESES> only_versicolor = universe.bitset_representation(&versicolor, NULL);
	bitset<MAX_HYPOTHESESES> setosa_and_virginica = universe.bitset_representation(&setosa, &virginica, NULL);
	bitset<MAX_HYPOTHESESES> only_virginica = universe.bitset_representation(&virginica, NULL);

	//writing out to a file.
		ofstream outFile;
				outFile.open("evidence_iris.csv", ios::app);
				outFile << "Hypotheses:, setosa, versicolor, virginica" << endl;
				outFile << "Evidence:, sepal_length, sepal_width, petal_length, petal_width" << endl;
				outFile << "Frame, hypothesis, evidencetype, belief, plausability, maxclassification, sl_value, sw_value, pl_value, pw_value, sl_classification, sw_classification, pl_classification, pw_classification" << endl;

	// classify the instances
	for(int i=0; i<csv.number_of_rows(); i++) {
		vector<int> frame = csv.row(i);

		// evidence for SEPAL_LENGTH
		Evidence sepal_length = universe.add_evidence();
		double sepal_length_classification = classificator.classify(SEPAL_LENGTH, frame.at(0));
		sepal_length_classification *= 0.9; // we don't want 1.0 as mass
		if(sepal_length_classification >= 0.0) {
			// large sepal_length
			sepal_length.add_focal_set(sepal_length_classification, setosa_and_versicolor);
		} else {
			// small sepal_length
			sepal_length.add_focal_set(-sepal_length_classification, versicolor_and_virginica);
		}
		sepal_length.add_omega_set();

		// evidence for sepal_width
		Evidence sepal_width = universe.add_evidence();
		double sepal_width_classification = classificator.classify(SEPAL_WIDTH, frame.at(1));
		sepal_width_classification *= 0.9; // we don't want 1.0 as mass
		if(sepal_width_classification >= 0.0) {
			// large sepal_width
			sepal_width.add_focal_set(sepal_width_classification, only_versicolor);
		} else {
			// small sepal_width
			sepal_width.add_focal_set(-sepal_width_classification, setosa_and_virginica);
		}
		sepal_width.add_omega_set();

		// evidence for petal_length
		Evidence petal_length = universe.add_evidence();
		double petal_length_classification = classificator.classify(PETAL_LENGTH, frame.at(2));
		petal_length_classification *= 0.9; // we don't want 1.0 as mass
		if(petal_length_classification >= 0.0) {
			// large petal_length
			petal_length.add_focal_set(petal_length_classification, setosa_and_versicolor);
		} else {
			// small petal_length
			petal_length.add_focal_set(-petal_length_classification, only_virginica);
		}
		petal_length.add_omega_set();

		// evidence for petal_width
		Evidence petal_width = universe.add_evidence();
		double petal_width_classification = classificator.classify(PETAL_WIDTH, frame.at(3));
		petal_width_classification *= 0.9; // we don't want 1.0 as mass
		if(petal_width_classification >= 0.0) {
			// large petal_width
			petal_width.add_focal_set(petal_width_classification, versicolor_and_virginica);
		} else {
			// small petal_width
			petal_width.add_focal_set(-petal_width_classification, only_setosa);
		}

		petal_width.add_omega_set();

		// combine the features
		Evidence combined_features = sepal_length & sepal_width & petal_length & petal_width;


		//writing to csv output file.

				string evidence_type;
				double max_classification = max(max(abs(sepal_length_classification), abs(sepal_width_classification)), max(abs(petal_length_classification), abs(petal_width_classification)));
				if(max_classification == abs(sepal_length_classification)){
					evidence_type = "sepal_length";
					max_classification = sepal_length_classification;
				}
				else if(max_classification == abs(sepal_width_classification)){
					evidence_type = "sepal_width";
					max_classification = sepal_width_classification;
				}
				else if(max_classification == abs(petal_length_classification)){
					evidence_type = "petal_length";
					max_classification = petal_length_classification;
				}
				else {
					evidence_type = "petal_width";
					max_classification = petal_width_classification;
				}

				// find the most plausible emotion
				string* hypothesis = (string*) combined_features.best_match();

				outFile << i << ", " <<  *hypothesis << ", " <<  evidence_type << ", " << combined_features.belief(&(*hypothesis),NULL) << ", " << combined_features.plausability(&(*hypothesis),NULL) << ", " << max_classification << ", " << frame.at(0) << ", " << frame.at(1) << ", " << frame.at(2) << ", " << frame.at(3) << ", " << sepal_length_classification << ", " << sepal_width_classification << ", " << petal_length_classification << ", " << petal_width_classification << endl;

				cout << "\n written to file: " << i << endl;

		// find the most plausible class
		print_iris_stats(i,
						frame.at(0),
						frame.at(1),
						frame.at(2),
						frame.at(3),
						sepal_length_classification,
						sepal_width_classification,
						petal_length_classification,
						petal_width_classification,
						sepal_length,
						sepal_width,
						petal_length,
						petal_width,
						combined_features);

		// frame could be classified here to remove the ugly print function
		//string* emotion = (string*) combined_features.best_match();
		//cout << "Frame: " << frame.at(0) << " classified as " << *emotion << "." << endl;
	}
	outFile.close();
	return EXIT_SUCCESS;
}

string iris_hypothesis_to_string_function(void* element) {
	string *s = (string*) element;
	return *s;
}

// very ugly code for debugging + demonstration below here
void print_iris_stats(int ID,
		double sepal_length,
		double sepal_width,
		double petal_length,
		double petal_width,
		double SL_classification,
		double SW_classification,
		double PL_classification,
		double PW_classification,
		Evidence& SL_evidence,
		Evidence& SW_evidence,
		Evidence& PL_evidence,
		Evidence& PW_evidence,
		Evidence& combined)
{
	printf("---------------------------------\n");
	printf("### FLOWER: %03d ###\n", ID);
	printf("---------------------------------\n");
	printf("(-1.0: far below average, +1.0 far above average)\n");
	printf("SEPAL_LENGTH:  %3d -> %04.2f\n", sepal_length, SL_classification);
	printf("SEPAL_WIDTH: %3d -> %04.2f\n", sepal_width, SW_classification);
	printf("PETAL_LENGTH:  %3d -> %04.2f\n", petal_length, PL_classification);
	printf("PETAL_WIDTH: %3d -> %04.2f\n", petal_width, PW_classification);
	printf("---------------------------------\n");
	printf("(#: Belief, /: Plausability, -: nothing)\n");

	string bar("");
	int count;
	int belief;
	int add_plaus;
	const int BAR_LENGTH = 50;

	// setosa
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&setosa, NULL);
	add_plaus = (int) 100*combined.plausability(&setosa, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Setosa     | %s \n", bar.c_str());

	// versicolor
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&versicolor, NULL);
	add_plaus = (int) 100*combined.plausability(&versicolor, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Versicolor | %s \n", bar.c_str());

	// virginica
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&virginica, NULL);
	add_plaus = (int) 100*combined.plausability(&virginica, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("/");
	for(;count<BAR_LENGTH;count++) bar.append("-");
	printf("Virginica | %s \n", bar.c_str());

	printf("---------------------------------\n");
	cout << "classified as: " << iris_hypothesis_to_string_function(combined.best_match()) << endl;
	printf("---------------------------------\n");

	cout << endl;
}
