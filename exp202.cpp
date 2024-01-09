#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>

#include "SEPP_SDR_API.h"
// #define SETUP_SDR

#include "opssat_sidloc.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

int
sdr_init(uint32_t sampling_rate_cfg, uint32_t lpf_bw_cfg,
         bool rffe_calibration_en, double carrier_frequency_GHz,
         uint32_t gain_dB)
{
  SEPP_SDR_API sdr_; // <= restarts lms bridge (sdr front-end)
  try {
    // instantiate SDR

    sdr_.Print_Info(); // <= if this works - fantastic (can be removed)

    try {

      // Set the RFFE register default values
      std::cout << "Initialize Registers to defaults" << std::endl;
      sdr_.Init_RFFE_Registers();

      // Enable the receiver (hardware enable)
      std::cout << "Enable Receiver" << std::endl;
      sdr_.Enable_Receiver();

      // Read back some configuration values
      std::cout << "Read some configuration parameters:" << std::endl;
      double                     f_c   = sdr_.Get_RX_Carrier_Frequency_in_GHz();
      eSDR_RFFE_RX_SAMPLING_FREQ f_s   = sdr_.Get_RX_Sampling_Frequency();
      eSDR_RFFE_RX_LPF_BW        f_lpf = sdr_.Get_RXLPF_Bandwidth();
      eSDR_RFFE_INPUT            sdr_input = sdr_.Get_RF_Frontend_Input();
      float                      temp_degC = sdr_.Get_Temperature_degC();

      std::cout << "-----------------------------------------------------------"
                   "--------------"
                << std::endl;
      std::cout << "f_c = " << f_c << std::endl;
      std::cout << "f_s = " << (unsigned int)(f_s) << std::endl;
      std::cout << "f_lpf = " << (unsigned int)(f_lpf) << std::endl;
      std::cout << "selected RF input = " << (unsigned int)sdr_input
                << std::endl;
      std::cout << "temperature = " << temp_degC << " degC" << std::endl;
      std::cout << "-----------------------------------------------------------"
                   "--------------"
                << std::endl;
    } catch (SEPP_SDR_API_Exception &e) {
      std::cout << "SEPP_SDR_API_Exception: " << std::string(e.what())
                << std::endl;
      std::cout << "SKIP Telemetry readout and PROCEED" << std::endl;
    }

    try {
      for (int repeat = 0; repeat < 3; repeat++) {
        try {
          // Set RX sampling rate and filter bandwidth
          // Note: select sampling frequency > 2*filter bandwidth (Shannon)
          std::cout << "Set the RX sampling frequency = "
                    << sdr_.rxsf_to_MHz(
                           (eSDR_RFFE_RX_SAMPLING_FREQ)sampling_rate_cfg)
                    << " MHz" << std::endl;
          sdr_.Set_RX_Sampling_Frequency(
              (eSDR_RFFE_RX_SAMPLING_FREQ)sampling_rate_cfg);

          // read back the sampling frequency setting
          eSDR_RFFE_RX_SAMPLING_FREQ f_s = sdr_.Get_RX_Sampling_Frequency();

          // IF match and no exception, stop loop
          if (f_s == sampling_rate_cfg) {
            break;
          }
        } catch (SEPP_SDR_API_Exception &e) {
          std::cout << "SEPP_SDR_API_Exception: " << std::string(e.what())
                    << std::endl;
          std::cout << "RETRY to set RX sampling frequency" << std::endl;

          if (repeat == 2) {
            throw SEPP_SDR_API_Exception(
                "Set RX sampling frequency failed (REPEAT)");
          }
        }
      }

      std::cout << "Set the RX LPF bandwidth = "
                << sdr_.rxlpfbw_to_MHz((eSDR_RFFE_RX_LPF_BW)lpf_bw_cfg)
                << " MHz" << std::endl;
      sdr_.Set_RXLPF_Bandwidth((eSDR_RFFE_RX_LPF_BW)lpf_bw_cfg);

      // Set RX carrier frequency
      std::cout << "Set the RX carrier frequency f_c = "
                << carrier_frequency_GHz << " GHz" << std::endl;
      double achievable_rx_carrier_freq =
          sdr_.Set_RX_Carrier_Frequency_in_GHz(carrier_frequency_GHz);
      std::cout << "The RX carrier was set to the following achievable "
                   "frequency: f_ca = "
                << achievable_rx_carrier_freq << " GHz" << std::endl;

      // Start signal reception and gain sweep
      std::cout << "Capture signal for different gain values (6dB steps)"
                << std::endl;

      // Set RX channel gain
      std::cout << "Set gain = " << gain_dB << " dB" << std::endl;
      sdr_.Set_RX_Gain_in_dB(gain_dB);

      if (rffe_calibration_en == true) {
        // Do a calibration of the RF channel (for the given gain etc.)
        std::cout << "Calibrate RF frontend (LMS)" << std::endl;
        sdr_.Calibrate_RF_Frontend();
      } else {
        std::cout << "Skipped: Calibrate RF frontend (LMS)" << std::endl;
      }

      // Enable sampling clock (= start RX sample data output and SEPP data
      // capturing)
      std::cout << "Enable sampling clock" << std::endl;
      sdr_.Enable_RX_Sampling_Clock();

    } catch (SEPP_SDR_API_Exception &e) {
      std::cout << "SEPP_SDR_API_Exception: " << std::string(e.what())
                << std::endl;
      throw std::runtime_error("Exception in SDR initialization");
    }

  } catch (SEPP_SDR_API_Exception &e) {
    std::cout << "SEPP_SDR_API_Exception: " << std::string(e.what())
              << std::endl;
    throw std::runtime_error("Exception in SDR initialization");
  }

  try {
    std::cout << "Read some configuration parameters after calibration:"
              << std::endl;
    double                     f_c   = sdr_.Get_RX_Carrier_Frequency_in_GHz();
    eSDR_RFFE_RX_SAMPLING_FREQ f_s   = sdr_.Get_RX_Sampling_Frequency();
    eSDR_RFFE_RX_LPF_BW        f_lpf = sdr_.Get_RXLPF_Bandwidth();
    eSDR_RFFE_INPUT            sdr_input = sdr_.Get_RF_Frontend_Input();
    float                      temp_degC = sdr_.Get_Temperature_degC();

    std::cout << "-------------------------------------------------------------"
                 "------------"
              << std::endl;
    std::cout << "f_c = " << f_c << std::endl;
    std::cout << "f_s = " << (unsigned int)(f_s) << std::endl;
    std::cout << "f_lpf = " << (unsigned int)(f_lpf) << std::endl;
    std::cout << "selected RF input = " << (unsigned int)sdr_input << std::endl;
    std::cout << "temperature = " << temp_degC << " degC" << std::endl;
    std::cout << "-------------------------------------------------------------"
                 "------------"
              << std::endl;
  } catch (SEPP_SDR_API_Exception &e) {
    std::cout << "SEPP_SDR_API_Exception: " << std::string(e.what())
              << std::endl;
    std::cout << "SKIP Telemetry readout and PROCEED" << std::endl;
  }
  return 0;
}
uint32_t *
open_dev(const char *name)
{
  int    uiofd  = open(name, O_RDWR);
  size_t length = 0x00001000;
  off_t  offset = 0x0;
  if (uiofd < 0) {
    return nullptr;
  }

  uint32_t *d_mem = (uint32_t *)mmap(NULL, length, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, uiofd, offset);
  if (d_mem == MAP_FAILED) {
    std::cout << name << " Error is " << errno << std::endl;
    return nullptr;
  }
  return d_mem;
}

