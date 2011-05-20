#include <ieee1451_tim.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS


IEEE1451Transducer::IEEE1451Transducer()
{
	IEEE1451dot0_TIM::getInstance()->setTransducer(this);
}

void IEEE1451Transducer::receiveMessage(unsigned short transId, const char *message, unsigned int size)
{
	Command *cmd = (Command *) message;
	const char *buffer = message + sizeof(Command);

	switch (cmd->command)
	{
		/*case COMMAND_CLASS_QUERY_TEDS:
		case COMMAND_CLASS_UPDATE_TEDS:
		{
			TEDS *teds = getTEDS(buffer[0]);
			TEDSQueryReply reply;
			reply.header.length = 12;
			if (teds)
			{
				reply.maxSize = 0;
				reply.checksum = *((unsigned short *) teds->payload);
				reply.size = teds->size;
				reply.status = teds->status;
				reply.atributes = teds->atributes;
				reply.header.success = true;
			}else
			{
				reply.maxSize = 0;
				reply.checksum = 0;
				reply.size = 0;
				reply.status = 0;
				reply.atributes = 0x40;
				reply.header.success = false;
			}

			IEEE1451dot5_TIM::getInstance()->sendMessage(transId, (char *) &reply, sizeof(reply));
			break;
		}*/

		case COMMAND_CLASS_READ_TEDS_SEGMENT:
		{
			TEDS_TIM *teds = getTEDS(buffer[0]);
			if (teds)
			{
				unsigned int size = sizeof(TEDSReadReply) + teds->size;
				char *reply = new char[size];

				TEDSReadReply *readReply = (TEDSReadReply *) reply;
				readReply->header.success = true;
				readReply->header.length = teds->size + sizeof(readReply->offset);
				readReply->offset = 0;

				for (unsigned short i = 0; i < teds->size; i++)
					reply[i + sizeof(TEDSReadReply)] = teds->payload[i];

				IEEE1451dot5_TIM::getInstance()->sendMessage(transId, reply, size);
				delete[] reply;
			}else
			{
				TEDSReadReply reply;
				reply.header.success = false;
				reply.header.length = sizeof(reply.offset);;
				reply.offset = 0;

				IEEE1451dot5_TIM::getInstance()->sendMessage(transId, (char *) &reply, sizeof(TEDSReadReply));
			}

			break;
		}

		case COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT:
		{
			unsigned int offset = *((unsigned int *) buffer);
			readDataSet(transId, offset);
			break;
		}

		case COMMAND_CLASS_TRANSDUCER_CHANNEL_OPERATE:
			start();
			break;

		case COMMAND_CLASS_TRANSDUCER_CHANNEL_IDLE:
			stop();
			break;

		default:
			db<IEEE1451dot0_TIM>(INF) << "Received invalid message\n";
			break;
	}
}

//-------------------------------------------

IEEE1451dot0_TIM *IEEE1451dot0_TIM::dot0 = 0;

