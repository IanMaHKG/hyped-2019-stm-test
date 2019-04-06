/*
 * Author: Neil McBlane
 * Organisation: HYPED
 * Date: 05/04/2019
 * Description: Main file for navigation class.
 *
 *    Copyright 2019 HYPED
 *    Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 *    except in compliance with the License. You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software distributed under
 *    the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *    either express or implied. See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "navigation/navigation.hpp"

namespace hyped {
namespace navigation {

Navigation::Navigation(Logger& log)
         : log_(log),
           imu_manager_(log),
           data_(Data::getInstance()),
           sensor_readings_(nullptr),
           acceleration_(0., {0., 0., 0.}),
           velocity_(0., {0., 0., 0.}),
           distance_(0., {0., 0., 0.}),
           acceleration_integrator_(&velocity_),
           velocity_integrator_(&distance_)
{
  // Initialise sensors
  imu_manager_.start();

  calibrateGravity();
}

// TODO(Neil/Lukas): do this more smartly?
NavigationType Navigation::getAcceleration() const
{
  return acceleration_.value[0];
}

// TODO(Neil/Lukas): do this more smartly?
NavigationType Navigation::getVelocity() const
{
  return velocity_.value[0];
}

// TODO(Neil/Lukas): do this more smartly?
NavigationType Navigation::getDistance() const
{
  return distance_.value[0];
}

// TODO(Neil): check this is still the same, why is it so?
NavigationType Navigation::getEmergencyBrakingDistance() const
{
  // TODO(Neil): Account for actuation delay and/or communication latency?
  return velocity_.value[0]*velocity_.value[0] / kEmergencyDeceleration;
}

// TODO(Neil): check this is still the same, why is it so?
NavigationType Navigation::getBrakingDistance() const
{
  // A polynomial fit for the braking distance at a specific (normalised) velocity, where
  // kCoeffSlow for < 50m/s and kCoeffFast for > 50m/s because kCoeffFast is inaccurate
  // at < 10m/s but they both agree between ~10 and ~50m/s.
  static constexpr std::array<NavigationType, 16> kCoeffSlow = {
       136.3132, 158.9403,  63.6093, -35.4894, -149.2755, 152.6967, 502.5464, -218.4689,
      -779.534,   95.7285, 621.1013,  50.4598, -245.099,  -54.5,     38.0642,   12.3548};
  static constexpr std::array<NavigationType, 16> kCoeffFast = {
       258.6,  299.2,  115.2, -104.7, -260.9, 488.5, 940.8, -808.5, -1551.9,  551.7,
      1315.7,  -61.4, -551.4,  -84.5,   90.7,  26.2};

  NavigationType braking_distance = 2.0;
  NavigationType var = 1.0;
  if (getVelocity() < 50.0) {
    NavigationType norm_v = (getVelocity() - 30.0079) / 17.2325;
    for (unsigned int i = 0; i < kCoeffSlow.size(); ++i) {
      braking_distance += kCoeffSlow[i] * var;
      var *= norm_v;
    }
  } else {
    NavigationType norm_v = (getVelocity() - 41.4985) / 23.5436;
    for (unsigned int i = 0; i < kCoeffFast.size(); ++i) {
      braking_distance += kCoeffFast[i] * var;
      var *= norm_v;
    }
  }
  return braking_distance;
}

// TODO(Neil): add delay between samples?
void Navigation::calibrateGravity()
{
  log_.INFO("NAV", "Calibrating gravity");
  std::array<OnlineStatistics<NavigationVector>, data::Sensors::kNumImus> online_array;
  // Average each sensor over specified number of readings
  for (int i = 0; i < kNumCalibrationQueries; ++i) {
    sensor_readings = data_.getSensorsImuData();
    for (int j = 0; j < data::Sensors::kNumImus; ++j) {
      online_array[j].update(sensor_readings.value[j].acc);
    }
  }
  for (int j = 0; j < kNumImus; ++j) {
    gravity_calibration_[j] = online_array[j].getMean();
    log_.INFO("NAV",
      "Update: g=(%.2f, %.2f, %.2f)", //NOLINT
                  gravity_calibration_[j].value[0],
                  gravity_calibration_[j].value[1],
                  gravity_calibration_[j].value[2]);
  }
}

// TODO(Lukas): Kalman filter goes here
void Navigation::queryImus()
{
  OnlineStatistics<NavigationVector> filter;
  sensor_readings = data_.getSensorsImuData();
  for (int i = 0; i < data::Sensors::kNumImus; ++i) {
    // Apply calibrated correction
    filter.update(sensor_readings_.value[i] - gravity_calibration_[i]);
  }
  acceleration_.value = filter.getMean();
  acceleration_.timestamp = sensor_readings_.timestamp;

  acceleration_integrator_.update(acceleration_);
  velocity_integrator_.update(velocity_);
}

void Navigation::updateData()
{
  data::Navigation nav_data;
  nav_data.distance                   = getDistance();
  nav_data.velocity                   = getVelocity();
  nav_data.acceleration               = getAcceleration();
  nav_data.emergency_braking_distance = getEmergencyBrakingDistance();
  nav_data.braking_distance           = getBrakingDistance();

  data_.setNavigationData(nav_data);

  log_.INFO("NAV",
      "Update: a=(%.2f, %.2f, %.2f), v=(%.2f, %.2f, %.2f), d=(%.2f, %.2f, %.2f)", //NOLINT
      acceleration_.value[0], acceleration_.value[1], acceleration_.value[2],
      velocity_.value[0]    , velocity_.value[1]    , velocity_.value[2],
      distance_.value[0]    , distance_.value[1]    , distance_.value[2]);
}


}}  // namespace hyped::navigation