void
uio_write(uint32_t *d_mem, size_t offset, size_t value)
{
  *((volatile uint32_t *)(d_mem + offset)) = value;
}

size_t
uio_read(uint32_t *d_mem, size_t offset)
{
  return *((volatile uint32_t *)(d_mem + offset));
}

void
Enable_Data_Streaming(uint32_t *sdr_uio)
{
  std::cout << "Writing register " << uio_read(sdr_uio, 0x00) << std::endl;
  uint32_t temporary_control;
  temporary_control = 0x01;

  std::cout << "Writing register " << temporary_control << std::endl;
  uio_write(sdr_uio, 0x00, temporary_control);
}

void
Disable_Data_Streaming(uint32_t *sdr_uio)
{
  uint32_t temporary_control;
  temporary_control = uio_read(sdr_uio, 0x00) & (1 ^ 0xFFFFFFFF);
  uio_write(sdr_uio, 0x00, temporary_control);
}

void
Clear_Receiver_Buffers(uint32_t *sdr_uio)
{
  // clear FPGA internal sample buffers
  uint32_t temporary_control = uio_read(sdr_uio, 0x00);
  uio_write(sdr_uio, 0x00, temporary_control & ~(1 << 1));
  // uio_write(sdr_uio, 0x00, temporary_control | (1 << 1));
  std::this_thread::sleep_for(std::chrono::microseconds(10000));
  // uio_write(sdr_uio, 0x00, temporary_control & ~(1 << 1));
  uio_write(sdr_uio, 0x00, temporary_control | (1 << 1));
}

