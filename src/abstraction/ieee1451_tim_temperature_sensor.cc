#include <ieee1451_tim_temperature_sensor.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS


IEEE1451TemperatureSensor::IEEE1451TemperatureSensor(bool tim_im, bool polling)
{
	this->channelNumber = 0x01; //Só suporta um transdutor por mote!
	this->tim_im = tim_im;
	this->polling = polling;
	this->pos = 0;
	this->executeThread = 0;

	channelArray = new char[123];
	channelArray[0] = 0x00; channelArray[1] = 0x00; channelArray[2] = 0x00; channelArray[3] = 0x77; channelArray[4] = 0x03; channelArray[5] = 0x04; channelArray[6] = 0x00; channelArray[7] = 0x03; channelArray[8] = 0x01; channelArray[9] = 0x01; channelArray[10] = 0x0a; channelArray[11] = 0x01; channelArray[12] = 0x00; channelArray[13] = 0x0b; channelArray[14] = 0x01; channelArray[15] = 0x00; channelArray[16] = 0x0c; channelArray[17] = 0x06; channelArray[18] = 0x32; channelArray[19] = 0x01; channelArray[20] = 0x00; channelArray[21] = 0x39; channelArray[22] = 0x01; channelArray[23] = 0x82; channelArray[24] = 0x0d; channelArray[25] = 0x04; channelArray[26] = 0x43; channelArray[27] = 0x69; channelArray[28] = 0x00; channelArray[29] = 0x00; channelArray[30] = 0x0e; channelArray[31] = 0x04; channelArray[32] = 0x43; channelArray[33] = 0xb0; channelArray[34] = 0x80; channelArray[35] = 0x00; channelArray[36] = 0x0f; channelArray[37] = 0x04; channelArray[38] = 0x3f; channelArray[39] = 0x00; channelArray[40] = 0x00; channelArray[41] = 0x00; channelArray[42] = 0x10; channelArray[43] = 0x01; channelArray[44] = 0x00; channelArray[45] = 0x12; channelArray[46] = 0x09; channelArray[47] = 0x28; channelArray[48] = 0x01; channelArray[49] = 0x01; channelArray[50] = 0x29; channelArray[51] = 0x01; channelArray[52] = 0x04; channelArray[53] = 0x2a; channelArray[54] = 0x01; channelArray[55] = 0x00; channelArray[56] = 0x13; channelArray[57] = 0x1c; channelArray[58] = 0x2b; channelArray[59] = 0x02; channelArray[60] = 0x00; channelArray[61] = 0x0a; channelArray[62] = 0x2c; channelArray[63] = 0x04; channelArray[64] = 0x00; channelArray[65] = 0x00; channelArray[66] = 0x00; channelArray[67] = 0x00; channelArray[68] = 0x2d; channelArray[69] = 0x04; channelArray[70] = 0x41; channelArray[71] = 0x20; channelArray[72] = 0x00; channelArray[73] = 0x00; channelArray[74] = 0x2e; channelArray[75] = 0x06; channelArray[76] = 0x32; channelArray[77] = 0x01; channelArray[78] = 0x00; channelArray[79] = 0x37; channelArray[80] = 0x01; channelArray[81] = 0x82; channelArray[82] = 0x2f; channelArray[83] = 0x02; channelArray[84] = 0x00; channelArray[85] = 0x00; channelArray[86] = 0x16; channelArray[87] = 0x04; channelArray[88] = 0x00; channelArray[89] = 0x00; channelArray[90] = 0x00; channelArray[91] = 0x00; channelArray[92] = 0x17; channelArray[93] = 0x04; channelArray[94] = 0x3d; channelArray[95] = 0xcc; channelArray[96] = 0xcc; channelArray[97] = 0xcd; channelArray[98] = 0x18; channelArray[99] = 0x04; channelArray[100] = 0x00; channelArray[101] = 0x00; channelArray[102] = 0x00; channelArray[103] = 0x00; channelArray[104] = 0x19; channelArray[105] = 0x04; channelArray[106] = 0x3f; channelArray[107] = 0x00; channelArray[108] = 0x00; channelArray[109] = 0x00; channelArray[110] = 0x1f; channelArray[111] = 0x06; channelArray[112] = 0x30; channelArray[113] = 0x01; channelArray[114] = 0x02; channelArray[115] = 0x31; channelArray[116] = 0x01; channelArray[117] = 0x02; channelArray[118] = 0x22; channelArray[119] = 0x01; /*channelArray[120] = 0x02;*/ channelArray[121] = 0xf4; channelArray[122] = 0xa5; //checksum errado
	CHANNEL_TEDS = new TEDS_TIM(channelArray, 123);

	if (!tim_im)
		channelArray[120] = 0x01;
	else if (tim_im)
		channelArray[120] = 0x02;

	tempSensorUtnArray = new char[43];
	tempSensorUtnArray[0] = 0x00; tempSensorUtnArray[1] = 0x00; tempSensorUtnArray[2] = 0x00; tempSensorUtnArray[3] = 0x27; tempSensorUtnArray[4] = 0x03; tempSensorUtnArray[5] = 0x04; tempSensorUtnArray[6] = 0x00; tempSensorUtnArray[7] = 0x0c; tempSensorUtnArray[8] = 0x01; tempSensorUtnArray[9] = 0x01; tempSensorUtnArray[10] = 0x04; tempSensorUtnArray[11] = 0x01; tempSensorUtnArray[12] = 0x00; tempSensorUtnArray[13] = 0x05; tempSensorUtnArray[14] = 0x1a; tempSensorUtnArray[15] = 0x4f; tempSensorUtnArray[16] = 0x6e; tempSensorUtnArray[17] = 0x62; tempSensorUtnArray[18] = 0x6f; tempSensorUtnArray[19] = 0x61; tempSensorUtnArray[20] = 0x72; tempSensorUtnArray[21] = 0x64; tempSensorUtnArray[22] = 0x20; tempSensorUtnArray[23] = 0x54; tempSensorUtnArray[24] = 0x65; tempSensorUtnArray[25] = 0x6d; tempSensorUtnArray[26] = 0x70; tempSensorUtnArray[27] = 0x65; tempSensorUtnArray[28] = 0x72; tempSensorUtnArray[29] = 0x61; tempSensorUtnArray[30] = 0x74; tempSensorUtnArray[31] = 0x75; tempSensorUtnArray[32] = 0x72; tempSensorUtnArray[33] = 0x65; tempSensorUtnArray[34] = 0x20; tempSensorUtnArray[35] = 0x53; tempSensorUtnArray[36] = 0x65; tempSensorUtnArray[37] = 0x6e; tempSensorUtnArray[38] = 0x73; tempSensorUtnArray[39] = 0x6f; tempSensorUtnArray[40] = 0x72; tempSensorUtnArray[41] = 0xf5; tempSensorUtnArray[42] = 0x92; //checksum errado
	TEMPSENSOR_UTN_TEDS = new TEDS_TIM(tempSensorUtnArray, 43);
}

