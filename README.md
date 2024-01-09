# OPSSAT-SIDLOC Experiment

SIDLOC (Spacecraft Identification and Localization) is a new transmission scheme proposed by Libre Space Foundation enable LEO satellite identification and tracking.
The SIDLOC system relies on a reduced size autonomous transmitter that requires minimal integration and area on the carrier spacecraft.
The ground segment of the SIDLOC system utilizes the existing SatNOGS network and the UHF band to perform the signal demodulation and identification.
The SIDLOC system uses DSSS (Direct Sequence Spread Spectrum) PSK modulated signals with an effective bitrate of ~50 bitss.
Despite the fact that SIDLOC operates using the downlink, a feature that is not available in OPS-SAT SDR frontend, vital information can be retrieved performing the mirrored case; one or more ground stations transmitting SIDLOC modulated signals to the satellite.
The goal of this activity is to utilize the SDR capabilities of the OPS-SAT satellite, test the SIDLOC system under realistic conditions and identify the impact of the LEO channel impairments on the SIDLOC system.

## Requirements
* C++11
* Make
* ARM Linux compiler

## Experiment compilation and execution
In order this program to be able to operate, the OPS-SAT FPGA image should be changed to the image provided by Libre Space Foundation, enhanced with a Scatter-Gather DMA engine.
Unfortunately due to licensing issues, the FPGA design cannot be shared.
Contact the OPS-SAT team to ensure that the correct bitstream is loaded.

To generate the executable, just run `make`.
If the SEPP standard image is placed at a custom path, set it accordingly by overriding the `STANDARD_IMAGE_PATH` variable of the Makefile.

After successful compilation, the `exp202` executable should be present inside the `launch` directory, which contains everything needed to run the experiment.

### Configuration
The experiment parameters can be set from the `config.ini` file that can be found at the `launch` directory.
Except the SDR parameters, the experiment requires two extra parameters:
* `output_path`: The file to store the IQ recording
* `number_of_samples`: The number of IQ samples to store into the file. If this parameter is not set, the experiment will run indefinitely


