/*
 * Author: Neil McBlane, Brano Pilnan
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

#ifndef NAVIGATION_NAVIGATION_HPP_
#define NAVIGATION_NAVIGATION_HPP_

#include <array>

#include "data/data.hpp"
#include "data/data_point.hpp"
#include "sensors/imu.hpp"
#include "utils/logger.hpp"
#include "utils/math/integrator.hpp"
#include "utils/math/statistics.hpp"

namespace hyped {

using data::Data;
using data::DataPoint;
using data::ImuData;
using data::NavigationType;
using data::NavigationVector;
using utils::Logger;
using utils::math::Integrator;
using utils::math::OnlineStatistics;

namespace navigation {

  class Navigation {
    public:
      typedef std::array<ImuData, data::Sensors::kNumImus> ImuDataArray;
      typedef DataPoint<ImuDataArray>                      ImuDataPointArray;
      typedef std::array<NavigationVector, data::Sensors::kNumImus> NavigationArray;

      /**
       * @brief Construct a new Navigation object
       *
       * @param log System logger
       */
      explicit Navigation(Logger& log);
      /**
       * @brief Get the measured acceleration [m/s^2]
       *
       * @return NavigationType Returns the forward component of acceleration vector (negative when
       *                        decelerating) [m/s^2]
       */
      NavigationType getAcceleration() const;
      /**
       * @brief Get the measured velocity [m/s]
       *
       * @return NavigationType Returns the forward component of velocity vector [m/s]
       */
      NavigationType getVelocity() const;
      /**
       * @brief Get the measured displacement [m]
       *
       * @return NavigationType Returns the forward component of displacement vector [m]
       */
      NavigationType getDistance() const;
      /**
       * @brief Get the emergency braking distance [m]
       *
       * @return NavigationType emergency braking distance [m]
       */
      NavigationType getEmergencyBrakingDistance() const;
      /**
       * @brief Get the braking distance [m]
       *
       * @return NavigationType braking distance [m]
       */
      NavigationType getBrakingDistance() const;
      /**
       * @brief Get the determined gravity calibration [m/s^2]
       *
       * @return NavitationArray recorded gravitational acceleration [m/s^2]
       */
      NavigationArray getGravityCalibration() const;
      /**
       * @brief Update central data structure
       */
      void updateData();

    private:
      static constexpr int kNumCalibrationQueries = 10000;
      static constexpr NavigationType kEmergencyDeceleration = 24;

      // System communication
      Logger& log_;
      Data& data_;

      // To store estimated values
      ImuDataPointArray sensor_readings_;
      DataPoint<NavigationVector> acceleration_;
      DataPoint<NavigationVector> velocity_;
      DataPoint<NavigationVector> distance_;
      NavigationArray gravity_calibration_;

      // To convert acceleration -> velocity -> distance
      Integrator<NavigationVector> acceleration_integrator_;  // acceleration to velocity
      Integrator<NavigationVector> velocity_integrator_;      // velocity to distance

      /**
       * @brief Determine the value of gravitational acceleration measured by sensors at rest
       */
      void calibrateGravity();
      /**
       * @brief Query sensors to determine acceleration, velocity and distance
       */
      void queryImus();
  };


}}  // namespace hyped::navigation

#endif  // NAVIGATION_NAVIGATION_HPP_
