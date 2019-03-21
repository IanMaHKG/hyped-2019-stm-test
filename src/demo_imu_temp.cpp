/*
 * Author: Gregory Dayao
 * Organisation: HYPED
 * Date: 20/03/19
 * Description: Demo for MPU9250 sensor
 *
 *    Copyright 2018 HYPED
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

#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/system.hpp"
#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"

using hyped::sensors::Imu;
using hyped::utils::Logger;
using hyped::utils::concurrent::Thread;

int main(int argc, char* argv[])
{
  hyped::utils::System::parseArgs(argc, argv);
  Logger log(true, 0);
  Imu imu(log, 66, 0x08);
  int imu_temp_;
  Thread::sleep(200);
  log.INFO("TEST-Imu", "Imu instance successfully created");
  for (int j = 0; j < 100; j++) {
    imu.getTemperature(&imu_temp_);
    log.INFO("TEST-Imu", "Temperature %d degrees Celcius", imu_temp_);
    Thread::sleep(100);
  }
 	return 0;
}
