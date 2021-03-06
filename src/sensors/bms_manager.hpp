/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 20/06/18
 * Description:
 * BMS manager for getting battery data and pushes to data struct.
 * Checks whether batteries are in range and enters emergency state if fails.
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

#ifndef SENSORS_BMS_MANAGER_HPP_
#define SENSORS_BMS_MANAGER_HPP_

#include <cstdint>

#include "sensors/manager_interface.hpp"

#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "sensors/interface.hpp"
#include "utils/system.hpp"

namespace hyped {

using utils::concurrent::Thread;
using utils::Logger;
using hyped::data::BatteryData;

namespace sensors {

class BmsManager: public ManagerInterface  {
  typedef array<BatteryData, data::Batteries::kNumLPBatteries> BatteriesLP;
  typedef array<BatteryData, data::Batteries::kNumHPBatteries> BatteriesHP;
 public:
  explicit BmsManager(Logger& log);
  void run()                override;

 private:
  BMSInterface*   bms_[data::Batteries::kNumLPBatteries+data::Batteries::kNumHPBatteries];
  utils::System&  sys_;

  /**
   * @brief needs to be references because run() passes directly to data struct
   *
   */
  data::Data&     data_;
  data::Batteries batteries_;
  bool batteriesInRange();
};

}}  // namespace hyped::sensors

#endif  // SENSORS_BMS_MANAGER_HPP_