IEEE1451dot0_TIM::IEEE1451dot0_TIM()
{
	metaArray = new char[40];
	metaArray[0] = 0x00; metaArray[1] = 0x00; metaArray[2] = 0x00; metaArray[3] = 0x24; metaArray[4] = 0x03; metaArray[5] = 0x04; metaArray[6] = 0x00; metaArray[7] = 0x01; metaArray[8] = 0x01; metaArray[9] = 0x01; metaArray[10] = 0x04; metaArray[11] = 0x0a; metaArray[12] = 0x00; metaArray[13] = 0x00; metaArray[14] = 0x00; metaArray[15] = 0x00; metaArray[16] = 0x00; metaArray[17] = 0x00; metaArray[18] = 0x00; metaArray[19] = 0x00; metaArray[20] = 0x00; metaArray[21] = 0x00; metaArray[22] = 0x0a; metaArray[23] = 0x04; metaArray[24] = 0x41; metaArray[25] = 0x20; metaArray[26] = 0x00; metaArray[27] = 0x00; metaArray[28] = 0x0c; metaArray[29] = 0x04; metaArray[30] = 0x00; metaArray[31] = 0x00; metaArray[32] = 0x00; metaArray[33] = 0x00; metaArray[34] = 0x0d; metaArray[35] = 0x02; metaArray[36] = 0x00; metaArray[37] = 0x01; metaArray[38] = 0xfe; metaArray[39] = 0xb6; //checksum errado
	META_TEDS = new TEDS_TIM(metaArray, 40);

	timUtnArray = new char[26];
	timUtnArray[0] = 0x00; timUtnArray[1] = 0x00; timUtnArray[2] = 0x00; timUtnArray[3] = 0x16; timUtnArray[4] = 0x03; timUtnArray[5] = 0x04; timUtnArray[6] = 0x00; timUtnArray[7] = 0x0c; timUtnArray[8] = 0x01; timUtnArray[9] = 0x01; timUtnArray[10] = 0x04; timUtnArray[11] = 0x01; timUtnArray[12] = 0x00; timUtnArray[13] = 0x05; timUtnArray[14] = 0x09; timUtnArray[15] = 0x45; timUtnArray[16] = 0x50; timUtnArray[17] = 0x4f; timUtnArray[18] = 0x53; timUtnArray[19] = 0x2f; timUtnArray[20] = 0x4d; timUtnArray[21] = 0x69; timUtnArray[22] = 0x63; timUtnArray[23] = 0x61; timUtnArray[24] = 0xff; timUtnArray[25] = 0xff; //checksum errado
	TIM_UTN_TEDS = new TEDS_TIM(timUtnArray, 26);

	phyArray = new char[25];
	phyArray[0] = 0x00; phyArray[1] = 0x00; phyArray[2] = 0x00; phyArray[3] = 0x15; phyArray[4] = 0x03; phyArray[5] = 0x04; phyArray[6] = 0x05; phyArray[7] = 0x0d; phyArray[8] = 0x01; phyArray[9] = 0x01; phyArray[10] = 0x0a; phyArray[11] = 0x01; phyArray[12] = 0xff; phyArray[13] = 0x0c; phyArray[14] = 0x02; phyArray[15] = 0x00; phyArray[16] = 0x01; phyArray[17] = 0x16; phyArray[18] = 0x01; phyArray[19] = 0x01; phyArray[20] = 0x28; phyArray[21] = 0x01; phyArray[22] = 0x01; phyArray[23] = 0xfe; phyArray[24] = 0x74; //checksum errado
	PHY_TEDS = new TEDS_TIM(phyArray, 25);

	transducer = 0;

	IEEE1451dot5_TIM::getInstance();
}

IEEE1451dot0_TIM::~IEEE1451dot0_TIM()
{
	if (metaArray)
		delete metaArray;
	if (timUtnArray)
		delete timUtnArray;
	if (phyArray)
		delete phyArray;

	if (META_TEDS)
		delete META_TEDS;
	if (TIM_UTN_TEDS)
		delete TIM_UTN_TEDS;
	if (PHY_TEDS)
		delete PHY_TEDS;
}

IEEE1451dot0_TIM *IEEE1451dot0_TIM::getInstance()
{
	if (!dot0)
		dot0 = new IEEE1451dot0_TIM();
	return dot0;
}

TEDS_TIM *IEEE1451dot0_TIM::getTEDS(char id)
{
	if (id == 0x01)
		return META_TEDS;
	else if (id == 0x0c)
		return TIM_UTN_TEDS;
	else if (id == 0x0d)
		return PHY_TEDS;
	return 0;
}

void IEEE1451dot0_TIM::receiveMessage(unsigned short transId, const char *message, unsigned int size)
{
	Command *cmd = (Command *) message;
	const char *buffer = message + sizeof(Command);

	if (cmd->channelNumber == ADDRESS_CLASS_TIM)
	{
		switch (cmd->command)
		{
			/*case COMMAND_CLASS_QUERY_TEDS:
			case COMMAND_CLASS_UPDATE_TEDS:
			{
				TEDS *teds = getTEDS(buffer[0]);
				TEDSQueryReply reply;
				reply.header.length = 12;
				if (teds)
				{
					reply.maxSize = 0;
					reply.checksum = *((unsigned short *) teds->payload);
					reply.size = teds->size;
					reply.status = teds->status;
					reply.atributes = teds->atributes;
					reply.header.success = true;
				}else
				{
					reply.maxSize = 0;
					reply.checksum = 0;
					reply.size = 0;
					reply.status = 0;
					reply.atributes = 0x40;
					reply.header.success = false;
				}

				dot5->sendMessage(transId, (char *) &reply, sizeof(reply));
				break;
			}*/

			case COMMAND_CLASS_READ_TEDS_SEGMENT:
			{
				TEDS_TIM *teds = getTEDS(buffer[0]);
				if (teds)
				{
					int size = sizeof(TEDSReadReply) + teds->size;
					char *reply = new char[size];

					TEDSReadReply *readReply = (TEDSReadReply *) reply;
					readReply->header.success = true;
					readReply->header.length = teds->size + sizeof(readReply->offset);
					readReply->offset = 0;

					for (unsigned short i = 0; i < teds->size; i++)
						reply[i + sizeof(TEDSReadReply)] = teds->payload[i];

					IEEE1451dot5_TIM::getInstance()->sendMessage(transId, reply, size);
					delete[] reply;
				}else
				{
					TEDSReadReply reply;
					reply.header.success = false;
					reply.header.length = sizeof(reply.offset);
					reply.offset = 0;

					IEEE1451dot5_TIM::getInstance()->sendMessage(transId, (char *) &reply, sizeof(TEDSReadReply));
				}
				break;
			}

			default:
				db<IEEE1451dot0_TIM>(INF) << "Received invalid message\n";
				break;
		}
	}else if (((cmd->channelNumber >= ADDRESS_CLASS_TRANSDUCER_CHANNEL_FIRST) && (cmd->channelNumber <= ADDRESS_CLASS_TRANSDUCER_CHANNEL_LAST)) ||
			(cmd->channelNumber == ADDRESS_CLASS_GLOBAL))
	{
		transducer->receiveMessage(transId, message, size);

	}else if ((cmd->channelNumber >= ADDRESS_CLASS_GROUP_FIRST) && (cmd->channelNumber <= ADDRESS_CLASS_GROUP_LAST))
	{
		//group
	}
}