int
fpga_frontend_en(uint32_t *sdr_uio)
{

  // Disable data streaming
  std::cout << "SDR middleware: Disable data streaming" << std::endl;
  Disable_Data_Streaming(sdr_uio);

  // Clear FPGA internal buffers to delete old samples
  std::cout << "SDR middleware: Clear RX buffers" << std::endl;
  // Clear_Receiver_Buffers(sdr_uio);

  // Enable FPGA SDR Sample Receiver via AVMMS RegMap
  std::cout << "SDR middleware: Enable data streaming" << std::endl;
  Enable_Data_Streaming(sdr_uio);
  usleep(100);

  /* AFTER THIS YOU CAN SUCCESSFULLY GATHER IQ VALUES FROM SDR */
}

// int fpga_frontend_dis(uint32_t* sdr_uio){
//     // Stop FPGA SDR Sample Receiver via AVMMS RegMap
//     std::cout << "SDR middleware: Disable data streaming" << std::endl;
// 	Disable_Data_Streaming(sdr_uio);
// }

// int sdr_deinit(void){
//     // Disable sampling clock (= stop RX sample data output and SEPP data
//     capturing -> some old buffered data may remain) std::cout << "Disable
//     sampling clock" << std::endl; sdr_.Disable_RX_Sampling_Clock();
//     // Disable the RX frontend
//     std::cout << "Disable Receiver" << std::endl;
//     sdr_.Disable_Receiver();
// }

// Function to trim leading and trailing spaces from a string
std::string
trim(const std::string &str)
{
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first)
    return str;
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

// Function to check if a string represents a valid integer
bool
isInteger(const std::string &str)
{
  return !str.empty() &&
         str.find_first_not_of("0123456789") == std::string::npos;
}

/**
 * @brief Stores IQ samples into a file
 *
 * @param output the full path of the output file to store the IQ samples
 * @param constrained if set to true, only a specific amount of samples will be
 * stored and the function will return afterwards
 * @param nsamples the number of samples to store if the \param constrained is
 * set
 */
