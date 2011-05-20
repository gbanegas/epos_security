#ifndef __ieee1451_ncap_application_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __ieee1451_ncap_application_h

#include <ieee1451_ncap.h>
#include <ieee1451_objects.h>
#include <sip_defs.h>
#include <sip_manager.h>
#include <sip_user_agent.h>
#include <utility/list.h>

__BEGIN_SYS


class TEDSRetriever;

class NCAPApplication
{
private:
	friend class TEDSRetriever;

	struct TIMCache
	{
		IEEE1451TIMChannel *tim;
		UserAgent *ua;
		Simple_List<TIMCache>::Element link;

		TIMCache(IEEE1451TIMChannel *tim, UserAgent *ua) : tim(tim), ua(ua), link(this) {};
		~TIMCache() { delete tim; delete ua; };
	};

	Simple_List<TIMCache> cache;
	Simple_List<TEDSRetriever> retrievers;

	static NCAPApplication *app;
	NCAPApplication();

public:
	~NCAPApplication();

	static NCAPApplication *getInstance();

	TIMCache *getTIMCache(const IP::Address &address);
	TIMCache *getTIMCache(const char *uri);
	TEDSRetriever *getRetriever(unsigned short transId);

	void updateTIM(const IP::Address &address);
	void updateTIMCompleted(TEDSRetriever *retriever, IEEE1451TIMChannel *tim, IP::Address address);

	void reportTimConnected(const IP::Address &address);
	void reportTimDisconnected(const IP::Address &address);
	void reportError(unsigned short transId, int errorCode);
	void reportCommandReply(const IP::Address &address, unsigned short transId, const char *message, unsigned int length);
	void reportTimInitiatedMessage(const IP::Address &address, const char *message, unsigned int length);

	void readTemperature(const IP::Address &address, const char *buffer);

	unsigned short sendOperate(const IP::Address &address, unsigned short channelNumber);
	unsigned short sendIdle(const IP::Address &address, unsigned short channelNumber);
	unsigned short sendReadTEDS(const IP::Address &address, unsigned short channelNumber, char tedsId);
	unsigned short sendReadDataSet(const IP::Address &address, unsigned short channelNumber);

	void sendSipMessage(UserAgent *ua, const char *data);
	void sendSipNotify(UserAgent *ua, SipSubscriptionState state, SipPidfXmlBasicElement pidfXml);

	//static int readDataSetThread(NCAPApplication *ncap, IP::Address address, IEEE1451TIMChannel *tim);
	static int messageCallback(SipEventCallback event, UserAgent *ua, const char *remote);
};

//-------------------------------------------

class TEDSRetriever
{
private:
	friend class NCAPApplication;

	enum State
	{
		meta_teds					= 0,
		tim_transducer_name_teds	= 1,
		phy_teds					= 2,
		transducer_channel_teds		= 3,
		transducer_name_teds		= 4
	};

	IP::Address address;
	NCAPApplication *app;
	IEEE1451TransducerChannel *transducer;
	IEEE1451TIMChannel *tim;

	short state;
	char tedsId;
	unsigned short lastTransId;

	Simple_List<TEDSRetriever>::Element link;

public:
	TEDSRetriever(const IP::Address &address, NCAPApplication *app);
	~TEDSRetriever() {};

	void repeat() { execute(); };
	void process(const char *message, unsigned int length);
	void execute();
};

__END_SYS

#endif
