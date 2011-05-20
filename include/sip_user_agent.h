#ifndef __sip_user_agent_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_user_agent_h

#include <sip_defs.h>
#include <sip_header.h>
#include <sip_message.h>
#include <sip_transactions.h>
#include <utility/list.h>

__BEGIN_SYS


class SipDialog
{
private:
	friend class UserAgent;
	friend class UserAgentClient;
	friend class UserAgentServer;

	SipMessageType type;

	/*enum State
	{
		sttIdle,
		//sttEarly,
		sttConfirmed
	} curState;*/

	char *callID;
	char *localTag;
	char *remoteTag;
	unsigned int localSequenceNumber;
	unsigned int remoteSequenceNumber;
	char *localURI;
	char *remoteURI;
	char *remoteTarget;
	Simple_List<SipHeader/*Route*/> routeSet;

	Simple_List<SipDialog>::Element link;

public:
	SipDialog(SipMessageType type);
	~SipDialog();

	void setDialog(const char *callID, const char *localTag, const char *remoteTag,
			unsigned int localSequenceNumber, unsigned int remoteSequenceNumber,
			const char *localURI, const char *remoteURI, const char *remoteTarget);
	void setRemoteTarget(const char *remoteTarget);
	void addRouteBack(SipHeaderRoute *route);
	void addRouteFront(SipHeaderRoute *route);
	void clearRouteSet();

	int getRouteSetSize() { return (int) routeSet.size(); };
	SipHeaderRoute *getRoute(int pos);
};

//-------------------------------------------

class SipSubscription
{
private:
	friend class UserAgent;
	friend class UserAgentClient;
	friend class UserAgentServer;

	SipEventPackage eventType;
	char *eventId;

	//Functor_Handler *timerHandler;
	//Alarm *timerAlarm;

public:
	SipSubscription() : eventType(SIP_EVENT_PACKAGE_INVALID), eventId(0)/*, timerHandler(0), timerAlarm(0)*/ {};
	~SipSubscription() { clear(); };

	void setSubscription(SipEventPackage eventType, const char *eventId);
	//void startTimer(void *p);
	//void stopTimer();

	bool isActive() { return eventType != SIP_EVENT_PACKAGE_INVALID; };
	void clear();
};

//-------------------------------------------

class UserAgentClient
{
private:
	friend class UserAgent;
	UserAgent *ua;

public:
	UserAgentClient(UserAgent *ua) : ua(ua) {};
	~UserAgentClient() {};

private:
	SipRequest *createRequest(SipMessageType msgType, SipDialog *dialog = 0, const char *to = 0, SipMessage *invite = 0);

public:
	SipRequestAck *createAck(const char *to, SipRequestInvite *invite);
	SipRequestBye *createBye(const char *to);
	SipRequestInvite *createInvite(const char *to);
	SipRequestMessage *createMessage(const char *to, const char *data);
	SipRequestNotify *createNotify(const char *to, SipSubscriptionState state, SipPidfXmlBasicElement pidfXmlElement, unsigned int expires = 0);
	SipRequestSubscribe *createSubscribe(const char *to) { return 0; };
	void sendRequest(SipRequest *request);

	bool receiveMsg(SipResponse *response);
	bool receiveMsg(SipRequest *request, SipResponse *response, SipTransaction *transaction);
	//bool receive1xx(SipRequest *request, SipResponse *response, SipTransaction *transaction, SipDialog *dialog) { return true; };
	bool receive2xx(SipRequest *request, SipResponse *response, SipTransaction *transaction, SipDialog *dialog);
	bool receive3xx6xx(SipRequest *request, SipResponse *response, SipTransaction *transaction, SipDialog *dialog);

	SipDialog *createDialog(SipRequest *request, SipResponse *response);
};

//-------------------------------------------