IEEE1451TemperatureSensor::~IEEE1451TemperatureSensor()
{
	if (executeThread)
		delete executeThread;

	if (channelArray)
		delete channelArray;
	if (tempSensorUtnArray)
		delete tempSensorUtnArray;

	if (CHANNEL_TEDS)
		delete CHANNEL_TEDS;
	if (TEMPSENSOR_UTN_TEDS)
		delete TEMPSENSOR_UTN_TEDS;
}

TEDS_TIM *IEEE1451TemperatureSensor::getTEDS(char id)
{
	if (id == 0x03)
		return CHANNEL_TEDS;
	else if (id == 0x0c)
		return TEMPSENSOR_UTN_TEDS;
	return 0;
}

void IEEE1451TemperatureSensor::start()
{
	db<IEEE1451TemperatureSensor>(INF) << "Temperature sensor start\n";
	executeThread->resume();
}

void IEEE1451TemperatureSensor::stop()
{
	db<IEEE1451TemperatureSensor>(INF) << "Temperature sensor stop\n";
	executeThread->suspend();
}

void IEEE1451TemperatureSensor::readDataSet(unsigned short transId, unsigned int offset)
{
	db<IEEE1451TemperatureSensor>(INF) << "Reading data set (polling)...\n";

	unsigned int size = sizeof(DataSetReadReply) + DATASET_SIZE * sizeof(float);
	char *buffer = new char[size];

	DataSetReadReply *readReply = (DataSetReadReply *) buffer;
	float *data = (float *) (buffer + sizeof(DataSetReadReply));

	readReply->header.success = true;
	readReply->header.length = DATASET_SIZE * sizeof(float) + sizeof(readReply->offset);
	readReply->offset = 0; //TODO: Ver esquema do offset

	dataSetMutex.lock();
	for (int i = 0, j = pos; i < DATASET_SIZE; i++)
	{
		data[i] = dataSet[j];
		j = (j + 1) % DATASET_SIZE;
	}
	dataSetMutex.unlock();

	IEEE1451dot5_TIM::getInstance()->sendMessage(transId, buffer, size);
	delete[] buffer;
}

void IEEE1451TemperatureSensor::sendDataSet()
{
	db<IEEE1451TemperatureSensor>(INF) << "Sending data set (tim_im)...\n";

	unsigned int size = sizeof(Command) + DATASET_SIZE * sizeof(float);
	char *buffer = new char[size];

	Command *cmd = (Command *) buffer;
	float *data = (float *) (buffer + sizeof(Command));

	cmd->channelNumber = channelNumber;
	cmd->command = COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT;
	cmd->length = DATASET_SIZE * sizeof(float);

	dataSetMutex.lock();
	for (int i = 0, j = pos; i < DATASET_SIZE; i++)
	{
		data[i] = dataSet[j];
		j = (j + 1) % DATASET_SIZE;
	}
	dataSetMutex.unlock();

	IEEE1451dot5_TIM::getInstance()->sendMessage(0, buffer, size);
	delete[] buffer;
}

int IEEE1451TemperatureSensor::execute()
{
	db<IEEE1451TemperatureSensor>(INF) << "Temperature sensor execute thread created\n";

	executeThread = Thread::self();
	IEEE1451dot5_TIM::getInstance()->connect();
	executeThread->suspend();
	int count = 1;

	while (true)
	{
		if ((tim_im) && (!polling) && (pos == 0))
			IEEE1451dot5_TIM::getInstance()->disconnect();

		db<IEEE1451TemperatureSensor>(INF) << "Collecting data (pos = " << pos << ")...\n";

		//while (!temperature.enable());
		//while (!temperature.data_ready());

		dataSetMutex.lock();
		dataSet[pos] = count++; //(float) temperature.sample();
		pos = (pos + 1) % DATASET_SIZE;
		dataSetMutex.unlock();

		//temperature.disable();
		Alarm::delay(READ_INTERVAL);

		if ((tim_im) && (!polling) && (pos == 0))
			IEEE1451dot5_TIM::getInstance()->connect();

		if ((tim_im) && (pos == 0))
			sendDataSet();
	}

	return 0;
}

/* IEEE 1451.0 (2007) -> Cap. 5.10
   -> TIM-Initiated Message
      Sampling modes: Continuous sampling mode
      Data transmission mode: Streaming when a buffer is full mode
         Streaming operation = Continuous sampling + Streaming when a buffer is full

   -> Polling
      Sampling modes: Continuous sampling mode
      Data transmission mode: Only when commanded mode */

__END_SYS
