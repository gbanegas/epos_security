#ifndef __sip_manager_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_manager_h

#include <sip_defs.h>
#include <sip_message.h>
#include <mutex.h>
#include <ip/ip.h>
#include <ip/udp.h>
#include <utility/list.h>

__BEGIN_SYS

#define SIP_PORT 5060

class SipTransportLayer
{
private:
	char *hostIP;
	int hostPort;

	char sendBuffer[MAX_MSG_SIZE + 1];
	UDP udp;

public:
	SipTransportLayer() : udp(IP::instance()), socket(&udp) { hostIP = 0; hostPort = 0; sendBuffer[0] = 0; };
	~SipTransportLayer() { if (hostIP) delete hostIP; };

	bool init();

	int sendMessage(SipMessage *message);
	int sendData(const char *destination, int port, const char *data, int length);

	const char *getHostIP() { return hostIP; };
	int getHostPort() { return hostPort; };

private:
	class MySocket : public UDP::Socket
	{
	public:
		MySocket(UDP *udp) : UDP::Socket(udp, UDP::Address(IP::instance()->address(), SIP_PORT), UDP::Address(Traits<IP>::BROADCAST, SIP_PORT)) {};
		virtual ~MySocket() {};

		void received(const UDP::Address &src, const char *data, unsigned int size);
	} socket;
};

//-------------------------------------------

class UserAgent;
typedef int (*USER_CALLBACK)(SipEventCallback event, UserAgent *ua, const char *remote);

class SipManager
{
private:
	Simple_List<SipMessage> messages;
	Mutex messagesMutex;

	Thread *threadReceiveMessage;

	SipTransportLayer transport;
	Simple_List<UserAgent> ua;

	static bool terminated;
	static SipManager *instance;
	SipManager();

public:
	~SipManager();

	static SipManager *getInstance();
	static void deleteInstance();
	static bool isTerminated() { return terminated; };

	bool init();

	static int receiveMessageThread();

	static USER_CALLBACK callback;
	static void registerUserHandler(USER_CALLBACK callback) { SipManager::callback = callback; };

	UserAgent *createUserAgent(const char *uri);
	UserAgent *getUserAgent(const char *uri);

	SipTransportLayer *getTransport() { return &transport; };

	void addMessageList(SipMessage *msg);
	SipMessage *getMessageList();

	static void random(char *buffer);
};

__END_SYS

#endif
