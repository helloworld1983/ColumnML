// Copyright (C) 2018 Kaan Kara - Systems Group, ETH Zurich

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.

// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//*************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "../src/scd.h"

using namespace std;

#define NUM_VALUES_PER_LINE 16
#define VALUE_TO_INT_SCALER 10

int main(int argc, char* argv[]) {

	cpu_set_t cpuset;
	pthread_t thread = pthread_self();
	pthread_attr_t attr;
	pthread_getattr_np(thread, &attr);
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

	char* pathToDataset;
	uint32_t numSamples = 0;
	uint32_t numFeatures = 0;
	uint32_t miniBatchSize = 16384;
	uint32_t numInstances = 1;
	if (argc != 5) {
		cout << "Usage: ./app <numSamples> <numFeatures> <miniBatchSize> <f_instances>" << endl;
		return 0;
	}
	else {
		numSamples = atoi(argv[1]);
		numFeatures = atoi(argv[2]);
		miniBatchSize = atoi(argv[3]);
		numInstances = atoi(argv[4]);
	}
	char doRealSCD = 0;
	char doLogreg = 0;

	uint32_t stepSizeShifter = 1;
	uint32_t numEpochs = 64;
	uint32_t numMinibatchesAtATime = 1;
	float lambda = 0.01;

	scd scd_app(1);

	scd_app.generate_synthetic_data(numSamples, numFeatures, 0);
	scd_app.print_samples(2);

	float compressionRate = scd_app.compress_a(miniBatchSize, VALUE_TO_INT_SCALER);
	cout << "compressionRate: " << compressionRate << endl;
	scd_app.encrypt_a(miniBatchSize, 1);

	const uint32_t periodsToTest = 6;
	uint32_t periods[periodsToTest] = {1, 2, 4, 8, 16, 32};

	float x_history[scd_app.numFeatures*numEpochs];

	uint32_t index = 0;
	double times[periodsToTest*2];
	for (uint32_t i = 0; i < periodsToTest; i++){

		cout << "============================================================================== residualUpdatePeriod: " << periods[i] << endl;

		cout << "==================================AVX-14, minibatchSCD" << endl;
		times[index++] = scd_app.AVXmulti_float_linlogreg_SCD(x_history, 0, doLogreg, numEpochs, miniBatchSize, periods[i], 1.0/(1 << stepSizeShifter), lambda, 0, 0, VALUE_TO_INT_SCALER, 14);
		cout << "==================================AVX-14, minibatchSCD, decomp+decrypt" << endl;
		times[index++] = scd_app.AVXmulti_float_linlogreg_SCD(x_history, 0, doLogreg, numEpochs, miniBatchSize, periods[i], 1.0/(1 << stepSizeShifter), lambda, 1, 1, VALUE_TO_INT_SCALER, 14);

		// cout << "==================================FPGA-4, mSCD" << endl;
		// times[index++] = scd_app.float_linreg_FSCD(x_history, 0, numEpochs, miniBatchSize, periods[i], 1.0/(1 << stepSizeShifter), 0, 0, 0, VALUE_TO_INT_SCALER, numInstances);
		// cout << "==================================FPGA-4, mSCD, decomp+decrypt" << endl;
		// times[index++] = scd_app.float_linreg_FSCD(x_history, 0, numEpochs, miniBatchSize, periods[i], 1.0/(1 << stepSizeShifter), 0, 1, 1, VALUE_TO_INT_SCALER, numInstances);
	}

	index = 0;
	for (uint32_t i = 0; i < periodsToTest; i++) {
		for (uint32_t j = 0; j < 2; j++) {
			cout << times[index++] << "\t";
		}
		cout << endl;
	}

	return 0;
}