//-------------------------------------------

IEEE1451dot5_TIM *IEEE1451dot5_TIM::dot5 = 0;
IP::Address IEEE1451dot5_TIM::NCAP_ADDRESS = IP::Address((unsigned long) 0xc0a80a01);

IEEE1451dot5_TIM::IEEE1451dot5_TIM() : tcp(IP::instance())
{
	socket = 0;
	connected = false;
}

IEEE1451dot5_TIM *IEEE1451dot5_TIM::getInstance()
{
	if (!dot5)
		dot5 = new IEEE1451dot5_TIM();
	return dot5;
}

void IEEE1451dot5_TIM::connect()
{
	//TODO: Voltar a receber dados da rede!

	db<IEEE1451dot5_TIM>(INF) << "Connecting...\n";
	if (socket)
		delete socket; //TODO
	socket = new MyClientSocket(&tcp, NCAP_ADDRESS);

	while (!connected)
		Alarm::delay(1000);
}

void IEEE1451dot5_TIM::disconnect()
{
	//TODO: Parar de receber dados da rede!

	db<IEEE1451dot5_TIM>(INF) << "Disconnecting...\n";
	if (socket)
		socket->close();

	while (connected)
		Alarm::delay(1000);

	delete socket;
	socket = 0;
}

void IEEE1451dot5_TIM::sendMessage(unsigned short transId, const char *message, unsigned int length)
{
	db<IEEE1451dot5_TIM>(INF) << "Sending message (transId=" << transId << ")...\n";

	if (!socket)
	{
		db<IEEE1451dot5_TIM>(INF) << "Failed to send message (transId=" << transId << ")\n";
		return;
	}

	unsigned int dataLength = sizeof(IEEE1451_Packet) + length;
	char *data = new char[dataLength];

	IEEE1451_Packet *out = (IEEE1451_Packet *) data;
	char *msg = data + sizeof(IEEE1451_Packet);

	out->transId = transId;
	out->len = length;
	memcpy(msg, message, length);

	socket->send(data, dataLength);
	delete data;
}

void IEEE1451dot5_TIM::MyClientSocket::connected()
{
	db<IEEE1451dot5_TIM::MyClientSocket>(INF) << "Client socket connected\n";
	IEEE1451dot5_TIM::getInstance()->connected = true;
}

void IEEE1451dot5_TIM::MyClientSocket::closed()
{
	db<IEEE1451dot5_TIM::MyClientSocket>(INF) << "Client socket closed\n";
	IEEE1451dot5_TIM::getInstance()->connected = false;
}

void IEEE1451dot5_TIM::MyClientSocket::received(const char *data, u16 size)
{
	db<IEEE1451dot5_TIM::MyClientSocket>(INF) << "Client socket received message\n";

	if (!(remote().ip() == NCAP_ADDRESS))
	{
		db<IEEE1451dot5_TIM::MyClientSocket>(INF) << "Message source is not NCAP_ADDRESS!\n";
		return;
	}

	if (!IEEE1451dot5_TIM::getInstance()->connected)
	{
		db<IEEE1451dot5_TIM::MyClientSocket>(INF) << "Dot5 not connected!\n";
		return;
	}

	IEEE1451_Packet *in = (IEEE1451_Packet *) data;
	const char *msg = data + sizeof(IEEE1451_Packet);

	if (in->len > 0)
		IEEE1451dot0_TIM::getInstance()->receiveMessage(in->transId, msg, in->len);
}

__END_SYS