void
iq_capture(const std::string &output, bool constrained, size_t nsamples)
{
  const char  *output_filename;
  int64_t      nitems = nsamples;
  size_t       reps;
  int          ret;
  std::fstream out_file;

  /* Buffer to store received samples */
  uint32_t buffer32[DESC_PER_CHAIN * LEN_PER_DESCRIPTOR];

  if (constrained == false) {
    /*To trick the while statement to run idefinately*/
    nitems = 1;
  }
  opssat_sidloc opssat_dev = opssat_sidloc("/dev/uio0", "/dev/uio1");

  /* Activate the stream */
  opssat_dev.activate_stream();

  out_file.open(output, std::ios::out | std::ios::binary);

  /* Read REPS times from the FPGA */
  while (nitems > 0) {
    ret = opssat_dev.read_stream(buffer32, DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
    if (ret < 0) {
      continue;
    }
    out_file.write((char *)buffer32, DESC_PER_CHAIN * LEN_PER_DESCRIPTOR);
    if (constrained)
      nitems -= (DESC_PER_CHAIN * LEN_PER_DESCRIPTOR) / sizeof(uint32_t);
  }
  out_file.close();
}

int
main(int argc, char *argv[])
{
  double              carrier_frequency_ghz;
  eSDR_RFFE_RX_LPF_BW bw_code;
  uint32_t            gain_db;
  bool                calibrate_frontend;
  std::string         binary_path;
  uint32_t            f_set;
  bool                is_constrained;
  size_t              nsamples;
  std::string         output_path;

  if (argc != 2) {
    std::cerr << argc << " Usage: " << argv[0] << " <path-to-config>"
              << std::endl;
    return EXIT_FAILURE;
  }
  std::string path_to_config(argv[1]);

  std::map<std::string, std::string> configParams;

  // Read the configuration file
  std::ifstream configFile(path_to_config);
  if (!configFile.is_open()) {
    std::cerr << "Error opening config file." << std::endl;
    return 1;
  }

  std::string line;
  std::string section;

  while (std::getline(configFile, line)) {
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos) {
      // Remove comments at the end of the line
      line = line.substr(0, commentPos);
    }

    line = trim(line);

    if (line.empty()) {
      // Ignore empty lines
      continue;
    } else if (line[0] == '[' && line[line.length() - 1] == ']') {
      // Extract section name
      section = line.substr(1, line.length() - 2);
    } else {
      // Parse parameter and value
      size_t delimiterPos = line.find('=');
      if (delimiterPos != std::string::npos) {
        std::string param    = trim(line.substr(0, delimiterPos));
        std::string valueStr = trim(line.substr(delimiterPos + 1));

        if (isInteger(valueStr)) {
          configParams[param] = valueStr;
        } else {
          double realValue    = std::atof(valueStr.c_str());
          configParams[param] = valueStr;
        }
      }
    }
  }

  // Close the configuration file
  configFile.close();

  if (configParams.find("carrier_frequency_GHz") != configParams.end()) {
    carrier_frequency_ghz =
        std::atof(configParams["carrier_frequency_GHz"].c_str());
  } else {
    throw std::invalid_argument("Carrier frequency param not in config file");
  }

  if (configParams.find("lpf_bw_cfg") != configParams.end()) {
    bw_code =
        (eSDR_RFFE_RX_LPF_BW)std::atoi(configParams["lpf_bw_cfg"].c_str());
  } else {
    throw std::invalid_argument(
        "Lowpass Filter bandwidth param not in config file");
  }
  if (configParams.find("gain_db") != configParams.end()) {
    gain_db = std::atoi(configParams["gain_db"].c_str());
  } else {
    throw std::invalid_argument("RX gain param not in config file");
  }
  if (configParams.find("calibrate_frontend") != configParams.end()) {
    calibrate_frontend =
        (std::atoi(configParams["calibrate_frontend"].c_str())) ? true : false;
  } else {
    throw std::invalid_argument("Enable calibration param not in config file");
  }
  if (configParams.find("binary_path") != configParams.end()) {
    binary_path = configParams["binary_path"];
  } else {
    throw std::invalid_argument("Binary path param not in config file");
  }
  if (configParams.find("samp_freq_index") != configParams.end()) {
    f_set = (eSDR_RFFE_RX_SAMPLING_FREQ)(std::atoi(
        configParams["samp_freq_index"].c_str()));
  } else {
    throw std::invalid_argument(
        "Sampling frequency index param not in config file");
  }
  if (configParams.find("number_of_samples") != configParams.end()) {
    std::stringstream sstream(configParams["number_of_samples"]);
    sstream >> nsamples;
    is_constrained = true;
  } else {
    is_constrained = false;
    nsamples       = 0;
  }
  if (configParams.find("output_path") != configParams.end()) {
    output_path = configParams["output_path"];
  } else {
    throw std::invalid_argument("Binary path param not in config file");
  }

  double f_c;
  std::cout << "Setting up SDR" << std::endl;
  // #ifdef SETUP_SDR
  uint32_t *sdr_uio = open_dev("/dev/uio2");
  if (!sdr_uio)
    std::cerr << "Could not open UIO succesfully " << std::endl;
  eSDR_RFFE_RX_SAMPLING_FREQ f_s;
  eSDR_RFFE_RX_LPF_BW        f_lpf;
  eSDR_RFFE_INPUT            sdr_input;

  float    temp_degC;
  uint32_t gain;

  try {
    sdr_init(f_set, bw_code, calibrate_frontend, carrier_frequency_ghz,
             gain_db);
    fpga_frontend_en(sdr_uio);
  } catch (SEPP_SDR_API_Exception e) {
    std::runtime_error(e.what());
    throw std::runtime_error("Exception in SDR initialization");
  }
  std::stringstream s;
  s << output_path << std::time(0);
  const std::string logfile(s.str());

  std::cout << "IQ capture: Starting..." << ::std::endl;
  std::cout << "Output IQ file   : " << logfile << ::std::endl;
  std::cout << "Constrained      : " << is_constrained << ::std::endl;
  std::cout << "Samples Num      : " << nsamples << ::std::endl;
  iq_capture(logfile, is_constrained, nsamples);
  std::cout << "IQ capture: Finished" << ::std::endl;
  return EXIT_SUCCESS;
}
