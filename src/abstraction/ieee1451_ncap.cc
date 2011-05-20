#include <ieee1451_ncap.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <ieee1451_ncap_application.h>

__BEGIN_SYS


//-------------------------------------------

TEDS_NCAP::TEDS_NCAP(char id, const char *teds, unsigned int length, bool subblock)
	: id(id), link(this)
{
	unsigned int begin, end;

	if (subblock)
	{
		begin = 0;
		end = length;
	}else
	{
		begin = 4;
		end = length - 2;
	}

	for (unsigned int i = begin; i < end; )
	{
		unsigned short type = teds[i++] & 0xff;
		unsigned short len = teds[i++] & 0xff;

		char *value = new char[len];
		for (unsigned int j = 0; j < len; j++, i++)
			value[j] = teds[i];

		TLV *tlv = new TLV(type, len, value);
		tlvs.insert(&tlv->link);
	}
}

TEDS_NCAP::~TEDS_NCAP()
{
	Simple_List<TLV>::Iterator it = tlvs.begin();
	while (it != tlvs.end())
	{
		Simple_List<TLV>::Element *el = it++;
		TLV *tlv = el->object();
		tlvs.remove(&tlv->link);
		delete tlv;
	}
}

TLV *TEDS_NCAP::getTLV(char type)
{
	Simple_List<TLV>::Iterator it = tlvs.begin();
	while (it != tlvs.end())
	{
		TLV *tlv = it->object();
		it++;

		if (tlv->type == type)
			return tlv;
	}

	return 0;
}

//-------------------------------------------

//const unsigned short IEEE1451TIMChannel::TIM_CHANNEL_NUMBER = 0;

IEEE1451Channel::~IEEE1451Channel()
{
	Simple_List<TEDS_NCAP>::Iterator it = tedss.begin();
	while (it != tedss.end())
	{
		Simple_List<TEDS_NCAP>::Element *el = it++;
		TEDS_NCAP *teds = el->object();
		tedss.remove(&teds->link);
		delete teds;
	}
}

TEDS_NCAP *IEEE1451Channel::getTEDS(char id)
{
	Simple_List<TEDS_NCAP>::Iterator it = tedss.begin();
	while (it != tedss.end())
	{
		TEDS_NCAP *teds = it->object();
		it++;

		if (teds->id == id)
			return teds;
	}

	return 0;
}

//-------------------------------------------

IEEE1451dot0_NCAP *IEEE1451dot0_NCAP::dot0 = 0;

IEEE1451dot0_NCAP::IEEE1451dot0_NCAP()
{
	application = 0;
	IEEE1451dot5_NCAP::getInstance();
}

IEEE1451dot0_NCAP *IEEE1451dot0_NCAP::getInstance()
{
	if (!dot0)
		dot0 = new IEEE1451dot0_NCAP();
	return dot0;
}

char *IEEE1451dot0_NCAP::createCommand(unsigned short channelNumber, unsigned short command, const char *args, unsigned int length)
{
	unsigned int size = sizeof(Command) + length;
	char *buffer = new char[size];

	Command *cmd = (Command *) buffer;
	cmd->channelNumber = channelNumber;
	cmd->command = command;
	cmd->length = length;

	for (unsigned short i = 0; i < length; i++)
		buffer[i + sizeof(Command)] = args[i];

	return buffer;
}

unsigned short IEEE1451dot0_NCAP::sendCommand(const IP::Address &destination, const char *message, unsigned int length)
{
	static unsigned int idGenerator = 1;
	unsigned short transId = idGenerator++;

	IEEE1451dot5_NCAP::getInstance()->sendMessage(transId, destination, message, length);
	return transId;
}

void IEEE1451dot0_NCAP::timConnected(const IP::Address &address)
{
	application->reportTimConnected(address);
}

void IEEE1451dot0_NCAP::timDisconnected(const IP::Address &address)
{
	application->reportTimDisconnected(address);
}

