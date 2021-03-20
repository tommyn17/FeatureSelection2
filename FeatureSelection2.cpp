// FeatureSelection2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <cmath>
#include <fstream>
#include <istream>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

vector<int> classes;
vector<vector<double>> feature_data;
vector<int> best_features;
double highest_accuracy;

void get_data(string filename) {
    ifstream ifs(filename);
    string data_line;
    int row = 0;

    while (getline(ifs, data_line)) { //read line by line and parse data

        istringstream iss(data_line);
        vector<string> tokens{ istream_iterator<string>{iss}, istream_iterator<string>{} };
        feature_data.resize(tokens.size() - 1);
        for (int i = 0; i < tokens.size(); i++) {

            if (i == 0) {
                classes.push_back(stoi(tokens.at(i)));
            }
            else {
                feature_data.at(i - 1).push_back(stod(tokens.at(i)));
            }

        }
        row++;
    }

    ifs.close();
    
}

bool intersects(int feature_to_add, vector<int> current_set_of_features) { //helper function to find features already in current set
    for (int i = 0; i < current_set_of_features.size(); i++) {
        if (feature_to_add == current_set_of_features.at(i)) {
            return true;
        }
    }
    return false;
    

}

double leave_one_out_cross_validation(vector<vector<double>> feature_data, vector<int> classes, vector<int> current_set, int feature_to_add) {
    vector<vector<double>> temp;
    bool add = false;
    for (int i = 0; i < feature_data.size(); i++) {
        for (int j = 0; j < current_set.size(); j++) {
            if (i == current_set.at(j)) { // add current features to local copy
                add = true;
                temp.push_back(feature_data.at(i));
            }
        }
        if (i == feature_to_add) { // add new feature to local copy
            add = true;
            temp.push_back(feature_data.at(i));
        }
        if (!add) { // feature is not being used so set to 0
            vector<double> all_zero(feature_data.at(i).size(), 0.0);
            temp.push_back(all_zero);

        }
        add = false;
    }


    int num_correct = 0;
    for (int i = 0; i < classes.size(); i++) {
        vector<double> obj_to_classify;
        for (int j = 0; j < feature_data.size(); j++) { //initialize the object we are classifying
            obj_to_classify.push_back(temp.at(j).at(i));
        }
        int label_object = classes.at(i);
        double nearest_neighbor_distance = numeric_limits<double>::max();  //initialize distance, label , and location to max value
        int nearest_neighbor_location = numeric_limits<int>::max();
        int nearest_neighbor_label = numeric_limits<int>::max();
        for (int k = 0; k < classes.size(); k++) {
            if (k != i) {
                double sum = 0.0;
                for (int l = 0; l < obj_to_classify.size(); l++) { //calculate euclidean distance
                    sum += pow(obj_to_classify.at(l) - temp.at(l).at(k), 2);
                }
                double distance = sqrt(sum);


                if (distance < nearest_neighbor_distance) { //distance is closer so replace with new distance
                    nearest_neighbor_distance = distance;
                    nearest_neighbor_location = k;
                    nearest_neighbor_label = classes.at(nearest_neighbor_location);
                }
            }
        }
        if (label_object == nearest_neighbor_label) { //object is correctly classified
            num_correct++;
        }

    }
 
    double accuracy = static_cast<double>(num_correct) / static_cast<double>(classes.size());

    return accuracy;
}

double remove_cross_validation(vector<vector<double>> feature_data, vector<int> classes, vector<int> current_set, int feature_to_remove) {
    vector<vector<double>> temp;
    vector<double> all_zero(feature_data.at(0).size(), 0.0);
    bool add = false;
    for (int i = 0; i < feature_data.size(); i++) {
        for (int j = 0; j < current_set.size(); j++) {
            if (i == current_set.at(j)) { // add current features to local copy
                add = true;
                temp.push_back(feature_data.at(i));
            }
        }
        if (i == feature_to_remove) { // remove feature from local copy by replacing with 0
            add = true;
            temp.push_back(all_zero);
        }
        if (!add) { // features are not in current_set so they are set to 0
            temp.push_back(all_zero);

        }
        add = false;
    }



    int num_correct = 0;
    for (int i = 0; i < classes.size(); i++) {
        vector<double> obj_to_classify;
        for (int j = 0; j < feature_data.size(); j++) { //initialize object
            obj_to_classify.push_back(temp.at(j).at(i));
        }
        int label_object = classes.at(i);
        double nearest_neighbor_distance = numeric_limits<double>::max();
        int nearest_neighbor_location = numeric_limits<int>::max();
        int nearest_neighbor_label = numeric_limits<int>::max();
        for (int k = 0; k < classes.size(); k++) {
            if (k != i) {
                double sum = 0.0;
                for (int l = 0; l < obj_to_classify.size(); l++) { //calculate euclidean distance
                    sum += pow(obj_to_classify.at(l) - temp.at(l).at(k), 2);
                }
                double distance = sqrt(sum);


                if (distance < nearest_neighbor_distance) {
                    nearest_neighbor_distance = distance;
                    nearest_neighbor_location = k;
                    nearest_neighbor_label = classes.at(nearest_neighbor_location);
                }
            }
        }
      
        if (label_object == nearest_neighbor_label) {
            num_correct++;
        }

    }

    double accuracy = static_cast<double>(num_correct) / static_cast<double>(classes.size());

    return accuracy;
}


