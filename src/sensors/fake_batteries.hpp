/*
 * Author: Jack Horsburgh and Gregory Dayo
 * Organisation: HYPED
 * Date: 6/04/19
 * Description: Main class for fake.
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

#ifndef SENSORS_FAKE_BATTERIES_HPP_
#define SENSORS_FAKE_BATTERIES_HPP_

#include <string>
#include <vector>

#include "utils/concurrent/thread.hpp"
#include "data/data.hpp"
#include "sensors/interface.hpp"

namespace hyped {

using utils::Logger;
using data::Data;

namespace sensors {


class FakeBatteries : public BMSInterface {
  typedef array<array<uint16_t, 6>, 4> BatteryCases;
  typedef array<uint16_t, 6> BatteryInformation;

 public:
  FakeBatteries(Logger& log, bool is_lp, bool is_fail);
  void getData(BatteryData* battery) override;
  bool isOnline() override;

 private:
  Data& data_;
  utils::Logger& log_;
  void checkFailure();
  void updateBatteryData();

  BatteryInformation lp_failure_;
  BatteryInformation lp_success_;    // TODO(Greg): adjust values
  BatteryInformation hp_failure_;
  BatteryInformation hp_success_;    // TODO(Greg): adjust values
  // different success and fail cases
  BatteryCases cases_;

  bool is_lp_;
  bool is_fail_;
  int case_index_;

  uint16_t voltage_;
  int16_t current_;
  uint8_t charge_;
  int8_t temperature_;
  uint16_t low_voltage_cell_;
  uint16_t high_voltage_cell_;

  uint64_t acc_start_time_;
  bool acc_started_;
  uint64_t failure_time_;
  bool failure_happened_;
};
}}    // namespace hyped::sensors

#endif  // SENSORS_FAKE_BATTERIES_HPP_