void IEEE1451dot0_NCAP::receiveMessage(const IP::Address &address, unsigned short transId, const char *message, unsigned int length)
{
	if (transId == 0)
		application->reportTimInitiatedMessage(address, message, length);
	else
		application->reportCommandReply(address, transId, message, length);
}

void IEEE1451dot0_NCAP::errorOnSend(int errorCode, unsigned short transId)
{
	//Dependendo do erro, avisar que desconectou também!
	application->reportError(transId, errorCode);
}

//-------------------------------------------

IEEE1451dot5_NCAP *IEEE1451dot5_NCAP::dot5 = 0;

IEEE1451dot5_NCAP::IEEE1451dot5_NCAP() : tcp(IP::instance())
{
	db<IEEE1451dot5_NCAP>(INF) << "IEEE1451dot5 listening...\n";
	MyServerSocket *socket = new MyServerSocket(&tcp);
	sockets.insert(&socket->link);
}

IEEE1451dot5_NCAP::~IEEE1451dot5_NCAP()
{
	Simple_List<MyServerSocket>::Iterator it = sockets.begin();
	while (it != sockets.end())
	{
		Simple_List<MyServerSocket>::Element *el = it++;
		MyServerSocket *socket = el->object();
		sockets.remove(&socket->link);
		delete socket;
	}
}

IEEE1451dot5_NCAP *IEEE1451dot5_NCAP::getInstance()
{
	if (!dot5)
		dot5 = new IEEE1451dot5_NCAP();
	return dot5;
}

IEEE1451dot5_NCAP::MyServerSocket *IEEE1451dot5_NCAP::getSocket(const IP::Address &addr)
{
	Simple_List<MyServerSocket>::Iterator it = sockets.begin();
	while (it != sockets.end())
	{
		MyServerSocket *socket = it->object();
		it++;

		if (socket->remote().ip() == addr)
			return socket;
	}

	return 0;
}

void IEEE1451dot5_NCAP::sendMessage(unsigned short transId, const IP::Address &destination, const char *message, unsigned int length)
{
	db<IEEE1451dot5_NCAP>(INF) << "Sending message (transId=" << transId << ")...\n";

	MyServerSocket *socket = getSocket(destination);
	if (!socket)
	{
		db<IEEE1451dot5_NCAP>(INF) << "Failed to send message (transId=" << transId << ")\n";
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

TCP::Socket *IEEE1451dot5_NCAP::MyServerSocket::incoming(const TCP::Address &from)
{
	db<IEEE1451dot5_NCAP::MyServerSocket>(INF) << "Server socket incoming\n";
	MyServerSocket *socket = new MyServerSocket(*this);
	IEEE1451dot5_NCAP::getInstance()->sockets.insert(&socket->link);
	return static_cast<TCP::Socket *>(socket);
}

void IEEE1451dot5_NCAP::MyServerSocket::connected()
{
	db<IEEE1451dot5_NCAP::MyServerSocket>(INF) << "Server socket connected\n";
	IEEE1451dot0_NCAP::getInstance()->timConnected(remote());
}

void IEEE1451dot5_NCAP::MyServerSocket::closed()
{
	db<IEEE1451dot5_NCAP::MyServerSocket>(INF) << "Server socket closed\n";
	IEEE1451dot0_NCAP::getInstance()->timDisconnected(remote());
	IEEE1451dot5_NCAP::getInstance()->sockets.remove(&link);
	delete this;
}

void IEEE1451dot5_NCAP::MyServerSocket::received(const char *data, u16 size)
{
	db<IEEE1451dot5_NCAP::MyServerSocket>(INF) << "Server socket received message\n";
	IEEE1451_Packet *in = (IEEE1451_Packet *) data;
	const char *msg = data + sizeof(IEEE1451_Packet);

	if (in->len > 0)
		IEEE1451dot0_NCAP::getInstance()->receiveMessage(remote(), in->transId, msg, in->len);
}

void IEEE1451dot5_NCAP::MyServerSocket::closing()
{
	db<IEEE1451dot5_NCAP::MyServerSocket>(INF) << "Server socket closing\n";
	close();
}

__END_SYS
