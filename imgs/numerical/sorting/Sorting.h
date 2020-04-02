/** Bubble Sort Header
 *
 * \file Bubble_Sort/Sorting.h
 * \author Josh Carstens, ratking (jdc3498@rit.edu)
 * \date 29 Februrary 2020
 * \note help where the heck do they keep php on the CIS server
 */

template <typename T1>  // we only need one typename because the "ascending"
                        // value will never not be a boolean
                        auto Bubble(std::vector<T1> testvect, bool ascending) {
  // the swap lambda function as seen in Carl's example code
  auto swap = [](auto& element1, auto& element2) {
    auto tmp = element2;
    element2 = element1;
    element1 = tmp;
  };

  auto sortedvect = testvect;  // making a new vector to return with the same
                               // type/data as the input vector

  for (size_t idx = 0; idx < testvect.size() - 1; idx++) {
    // I don't quite remember how these nested for loops came to be, I think
    // it's something to do with iterating through passes
    for (size_t idx2 = 0; idx2 < testvect.size() - idx - 1; idx2++) {
      if (ascending == true) {  // the ascending case
        if (sortedvect[idx2] >
            sortedvect[idx2 + 1]) {  // if the current value is greater than the
                                     // next value
          swap(sortedvect[idx2],
               sortedvect[idx2 +
                          1]);  // switch the current value and the next value
        }
      } else {  // the descending case
        if (sortedvect[idx2] <
            sortedvect[idx2 + 1]) {  // if the current value is less than the
                                     // next value
          swap(sortedvect[idx2],
               sortedvect[idx2 +
                          1]);  // switch the current value and the next value
        }
      }
    }
  }
  return sortedvect;  // send the resulting vector back after the for loop has
                      // concluded
}