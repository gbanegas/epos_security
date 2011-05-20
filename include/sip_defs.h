#ifndef __sip_defs_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_defs_h

#include <system/config.h>
#include <utility/malloc.h>

__BEGIN_SYS


#define SIP_VERSION "SIP/2.0"
#define MAX_MSG_SIZE 1500
#define MAX_LINE 512
#define MAX_REASON_PHRASE_LEN 128


enum SipMessageType
{
	SIP_REQUEST_ACK,
	SIP_REQUEST_BYE,
	//SIP_REQUEST_CANCEL,
	SIP_REQUEST_INVITE,
	SIP_REQUEST_MESSAGE,
	SIP_REQUEST_NOTIFY, //Só suporta o envio!
	//SIP_REQUEST_OPTIONS,
	//SIP_REQUEST_REGISTER,
	SIP_REQUEST_SUBSCRIBE, //Só suporta a recepção!
	SIP_RESPONSE,
	SIP_MESSAGE_TYPE_INVALID
};

enum SipHeaderType
{
	//SIP_HEADER_ACCEPT,
	//SIP_HEADER_ACCEPT_ENCODING,
	//SIP_HEADER_ACCEPT_LANGUAGE,
	//SIP_HEADER_ALERT_INFO,
	SIP_HEADER_ALLOW,
	SIP_HEADER_ALLOW_EVENTS,
	//SIP_HEADER_AUTHENTICATION_INFO,
	//SIP_HEADER_AUTHORIZATION,
	SIP_HEADER_CALLID,
	//SIP_HEADER_CALL_INFO,
	SIP_HEADER_CONTACT,
	SIP_HEADER_CONTENT_DISPOSITION,
	//SIP_HEADER_CONTENT_ENCODING,
	//SIP_HEADER_CONTENT_LANGUAGE,
	SIP_HEADER_CONTENT_LENGTH,
	SIP_HEADER_CONTENT_TYPE,
	SIP_HEADER_CSEQ,
	//SIP_HEADER_DATE,
	//SIP_HEADER_ERROR_INFO,
	SIP_HEADER_EVENT,
	SIP_HEADER_EXPIRES,
	SIP_HEADER_FROM,
	//SIP_HEADER_IN_REPLY_TO,
	SIP_HEADER_MAX_FORWARDS,
	//SIP_HEADER_MIME_VERSION,
	//SIP_HEADER_MIN_EXPIRES,
	//SIP_HEADER_ORGANIZATION,
	//SIP_HEADER_PRIORITY,
	//SIP_HEADER_PROXY_AUTHENTICATE,
	//SIP_HEADER_PROXY_AUTHORIZATION,
	//SIP_HEADER_PROXY_REQUIRE,
	SIP_HEADER_RECORD_ROUTE,
	//SIP_HEADER_REPLY_TO,
	SIP_HEADER_REQUIRE,
	//SIP_HEADER_RETRY_AFTER,
	SIP_HEADER_ROUTE,
	//SIP_HEADER_SERVER,
	//SIP_HEADER_SUBJECT,
	SIP_HEADER_SUBSCRIPTION_STATE,
	//SIP_HEADER_SUPPORTED,
	SIP_HEADER_TIMESTAMP,
	SIP_HEADER_TO,
	SIP_HEADER_UNSUPPORTED,
	//SIP_HEADER_USER_AGENT,
	SIP_HEADER_VIA,
	//SIP_HEADER_WARNING,
	//SIP_HEADER_WWW_AUTHENTICATE,
	SIP_HEADER_TYPE_INVALID
};

enum SipBodyType
{
	SIP_BODY_APPLICATION_SDP,
	SIP_BODY_APPLICATION_PIDF_XML,
	SIP_BODY_TEXT_PLAIN,
	SIP_BODY_TYPE_INVALID
};

enum SipTransactionType
{
	SIP_TRANSACTION_CLIENT_INVITE,
	SIP_TRANSACTION_CLIENT_NON_INVITE,
	SIP_TRANSACTION_SERVER_INVITE,
	SIP_TRANSACTION_SERVER_NON_INVITE,
	SIP_TRANSACTION_TYPE_INVALID
};

enum SipEventCallback
{
	SIP_SESSION_INITIATED,
	SIP_SESSION_TERMINATED,
	SIP_SUBSCRIPTION_INITIATED,
	SIP_SUBSCRIPTION_TERMINATED,
	SIP_MESSAGE_RECEIVED,
	SIP_EVENT_CALLBACK_INVALID
};

enum SipTransportType
{
	SIP_TRANSPORT_UDP,
	SIP_TRANSPORT_TCP,
	SIP_TRANSPORT_TYPE_INVALID
};

enum SipEventPackage
{
	SIP_EVENT_PRESENCE,
	SIP_EVENT_PACKAGE_INVALID
};

enum SipSubscriptionState
{
	SIP_SUBSCRIPTION_STATE_ACTIVE,
	//SIP_SUBSCRIPTION_STATE_PENDING,
	SIP_SUBSCRIPTION_STATE_TERMINATED,
	SIP_SUBSCRIPTION_STATE_INVALID
};

enum SipPidfXmlBasicElement
{
	SIP_PIDF_XML_OPEN,
	SIP_PIDF_XML_CLOSED,
	SIP_PIDF_XML_BASIC_ELEMENT_INVALID
};

/*enum SipPriority
{
	SIP_PRIORITY_EMERGENCY,
	SIP_PRIORITY_URGENT,
	SIP_PRIORITY_NORMAL,
	SIP_PRIORITY_NONURGENT,
	SIP_PRIORITY_INVALID
};*/

enum SipTimer
{
	SIP_TIMER_A,
	SIP_TIMER_B,
	SIP_TIMER_C,
	SIP_TIMER_D,
	SIP_TIMER_E,
	SIP_TIMER_F,
	SIP_TIMER_G,
	SIP_TIMER_H,
	SIP_TIMER_I,
	SIP_TIMER_J,
	SIP_TIMER_K,
	SIP_TIMER_COUNT,

	SIP_TIMER_1 = 25000, //500000,  //500ms
	SIP_TIMER_2 = 200000, //4000000, //4s
	SIP_TIMER_4 = 250000, //5000000, //5s
	SIP_TIMER_32s = 1600000, //32000000, //32s
};


struct SipStatusCode
{
	int code;
	char reasonPhrase[MAX_REASON_PHRASE_LEN];
};


char *createString(const char *src);
bool startWith(const char *text1, const char *text2);
bool contains(const char *text1, const char *text2);
void removeLWS(char *text);
bool getLine(const char *text, char *result);
bool match(char *text, const char *match, char *result, bool ignoreSpecial = false);
void trim(char *text);
void skip(char *text, const char *skip);

__END_SYS

#endif
