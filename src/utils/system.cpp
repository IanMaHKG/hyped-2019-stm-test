/*
 * Authors: M. Kristien
 * Organisation: HYPED
 * Date: 2. April 2018
 * Description:
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

#include "utils/system.hpp"

#include <stdlib.h>
// #include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <csignal>

#define DEFAULT_VERBOSE   -1
#define DEFAULT_DEBUG     -1
#define DEFAULT_IMU_ID    -1
#define DEFAULT_IMU_COUNT -1
#define DEFAULT_RUN_ID		-1
#define DEFAULT_QUERIES   -1
#define DEFAULT_Q_DELAY   -1
#define DEFAULT_CALIBRATE -1

namespace hyped {
namespace utils {

namespace {
void printUsage()
{
  printf("./hyped [args]\n");
  printf(
    "All arguments are optional. To provide an argument with a value, use <argument>=<value>.\n"
    "Default value of verbose flags is 1\n"
    "Default value of debug   flags is 0\n"
    "\n  -v, --verbose[=<bool>]\n"
    "    Set system-wide setting of verbosity. If enabled, prints all INFO messages\n"
    "\n  --verbose_motor, --verbose_nav, --verbose_sensor, --verbose_state, --verbose_tlm\n"
    "    Set module-specific setting of verbosity. If enabled, prints all INFO messages\n"
    "\n  -d, --debug[=<level>]\n"
    "    Set system-wide debug level. All DBG[n] where n <= level messages are printed.\n"
    "\n  --debug_motor, --debug_nav, --debug_sensor, --debug_state, --debug_tlm\n"
    "    Set module-specific debug level. All DBG[n] where n <= level messages are printed.\n"
    "\n  --fake_imu --fake_proxi --fake_motors --fake_sensors --fake_keyence --fake_batteries\n"
    "    Make the system use the fake data drivers.\n"
    "\n  --fail_dec_imu --fail_acc_imu --fail_motors --miss_keyence --double_keyence\n"
    "    Make the system use the fake data drivers and fail them for testing.\n"
    "\n  --accurate\n"
    "    Make the system use the accurate fake system\n"
    "\n  --imu_id\n"
    "    IMU ID number to assign to written datafile."
    "\n  --run_id\n"
    "    Run ID number to assign to written datafile."
    "\n  --imu_count\n"
    "    Number of IMUs to use (max=7)."
    "");
}
}   // namespace hyped::utils::System

System::~System()
{
  if (log_) delete log_;
}

System::System(int argc, char* argv[])
    : verbose(false),
      verbose_motor(DEFAULT_VERBOSE),
      verbose_nav(DEFAULT_VERBOSE),
      verbose_sensor(DEFAULT_VERBOSE),
      verbose_state(DEFAULT_VERBOSE),
      verbose_tlm(DEFAULT_VERBOSE),
      debug(DEFAULT_DEBUG),
      debug_motor(DEFAULT_DEBUG),
      debug_nav(DEFAULT_DEBUG),
      debug_sensor(DEFAULT_DEBUG),
      debug_state(DEFAULT_DEBUG),
      debug_cmn(DEFAULT_DEBUG),
      fake_imu(false),
      fake_proxi(false),
      fake_sensors(false),
      fake_motors(false),
      fake_embrakes(false),
      fail_dec_imu(false),
      fail_acc_imu(false),
      fail_motors(false),
      miss_keyence(false),
      double_keyence(false),
      accurate(false),
      running_(true),
      imu_id(DEFAULT_IMU_ID),
      imu_count(DEFAULT_IMU_COUNT),
      run_id(DEFAULT_RUN_ID),
      debug_tlm(DEFAULT_DEBUG),
      running_(true)

{
  int c;
  int option_index = 0;
  while (1) {
    static option long_options[] = {
      {"verbose", optional_argument, 0, 'v'},
      {"verbose_motor", optional_argument, 0, 'a'},
      {"verbose_nav", optional_argument, 0, 'A'},
      {"verbose_sensor", optional_argument, 0, 'b'},
      {"verbose_state", optional_argument, 0, 'B'},
      {"verbose_tlm", optional_argument, 0, 'c'},
      {"debug", optional_argument, 0, 'd'},
      {"debug_motor", optional_argument, 0, 'e'},
      {"debug_nav", optional_argument, 0, 'E'},
      {"debug_sensor", optional_argument, 0, 'f'},
      {"debug_state", optional_argument, 0, 'F'},
      {"debug_tlm", optional_argument, 0, 'g'},
      {"help", no_argument, 0, 'h'},
      {"fake_imu", optional_argument, 0, 'i'},
      {"fail_dec_imu", optional_argument, 0, 'I'},
      {"fake_proxi", optional_argument, 0, 'j'},
      {"fail_acc_imu", optional_argument, 0, 'J'},
      {"fake_sensors", optional_argument, 0, 'k'},
      {"fail_motors", optional_argument, 0, 'K'},
      {"fake_keyence", optional_argument, 0, 'l'},
      {"miss_keyence", optional_argument, 0, 'L'},
      {"fake_motors", optional_argument, 0, 'm'},
      {"double_keyence", optional_argument, 0, 'M'},
      {"fake_embrakes", optional_argument, 0, 'n'},
      {"accurate", optional_argument, 0, 'N'},
      {"fake_batteries", optional_argument, 0, 'o'},
      {"imu_id", optional_argument, 0, 'p'},
      {"imu_count", optional_argument, 0, 'P'},
      {"run_id", optional_argument, 0, 'q'},
      {0, 0, 0, 0}
    };    // options for long in long_options array, can support optional argument
    // returns option character from argv array following '-' or '--' from command line
    c = getopt_long(argc, argv, "vd::h", long_options, &option_index);


    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
      case 'v':
        if (optarg) verbose = atoi(optarg);
        else        verbose = true;
        break;
      case 'h':
        printUsage();
        exit(0);
        break;
      case 'd':
        if (optarg) debug = atoi(optarg);
        else        debug = 0;
        break;
      case 'a':   // verbose_motor
        if (optarg) verbose_motor = atoi(optarg);
        else        verbose_motor = true;
        break;
      case 'A':   // verbose_nav
        if (optarg) verbose_nav = atoi(optarg);
        else        verbose_nav = true;
        break;
      case 'b':   // verbose_sensor
        if (optarg) verbose_sensor = atoi(optarg);
        else        verbose_sensor = true;
        break;
      case 'B':   // verbose_state
        if (optarg) verbose_state = atoi(optarg);
        else        verbose_state = true;
        break;
      case 'c':   // verbose_tlm
        if (optarg) verbose_tlm = atoi(optarg);
        else        verbose_tlm = true;
        break;
      case 'e':   // debug_motor
        if (optarg) debug_motor = atoi(optarg);
        else        debug_motor = 0;
        break;
      case 'E':   // debug_nav
        if (optarg) debug_nav = atoi(optarg);
        else        debug_nav = 0;
        break;
      case 'f':   // debug_sensor
        if (optarg) debug_sensor = atoi(optarg);
        else        debug_sensor = 0;
        break;
      case 'F':   // debug_state
        if (optarg) debug_state = atoi(optarg);
        else        debug_state = 0;
        break;
      case 'g':   // debug_tlm
        if (optarg) debug_tlm = atoi(optarg);
        else        debug_tlm = 0;
        break;
      case 'p':
        if (optarg) imu_id = atoi(optarg);
        else        imu_id = 1;
        break;
      case 'P':
        if (optarg) imu_count = atoi(optarg);
        else        imu_count = 7;
        break;
      case 'q':
        if (optarg) run_id = atoi(optarg);
        else        run_id = 1;
        break;
      default:
        printUsage();
        exit(1);
        break;
    }
  }

  // propagate verbose and debug to modules if not set module-specific
  if (verbose_motor   == DEFAULT_VERBOSE) verbose_motor   = verbose;
  if (verbose_nav     == DEFAULT_VERBOSE) verbose_nav     = verbose;
  if (verbose_sensor  == DEFAULT_VERBOSE) verbose_sensor  = verbose;
  if (verbose_state   == DEFAULT_VERBOSE) verbose_state   = verbose;
  if (verbose_tlm     == DEFAULT_VERBOSE) verbose_tlm     = verbose;

  if (debug_motor   == DEFAULT_DEBUG) debug_motor   = debug;
  if (debug_nav     == DEFAULT_DEBUG) debug_nav     = debug;
  if (debug_sensor  == DEFAULT_DEBUG) debug_sensor  = debug;
  if (debug_state   == DEFAULT_DEBUG) debug_state   = debug;
  if (debug_tlm     == DEFAULT_DEBUG) debug_tlm     = debug;

  log_ = new Logger(verbose, debug);
  system_ = this;   // own address
}

System* System::system_ = 0;

void System::parseArgs(int argc, char* argv[])
{
  if (system_) return;                  // when all command-line option have been parsed
  system_ = new System(argc, argv);     // System overloaded
}

System& System::getSystem()
{
  if (system_) return *system_;
  Logger log;
  log.ERR("SYSTEM", "somebody tried to get System"
          " before initialisation, aborting");
  exit(1);
}

Logger& System::getLogger()
{
  System& sys = getSystem();
  return *sys.log_;
}

static void gracefulExit(int x)
{
  Logger log(true, 0);
  log.INFO("SYSTEM", "termination signal received, exiting gracefully");
  exit(0);
}

static void segfaultHandler(int x)
{
  // start turning the system off
  System& sys = System::getSystem();
  sys.running_ = false;

  Logger log(true, 0);
  log.ERR("SYSTEM", "forced termination detected (segfault?)");
  exit(0);
}

bool System::setExitFunction()
{
  static bool signal_set = false;
  if (signal_set) return true;

  // nominal termination
  std::signal(SIGINT, &gracefulExit);

  // forced termination
  std::signal(SIGSEGV, &segfaultHandler);
  std::signal(SIGABRT, &segfaultHandler);
  std::signal(SIGFPE,  &segfaultHandler);
  std::signal(SIGILL,  &segfaultHandler);
  std::signal(SIGTERM, &segfaultHandler);

  signal_set = true;
  return true;
}


bool handle_registeres = System::setExitFunction();
}}  // namespace hyped::utils
