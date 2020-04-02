/** Bubble Sort test application
 *
 * \file Bubble_Sort/sort.cpp
 * \author Josh Carstens, man of Venezuelan descent (jdc3498@rit.edu)
 * \date 29 Februrary 2020
 * \note man i spent way too long on this
 */

// including libraries
#include <iostream>
#include <vector>
#include "imgs/numerical/sorting/Sorting.h"

using namespace std;

int main() {
  // our test values to sort, you can change the data type and values as needed
  vector<float> testvect = {11, 4, 1, 8, 3, 3, 7, 0, 9, 5};

  // the print_vector lambda function as seen in Carl's example code
  auto print_vector = [](auto& vector) {
    for (auto element : vector) {
      cout << element << endl;
    }
  };

  cout << "Original vector ..." << endl;
  print_vector(testvect);

  cout << "Sorted vector (ascending) ..." << endl;
  bool ascending = true;  // true = ascending, false = descending
  auto ascvect =
      Bubble(testvect, ascending);  // making a new vector and assigning the
                                    // resulting values to it
  print_vector(ascvect);  // printing the new vector

  cout << "Sorted vector (descending) ..." << endl;  // same stuff as above
  ascending = false;
  auto descvect = Bubble(testvect, ascending);
  print_vector(descvect);
}