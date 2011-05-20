#ifndef __ieee1451_ncap_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __ieee1451_ncap_h

#include <ieee1451_objects.h>
#include <thread.h>
#include <ip/ip.h>
#include <ip/tcp.h>
#include <utility/list.h>
#include <utility/malloc.h>

__BEGIN_SYS


class IEEE1451Channel;
class NCAPApplication;
class TEDS_NCAP;

class TLV
{
private:
	friend class TEDS_NCAP;
	friend class NCAPApplication;

	char type;
	unsigned short length;
	char *value;

	Simple_List<TLV>::Element link;

public:
	TLV(char type, unsigned short length, char *value) : type(type), length(length), value(value), link(this) {};
	~TLV() { delete value; };
};

//-------------------------------------------

class TEDS_NCAP
{
private:
	friend class IEEE1451Channel;

	char id;
	Simple_List<TLV> tlvs;

	Simple_List<TEDS_NCAP>::Element link;

public:
	TEDS_NCAP(char id, const char *teds, unsigned int length, bool subblock = false);
	~TEDS_NCAP();

	TLV *getTLV(char type);
};

//-------------------------------------------

class IEEE1451Channel
{
private:
	friend class NCAPApplication;

	IP::Address address;
	Simple_List<TEDS_NCAP> tedss;

public:
	IEEE1451Channel(const IP::Address &address) : address(address) {};
	virtual ~IEEE1451Channel();

	//virtual unsigned short getChannelNumber() = 0;

	TEDS_NCAP *getTEDS(char id);
	void addTEDS(TEDS_NCAP *teds) { tedss.insert(&teds->link); };
};

//-------------------------------------------

class IEEE1451TransducerChannel : public IEEE1451Channel
{
private:
	unsigned short channelNumber;

public:
	IEEE1451TransducerChannel(const IP::Address &address, short channelNumber) : IEEE1451Channel(address) { this->channelNumber = channelNumber; };
	~IEEE1451TransducerChannel() {};

	unsigned short getChannelNumber() { return channelNumber; };
};

//-------------------------------------------

class IEEE1451TIMChannel : public IEEE1451Channel
{
private:
	//static const unsigned short TIM_CHANNEL_NUMBER;
	IEEE1451TransducerChannel *transducer;
	bool connected;

public:
	IEEE1451TIMChannel(const IP::Address &address, IEEE1451TransducerChannel *transducer) : IEEE1451Channel(address), transducer(transducer), connected(false) {};
	~IEEE1451TIMChannel() { delete transducer; };

	IEEE1451TransducerChannel *getTransducer() { return transducer; };
	//unsigned short getChannelNumber() { return TIM_CHANNEL_NUMBER; };
	void connect() { connected = true; };
	void disconnect() { connected = false; };
	bool isConnected() { return connected; };
	bool isDisconnected() { return connected; };
};

//-------------------------------------------

class IEEE1451dot0_NCAP
{
private:
	NCAPApplication *application;

	static IEEE1451dot0_NCAP *dot0;
	IEEE1451dot0_NCAP();

public:
	~IEEE1451dot0_NCAP() {};

	static IEEE1451dot0_NCAP *getInstance();

	char *createCommand(unsigned short channelNumber, unsigned short command, const char *args = 0, unsigned int length = 0);
	unsigned short sendCommand(const IP::Address &destination, const char *message, unsigned int length);

	void timConnected(const IP::Address &address);
	void timDisconnected(const IP::Address &address);
	void receiveMessage(const IP::Address &address, unsigned short transId, const char *message, unsigned int length);
	void errorOnSend(int errorCode, unsigned short transId);

	void setApplication(NCAPApplication *application) { this->application = application; };
};

//-------------------------------------------

class IEEE1451dot5_NCAP
{
private:
	class MyServerSocket;

	TCP tcp;
	Simple_List<MyServerSocket> sockets;

	static IEEE1451dot5_NCAP *dot5;
	IEEE1451dot5_NCAP();

public:
	~IEEE1451dot5_NCAP();

	static IEEE1451dot5_NCAP *getInstance();

	MyServerSocket *getSocket(const IP::Address &addr);
	void sendMessage(unsigned short transId, const IP::Address &destination, const char *message, unsigned int length);

private:
	class MyServerSocket : public TCP::ServerSocket
	{
	private:
		friend class IEEE1451dot5_NCAP;

		Simple_List<MyServerSocket>::Element link;

	public:
		MyServerSocket(TCP *tcp) : TCP::ServerSocket(tcp, TCP::Address(IP::instance()->address(), IEEE1451_PORT)), link(this) {};
		MyServerSocket(const MyServerSocket &socket) : TCP::ServerSocket(socket), link(this) {};
		~MyServerSocket() {};

		TCP::Socket *incoming(const TCP::Address &from);
		void connected();
		void closed();
		void received(const char *data, u16 size);
		void closing();
		void sent(u16 size) {};
		void error(short errorCode) {};
	};
};

__END_SYS

#endif
