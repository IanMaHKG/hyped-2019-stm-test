/*
 * Author: Jack Horsburgh
 * Organisation: HYPED
 * Date: 19/06/18
 * Description: IMU manager for getting IMU data from around the pod and pushes to data struct
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


#include "sensors/imu_manager.hpp"

#include "sensors/imu.hpp"
#include "data/data.hpp"
#include "utils/timer.hpp"

namespace hyped {

using data::Data;
using data::Sensors;
using utils::System;
using data::NavigationVector;
using data::SensorCalibration;

namespace sensors {
ImuManager::ImuManager(Logger& log /*, ImuManager::DataArray *imu*/)
    : ImuManagerInterface(log),
      sys_(System::getSystem()),
      // sensors_imu_(imu),
      data_(Data::getInstance()),
      chip_select_ {20},
      // chip_select_ {117, 125, 123, 111, 112, 110, 20},
      is_calibrated_(false),
      calib_counter_(0)
{
  old_timestamp_ = utils::Timer::getTimeMicros();

  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k1MHz);
  for (int i = 0; i < data::Sensors::kNumImus; i++) {   // creates new real IMU objects
    imu_[i] = new Imu(log, chip_select_[i], 0x08);
  }
  utils::io::SPI::getInstance().setClock(utils::io::SPI::Clock::k20MHz);

  log_.INFO("IMU-MANAGER", "imu data has been initialised");
}

void ImuManager::run()
{
  // TODO(Greg): talk to navigation if they need calibration data, move to after calibration in run()
  // // collect calibration data
  // while (!is_calibrated_) {
  //   for (int i = 0; i < data::Sensors::kNumImus; i++) {
  //     ImuData imu;
  //     imu_[i]->getData(&imu);
  //     if (imu.operational) {
  //       stats_[i].update(imu.acc);
  //     }
  //   }
  //   calib_counter_++;
  //   if (calib_counter_ >= 100) is_calibrated_ = true;
  // }
  // log_.INFO("IMU-MANAGER", "Calibration complete!");

  // // send calibration data to data struct, is_calibrated_ needs to be true
  // if (updated()) {
  //   SensorCalibration sensor_calibration_data;
  //   sensor_calibration_data.imu_variance  = getCalibrationData();
  //   data_.setCalibrationData(sensor_calibration_data);
  // }
  // Thread::yield();
  
  // collect real data while system is running
  while (sys_.running_) {
    for (int i = 0; i < data::Sensors::kNumImus; i++) {
      log_.DBG("Debug-manager", "0");
      // DataPoint<array<ImuData, Sensors::kNumImus>> imu;
      imu_[i]->getData(&(sensors_imu_->value[i]));    // TODO(Greg): segfault, is sensors_imu_ initialised?
      log_.DBG("Debug-manager", "1");
    }
    resetTimestamp();
    log_.DBG("Debug-manager", "2");
    sensors_imu_->timestamp = utils::Timer::getTimeMicros();
    data_.setSensorsImuData(*sensors_imu_);
    log_.DBG("Debug-manager", "3");
  }
}

ImuManager::CalibrationArray ImuManager::getCalibrationData()       // TODO(Greg): ask nav if needed
{
  while (!is_calibrated_) {
    Thread::yield();
  }
  for (int i = 0; i < data::Sensors::kNumImus; i++) {
    imu_calibrations_[i] = stats_[i].getVariance();
  }
  return imu_calibrations_;
}

bool ImuManager::updated()
{
  if (old_timestamp_ != sensors_imu_->timestamp) {
    return true;
  }
  return false;
}

void ImuManager::resetTimestamp()
{
  old_timestamp_ = sensors_imu_->timestamp;
}
}}  // namespace hyped::sensors