void forward_search(vector<vector<double>> feature_data) {
    vector<int> current_set_of_features;
    for (int i = 0; i < feature_data.size(); i++) {
        cout << "On level " << i + 1 << " of the search tree" << endl;
        cout << endl;
        int feature_to_add = numeric_limits<int>::max();
        double best_accuracy = 0.0;
        for (int j = 0; j < feature_data.size(); j++) { // iterate through all features and finds the best feature
            if (!intersects(j, current_set_of_features)) {
                cout << "--Considering adding feature " << j + 1 << endl;
                double accuracy = leave_one_out_cross_validation(feature_data, classes, current_set_of_features, j);
                if (accuracy > best_accuracy) {
                    best_accuracy = accuracy;
                    feature_to_add = j;
                }
            }
        }
        current_set_of_features.push_back(feature_to_add);
        cout << "Feature Set: {";
        for (int j = 0; j < current_set_of_features.size() - 1; j++) {
            cout << current_set_of_features.at(j) + 1 << ", ";
        }
        cout << current_set_of_features.at(current_set_of_features.size() - 1) + 1 << "}" <<endl;
        if (best_accuracy > highest_accuracy) {
            highest_accuracy = best_accuracy;
            best_features = current_set_of_features;
        }
        cout << setprecision(3) << best_accuracy * 100 << "%" << endl;
        cout << "On level " << i + 1 << " I added feature " << feature_to_add  + 1 << " to current set" << endl;
    }
}

void backward_elimination(vector<vector<double>> feature_data) {
    vector<int> current_set_of_features;
    for (int i = 0; i < feature_data.size(); i++) {
        current_set_of_features.push_back(i);
    }
    for (int i = feature_data.size() - 1; i > 0; i--) {
        cout << "On level " << i + 1 << " of the search tree" << endl;
        cout << endl;
        int feature_to_remove = numeric_limits<int>::max(); // 
        double best_accuracy = 0.0;
        for (int j = 0; j < feature_data.size(); j++) { // Iterates through all features and removes the worst features
            if (intersects(j, current_set_of_features)) {
                cout << "--Considering removing feature " << j + 1 << endl;
                double accuracy = remove_cross_validation(feature_data, classes, current_set_of_features, j);
                if (accuracy > best_accuracy) {
                    best_accuracy = accuracy;
                    feature_to_remove = j;
                }
            }
        }
        vector<int> temp;
        for (int x = 0; x < current_set_of_features.size(); x++) {
            if (current_set_of_features.at(x) != feature_to_remove) {
                temp.push_back(current_set_of_features.at(x));
            }
        }

        current_set_of_features = temp;
        cout << "Feature Set: {";
        for (int j = 0; j < current_set_of_features.size() - 1; j++) {
            cout << current_set_of_features.at(j) + 1 << ", ";
        }
        cout << current_set_of_features.at(current_set_of_features.size() - 1) + 1 << "}" << endl;
        if (best_accuracy > highest_accuracy) {
            highest_accuracy = best_accuracy;
            best_features = current_set_of_features;
        }
        cout << setprecision(3) << best_accuracy * 100 << "%" << endl;
        cout << "On level " << i + 1 << " I removed feature " << feature_to_remove + 1 << " from current set" << endl;
    }
}


int main()
{
    int selection;
    string file;
    vector<int> features;
    //vector<int> feat = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };




    cout << "Welcome to Tommy's Feature Selection Algorithm." << endl;
    cout << "Type in the name of the file to test: ";
    cin >> file;
    get_data(file);
    cout << "Type the number of the algorithm you want to run." << endl;
    cout << "   1) Forward Selection" << endl;
    cout << "   2) Backward Elimination" << endl;
    cin >> selection;
    
    if (selection == 1) { //Forward Selection
        cout << "Accuracy with empty set: " << setprecision(3) << leave_one_out_cross_validation(feature_data, classes, features, -1) * 100 << "%" << endl;
        forward_search(feature_data);
    }
    if (selection == 2) { //Backward Elimination
        cout << "Accuracy with empty set: " << setprecision(3) << leave_one_out_cross_validation(feature_data, classes, features, -1) * 100 << "%" << endl;
        backward_elimination(feature_data);
    }

    cout << endl;
    cout << "Best feature Set: {";
    for (int j = 0; j < best_features.size() - 1; j++) {
        cout << best_features.at(j) + 1 << ", ";
    }
    cout << best_features.at(best_features.size() - 1) + 1 << "} with Accuracy: " << setprecision(3) << highest_accuracy * 100 << "%" << endl;
     
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
