#include "propulsion/file_reader.hpp"
#include "utils/system.hpp"
#include "utils/logger.hpp"
#include "propulsion/controller.hpp"

using hyped::motor_control::FileReader;
using hyped::utils::System;
using hyped::utils::Logger;
using hyped::motor_control::ControllerMessage;

int main(int argc, char* argv[]) {
  System::parseArgs(argc, argv);
  Logger log(true, 0);

  ControllerMessage messages[2];
  if (FileReader::readFileData(messages, "src/propulsion/configFiles/test_message.txt")) {
    for (int n = 0; n < 2; n++) {
      log.INFO("DEMO", messages[n].logger_output, 0);
      for (int i = 0; i < 8; i++) {
        log.INFO("DEMO", "%d", messages[n].message_data[i]);
      }
    }
  }
}
