/*
 * Author: Lukas Schaefer
 * Organisation: HYPED
 * Date: 30/03/2019
 * Description: Kalman filter manager (interface for filter and filter setup)
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

#include "kalman_manager.hpp"

namespace hyped
{
    namespace navigation
    {
        KalmanManager::KalmanManager(unsigned int n_, unsigned int m_)
            : n(n_),
              m(m_)
        {
            k = 0;
            kalmanFilter = KalmanMvar(n_, m_);
        }

        KalmanManager::KalmanManager(unsigned int n_, unsigned int m_, unsigned int k_)
            : n(n_),
              m(m_),
              k(k_)
        {
            kalmanFilter = KalmanMvar(n_, m_, k_);
        }

        void KalmanManager::setupStationary()
        {
            // setup dynamics & measurement models for stationary test
            MatrixXd A = createStateTransitionMatrix(0.0);
            MatrixXd Q = createStateTransitionCovarianceMatrix();
            MatrixXd H = createMeasurementMatrix();
            MatrixXd R = createStationaryMeasurementCovarianceMatrix();
            kalmanFilter.setModels(A, Q, H, R);

            // setup initial estimates
            VectorXd x = VectorXd::Zero(n);
            MatrixXd P = createInitialErrorCovarianceMatrix();
            kalmanFilter.setInitial(x, P);
        }

        void KalmanManager::setupElevator()
        {
            // setup dynamics & measurement models for elevator test
            MatrixXd A = createStateTransitionMatrix(0.0);
            MatrixXd Q = createStateTransitionCovarianceMatrix();
            MatrixXd H = createMeasurementMatrix();
            MatrixXd R = createElevatorMeasurementCovarianceMatrix();
            kalmanFilter.setModels(A, Q, H, R);

            // setup initial estimates
            VectorXd x = VectorXd::Zero(n);
            MatrixXd P = createInitialErrorCovarianceMatrix();
            kalmanFilter.setInitial(x, P);
        }

        void KalmanManager::updateStateTransitionMatrix(double dt)
        {
            MatrixXd A = createStateTransitionMatrix(dt);
            kalmanFilter.update(A);
        }

        void KalmanManager::filter(NavigationVector& z_)
        {
            VectorXd z(m);
            for (unsigned int i = 0; i < m; i++) {
                z(i) = z_[i];
            }
            kalmanFilter.filter(z);
        }

        void KalmanManager::filter(NavigationVector& u_, NavigationVector& z_)
        {
            VectorXd u(k);
            for (unsigned int i = 0; i < k; i++) {
                u(i) = u_[i];
            }

            VectorXd z(m);
            for (unsigned int i = 0; i < m; i++) {
                z(i) = z_[i];
            }

            kalmanFilter.filter(u, z);
        }

        const NavigationEstimate KalmanManager::getEstimate()
        {
            VectorXd x = kalmanFilter.getStateEstimate();
            NavigationVector pos = NavigationVector({x(0), x(1), x(2)})
            NavigationVector vel = NavigationVector({x(3), x(4), x(5)})
            NavigationVector acc = NavigationVector({x(6), x(7), x(8)})
            NavigationEstimate est = {pos, vel, acc};
            return est;
        }

        const MatrixXd KalmanManager::createInitialErrorCovarianceMatrix()
        {
            MatrixXd P = MatrixXd::Constant(n, n, 0.0);
            std::default_random_engine generator;
            std::normal_distribution<double> pos_var_noise(0.0, 0.001);
            std::normal_distribution<double> vel_var_noise(0.0, 0.005);
            std::normal_distribution<double> acc_var_noise(0.0, 0.01);

            for (unsigned int i = 0; i< n; i++) {
                if (i < n/3) {
                    P(i, i) = pos_var_noise(generator);
                } else if (i < 2*n/3) {
                    P(i, i) = vel_var_noise(generator);
                } else {
                    P(i, i) = acc_var_noise(generator);
                }
            }
            return P;
        }

        void KalmanManager::setInitialEstimate()
        {
            // create initial error covariance matrix P
            MatrixXd P = MatrixXd::Constant(n, n, 0.0);
            std::default_random_engine generator;
            std::normal_distribution<double> pos_var_noise(0.0, 0.001);
            std::normal_distribution<double> vel_var_noise(0.0, 0.005);
            std::normal_distribution<double> acc_var_noise(0.0, 0.01);

            for (unsigned int i = 0; i< n; i++) {
                if (i < n/3) {
                    P(i, i) = pos_var_noise(generator);
                } else if (i < 2*n/3) {
                    P(i, i) = vel_var_noise(generator);
                } else {
                    P(i, i) = acc_var_noise(generator);
                }
            }

            // create initial estimate x
            VectorXd x = VectorXd::Zero(n);
            kalmanFilter.setInitial(x, P);
        }

        const MatrixXd KalmanManager::createStateTransitionMatrix(double dt)
        {
            MatrixXd A(n, n);
            double acc_ddt = 0.5 * dt * dt;
            A << 1.0, 0.0, 0.0, dt, 0.0, 0.0, acc_ddt, 0.0, 0.0,
                 0.0, 1.0, 0.0, 0.0, dt, 0.0, 0.0, acc_ddt, 0.0,
                 0.0, 0.0, 1.0, 0.0, 0.0, dt, 0.0, 0.0, acc_ddt,
                 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, dt, 0.0, 0.0,
                 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, dt, 0.0,
                 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, dt,
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0;
            return A;
        }

        const MatrixXd KalmanManager::createMeasurementMatrix()
        {
            MatrixXd H = MatrixXd::Zero(m, n);
            for (unsigned int i = 0; i < m; i++) {
                H(i, n - (m - i)) = 1.0;
            }
            return H;
        }

        const MatrixXd KalmanManager::createStateTransitionCovarianceMatrix()
        {
            std::default_random_engine generator;
            std::normal_distribution<double> var_noise(0.01, 0.02);

            MatrixXd Q = MatrixXd::Constant(n, n, 0.0);
            /*
            for (unsigned int row = 0; row < n; row++)
            {
                for (unsigned int col = 0; col < n; col++)
                {
                    Q(row, col) = var_noise(generator);
                }
            }
            */
            return Q;
        }

        const MatrixXd KalmanManager::createStationaryMeasurementCovarianceMatrix()
        {
            std::default_random_engine generator;
            std::normal_distribution<double> var_noise(0.0, 0.0005);
            std::normal_distribution<double> cov_noise(0.0, 0.0001);

            MatrixXd R(m, m);
            double covariance = -0.0002;
            double variance = 0.0017;

            R << (variance + var_noise(generator)), (covariance + cov_noise(generator)),
                 (covariance + cov_noise(generator)),
                 (covariance + cov_noise(generator)), (variance + var_noise(generator)),
                 (covariance + cov_noise(generator)),
                 (covariance + cov_noise(generator)), (covariance + cov_noise(generator)),
                 (variance + var_noise(generator));
            return R;
        }

        const MatrixXd KalmanManager::createElevatorMeasurementCovarianceMatrix(unsigned int m)
        {
            MatrixXd R(m, m);
            R << 0.0085, 0.0014, 0.0025,
                 0.0014, 0.007, -0.004,
                 0.0025, -0.004, 0.12;
            return R;
        }
    }
}
