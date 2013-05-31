/*
 * sleep.cpp
 *
 *  Created on: Feb 4, 2013
 *      Author: amukherji
 */



/**
 * Dempster-Shafer Model for Evidence-Theory
 * Abhishek Mukherji
 *  DS Model, working with sleep csv data.


#include "dempstershafer.hpp"
#include "learningclassificator.hpp"
#include "csvreader.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


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
 // congestive heart failure, sleep apnea, sudden cardiac death, myocardial infraction, and aging

string congestive_heart_failure("congestive_heart_failure");
string sleep_apnea("sleep_apnea");
string cardiac_death("cardiac_death");
string myocardial_infraction("myocardial_infraction");
string aging("aging");

int sleep_main(int argc, char** argv) {
	// create the CSV reader
	if(argc < 2) {
		cerr << "Provide .csv file with video data as argument!" << endl;
		return EXIT_FAILURE;
	}

	CSVReader csv(argv[1]);

	// create Dempster-Shafer universe
	DempsterShaferUniverse universe;
	universe.add_hypotheseses(&congestive_heart_failure, &sleep_apnea, &cardiac_death, &myocardial_infraction, &aging, NULL);

	// create the feature classificator with online learning
	LearningClassificator classificator(0.05, 3);
	const int HEART_RATE = classificator.add_feature(74.90166118);
	const int CHEST_VOLUME = classificator.add_feature(5379.201706);
	const int BLOOD_OXYGEN = classificator.add_feature(6109.356118);

	// create bitset representations of emotions sets to save performance while classifying video frames
	bitset<MAX_HYPOTHESESES> congestive_heart_failure_and_sleep_apnea = universe.bitset_representation(&congestive_heart_failure, &sleep_apnea, NULL);
	bitset<MAX_HYPOTHESESES> cardiac_death_and_myocardial_infraction = universe.bitset_representation(&cardiac_death, &myocardial_infraction, NULL);
	bitset<MAX_HYPOTHESESES> only_sleep_apnea = universe.bitset_representation(&sleep_apnea, NULL);
	bitset<MAX_HYPOTHESESES> aging_and_myocardial_infraction = universe.bitset_representation(&aging, &myocardial_infraction, NULL);
	bitset<MAX_HYPOTHESESES> only_aging = universe.bitset_representation(&aging, NULL);

	// classify the frames
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
	printf("Eye Aperture:  %3d -> %04.2f\n", heart_rate, HR_classification);
	printf("Mouth Opening: %3d -> %04.2f\n", chest_volume, CV_classification);
	printf("Furrow Count:  %3d -> %04.2f\n", blood_oxygen, BO_classification);
	printf("---------------------------------\n");
	printf("(#: Belief, -: Plausability, .: nothing)\n");

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
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("-");
	for(;count<BAR_LENGTH;count++) bar.append(".");
	printf("Fear     | %s \n", bar.c_str());

	// sleep_apnea
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&sleep_apnea, NULL);
	add_plaus = (int) 100*combined.plausability(&sleep_apnea, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("-");
	for(;count<BAR_LENGTH;count++) bar.append(".");
	printf("Surprise | %s \n", bar.c_str());

	// cardiac_death
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&cardiac_death, NULL);
	add_plaus = (int) 100*combined.plausability(&cardiac_death, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("-");
	for(;count<BAR_LENGTH;count++) bar.append(".");
	printf("Disdain  | %s \n", bar.c_str());

	// myocardial_infraction
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&myocardial_infraction, NULL);
	add_plaus = (int) 100*combined.plausability(&myocardial_infraction, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("-");
	for(;count<BAR_LENGTH;count++) bar.append(".");
	printf("Disgust  | %s \n", bar.c_str());

	// aging
	count = 0;
	bar = "";
	belief = (int) 100*combined.belief(&aging, NULL);
	add_plaus = (int) 100*combined.plausability(&aging, NULL) - belief;
	for(int i=0; i<belief/(100/BAR_LENGTH); i++, count++) bar.append("#");
	for(int i=0; i<add_plaus/(100/BAR_LENGTH); i++, count++) bar.append("-");
	for(;count<BAR_LENGTH;count++) bar.append(".");
	printf("Anger    | %s \n", bar.c_str());

	printf("---------------------------------\n");
	cout << "classified as: " << sleep_hypothesis_to_string_function(combined.best_match()) << endl;
	printf("---------------------------------\n");

	cout << endl;
} */

