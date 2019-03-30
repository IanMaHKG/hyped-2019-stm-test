/*
 * Author: Iain Macpherson
 * Organisation: HYPED
 * Date: 11/03/2019
 * Description: Main class to read motor configuration data registers from a txt file
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <string>

#include "propulsion/file_reader.hpp"

namespace hyped {
namespace motor_control {
FileReader::FileReader(Logger& log)
  : log_(log)
{
}
void FileReader::readFileData(uint8_t* message, int32_t len)
{
  std::ifstream datafile;

  datafile.open("src/propulsion/configFiles/test_message.txt");

  if (!datafile.is_open()) {
    log_.INFO("FILE_READER", "Unable to open file");
    for (int i = 0; i < len; i++) message[i] = 0xFF;  // indicates error has occured.
  } else {
    std::string line;
    while (getline(datafile, line)) {
      if (line.empty());
      else if (line.front() == '#');
      else
        splitAndAddData(line, message, len);
    }
  }
  datafile.close();
}

void FileReader::splitAndAddData(std::string str, uint8_t* message, int32_t len)
{
  std::string word;
  std::string remains = str;
  uint32_t prevIndex = 0;
  for (int i = 0; i < len-1; i ++) {
    uint32_t index = str.find(' ');
    word = remains.substr(prevIndex, index);
    message[i] = (uint8_t) std::stoi(word, nullptr, 16);
    prevIndex = index+1;
    remains = remains.substr(prevIndex);
  }
}
}}  // namespace hyped::motor_control
