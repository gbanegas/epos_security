#ifndef __sip_transactions_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_transactions_h

#include <sip_defs.h>
#include <sip_manager.h>
#include <sip_message.h>

__BEGIN_SYS


class UserAgentClient;
class UserAgentServer;

class SipTransaction
{
private:
	friend class UserAgent;

protected:
	SipTransportLayer *transport;
	UserAgent *ua;

	Simple_List<SipTransaction>::Element link;

public:
	SipTransaction(UserAgent *ua);
	virtual ~SipTransaction() {};

	virtual SipTransactionType getTransactionType() = 0;
	virtual SipTransaction *matchingTransaction(SipMessage *msg) = 0;
	void clear();
};

//-------------------------------------------

class SipTransactionClientInvite : public SipTransaction
{
private:
	enum State
	{
		sttIdle,
		sttCalling,
		sttProceeding,
		sttCompleted,
		//sttTerminated
	} curState;

	SipRequestInvite *requestMsg;

public:
	SipTransactionClientInvite(UserAgent *ua);
	~SipTransactionClientInvite();

	void sendAck(SipResponse *Msg);

	void sendInvite(SipRequestInvite *Msg);
	void receive1xx(SipResponse *Msg);
	void receive2xx(SipResponse *Msg);
	void receive3xx6xx(SipResponse *Msg);
	void transportError();

	void timerAExpired();
	void timerBExpired();
	void timerDExpired();

	static void timerACallback(SipTransaction *p) { ((SipTransactionClientInvite *) p)->timerAExpired(); };
	static void timerBCallback(SipTransaction *p) { ((SipTransactionClientInvite *) p)->timerBExpired(); };
	static void timerDCallback(SipTransaction *p) { ((SipTransactionClientInvite *) p)->timerDExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_CLIENT_INVITE; };
	SipTransaction *matchingTransaction(SipMessage *msg);
};

//-------------------------------------------

class SipTransactionClientNonInvite : public SipTransaction
{
private:
	enum State
	{
		sttIdle,
		sttTrying,
		sttProceeding,
		sttCompleted,
		//sttTerminated
	} curState;

	SipRequest *requestMsg;

public:
	SipTransactionClientNonInvite(UserAgent *ua);
	~SipTransactionClientNonInvite();

	//void copyRequest(SipRequest *Msg);

	void sendRequest(SipRequest *Msg);
	void receive1xx(SipResponse *Msg);
	void receive2xx6xx(SipResponse *Msg);
	void transportError();

	void timerEExpired();
	void timerFExpired();
	void timerKExpired();

	static void timerECallback(SipTransaction *p) { ((SipTransactionClientNonInvite *) p)->timerEExpired(); };
	static void timerFCallback(SipTransaction *p) { ((SipTransactionClientNonInvite *) p)->timerFExpired(); };
	static void timerKCallback(SipTransaction *p) { ((SipTransactionClientNonInvite *) p)->timerKExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_CLIENT_NON_INVITE; };
	SipTransaction *matchingTransaction(SipMessage *msg);
};

//-------------------------------------------

class SipTransactionServerInvite : public SipTransaction
{
private:
	enum State
	{
		sttIdle,
		sttProceeding,
		sttCompleted,
		sttConfirmed,
		//sttTerminated
	} curState;

	SipRequestInvite *requestMsg;
	SipResponse *lastResponse;

public:
	SipTransactionServerInvite(UserAgent *ua);
	~SipTransactionServerInvite();

	void receiveInvite(SipRequestInvite *Msg);
	void receiveAck(SipRequestAck *Msg);
	void send1xx(SipResponse *Msg);
	void send2xx(SipResponse *Msg);
	void send3xx6xx(SipResponse *Msg);
	void transportError();

	void timerGExpired();
	void timerHExpired();
	void timerIExpired();

	static void timerGCallback(SipTransaction *p) { ((SipTransactionServerInvite *) p)->timerGExpired(); };
	static void timerHCallback(SipTransaction *p) { ((SipTransactionServerInvite *) p)->timerHExpired(); };
	static void timerICallback(SipTransaction *p) { ((SipTransactionServerInvite *) p)->timerIExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_SERVER_INVITE; };
	SipTransaction *matchingTransaction(SipMessage *msg);
};

//-------------------------------------------

class SipTransactionServerNonInvite : public SipTransaction
{
private:
	enum State
	{
		sttIdle,
		sttTrying,
		sttProceeding,
		sttCompleted,
		//sttTerminated
	} curState;

	SipRequest *requestMsg;
	SipResponse *lastResponse;

public:
	SipTransactionServerNonInvite(UserAgent *ua);
	~SipTransactionServerNonInvite();

	//void copyRequest(SipRequest *Msg);

	void receiveRequest(SipRequest *Msg);
	void send1xx(SipResponse *Msg);
	void send2xx6xx(SipResponse *Msg);
	void transportError();

	void timerJExpired();

	static void timerJCallback(SipTransaction *p) { ((SipTransactionServerNonInvite *) p)->timerJExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_SERVER_NON_INVITE; };
	SipTransaction *matchingTransaction(SipMessage *msg);
};

__END_SYS

#endif