class UserAgentServer
{
private:
	friend class UserAgent;
	UserAgent *ua;

public:
	UserAgentServer(UserAgent *ua) : ua(ua) {}
	~UserAgentServer() {};

private:
	SipResponse *createResponse(int statusCode, SipRequest *request);
	void sendResponse(SipResponse *response, SipMessageType requestType, SipTransaction *transaction);

public:
	bool receiveMsg(SipRequest *request);
	bool receiveMsg(SipRequest *request, SipTransaction *transaction);
	//bool receiveAck(SipRequestAck *request, SipTransaction *transaction, SipDialog *dialog) { return true; };
	bool receiveBye(SipRequestBye *request, SipTransaction *transaction, SipDialog *dialog);
	bool receiveInvite(SipRequestInvite *request, SipTransaction *transaction, SipDialog *dialog);
	bool receiveMessage(SipRequestMessage *request, SipTransaction *transaction, SipDialog *dialog);
	//bool receiveNotify(SipRequestNotify *request, SipTransaction *transaction, SipDialog *dialog) { return true; };
	bool receiveSubscribe(SipRequestSubscribe *request, SipTransaction *transaction, SipDialog *dialog);

	SipDialog *createDialog(SipRequest *request, SipResponse *response);
};

//-------------------------------------------

class UserAgent
{
private:
	friend class UserAgentClient;
	friend class UserAgentServer;
	friend class SipTransactionClientInvite;
	friend class SipTransactionClientNonInvite;
	friend class SipTransactionServerInvite;
	friend class SipTransactionServerNonInvite;
	friend class SipManager;

	UserAgentClient uac;
	UserAgentServer uas;

	Simple_List<SipDialog> dialogs;
	SipSubscription subscription;
	Simple_List<SipTransaction> transactions;

	char *uri;
	const char *textReceived;

	int timerValues[SIP_TIMER_COUNT];
	Functor_Handler<SipTransaction> *timerHandlers[SIP_TIMER_COUNT];
	Alarm *timerAlarms[SIP_TIMER_COUNT];

	Simple_List<UserAgent>::Element link;

public:
	UserAgent(const char *uri);
	~UserAgent();

	UserAgentClient *getUserAgentClient() { return &uac; };
	UserAgentServer *getUserAgentServer() { return &uas; };

	SipDialog *matchingDialog(SipMessage *msg, SipMessageType type);
	SipDialog *matchingDialog(const char *to, SipMessageType type);
	SipTransaction *matchingTransaction(SipMessage *msg);

	const char *getUri() { return uri; };
	const char *getTextReceived() { return textReceived; };
	bool hasSubscription() { return subscription.isActive(); };
	const char *getSubscriber();

	void addTransaction(SipTransaction *transaction) { transactions.insert(&transaction->link); };
	void removeTransaction(SipTransaction *transaction) { transactions.remove(&transaction->link); delete transaction; };
	SipDialog *addDialog(SipMessageType type);
	void removeDialog(SipDialog *dialog) { dialogs.remove(&dialog->link); delete dialog; };

	int getTimerValue(SipTimer timer) { return timerValues[timer]; };
	void setTimerValue(SipTimer timer, int timerValue) { timerValues[timer] = timerValue; };
	void startTimer(SipTimer timer, SipTransaction *p);
	void stopTimer(SipTimer timer);
};

//-------------------------------------------

class SendRTP
{
private:
	unsigned int sequence;
	unsigned int timestamp;

	UDP udp;

public:
	SendRTP() : sequence(0x016a), timestamp(0x00207a10), udp(IP::instance()), socket(&udp) {};
	~SendRTP() {};

	void sendData(const char *destination, unsigned short port, char *data, unsigned int size);

private:
	class MySocket : public UDP::Socket
	{
	public:
		MySocket(UDP *udp) : UDP::Socket(udp, UDP::Address(IP::instance()->address(), 8000), UDP::Address(Traits<IP>::BROADCAST, 8000)) {};
		virtual ~MySocket() {};

		void received(const UDP::Address &src, const char *data, unsigned int size)
		{
			db<SendRTP::MySocket>(INF) << "SendRTP::MySocket::received..\n";
		};
	} socket;
};

__END_SYS

#endif
