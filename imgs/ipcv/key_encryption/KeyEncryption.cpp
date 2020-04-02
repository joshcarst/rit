/** Implementation file for key encryption
 *
 *  \file imgs/ipcv/key_encryption/KeyEncryption.cpp
 *  \author Carl Salvaggio, Ph.D. (salvaggio@cis.rit.edu)
 *  \date 10 January 2020
 */

#include "imgs/ipcv/key_encryption/KeyEncryption.h"

namespace ipcv {

std::string Encrypt(const std::string& key, const std::string& str) {
  std::string encrypted_str;
  encrypted_str.reserve(str.size());

  for (size_t idx = 0; idx < str.length(); idx++) {
    encrypted_str.push_back(
        ((str.at(idx) - 32) + (key.at(idx % key.length()) - 32)) % (127 - 32) +
        32);
  }

  return encrypted_str;
}

std::string Decrypt(const std::string& key, const std::string& str) {
  std::string decrypted_str;
  decrypted_str.reserve(str.size());

  for (size_t idx = 0; idx < str.length(); idx++) {
    if ((str.at(idx) - 32) - (key.at(idx % key.length()) - 32) < 0) {
      decrypted_str.push_back(((str.at(idx) - 32) -
                               (key.at(idx % key.length()) - 32) + (127 - 32) +
                               32));
    } else {
      decrypted_str.push_back(
          ((str.at(idx) - 32) - (key.at(idx % key.length()) - 32)) + 32);
    }
  }

  return decrypted_str;
}
}
