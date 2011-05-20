#ifndef __sip_header_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_header_h

#include <sip_defs.h>
#include <utility/list.h>

__BEGIN_SYS


class SipHeader
{
private:
	friend class SipMessage;
	friend class SipDialog;

	Simple_List<SipHeader>::Element link;

public:
	SipHeader() : link(this) {};
	virtual ~SipHeader() {};

	static void decodeHeader(char *line, Simple_List<SipHeader> &ret);
	static SipHeader *createHeader(SipHeaderType type/*, const SipHeader *copy = 0*/);

	virtual SipHeaderType getHeaderType() = 0;

	virtual bool parse(char *sipMsg) = 0;
	virtual bool encode(char *sipMsg) = 0;
};

//-------------------------------------------

class ValueString : public SipHeader
{
protected:
	char *string;

public:
	ValueString() : string(0) {};
	ValueString(ValueString &value) { string = createString(value.string); };
	~ValueString() { if (string) delete string; };

	virtual bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	const char *getString() { return string; };
	void setString(const char *string);
};

//-------------------------------------------

class ValueNumber : public SipHeader
{
protected:
	int number;

public:
	ValueNumber() : number(0) {};
	ValueNumber(ValueNumber &value) : number(value.number) {};
	~ValueNumber() {};

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	int getNumber() { return number; };
	void setNumber(int number) { this->number = number; };
};

//-------------------------------------------

class ValueAddressParams : public SipHeader
{
private:
	char *display;
	char *address;
	char *tag;
	bool lr;

public:
	ValueAddressParams() : display(0), address(0), tag(0), lr(false) {};
	ValueAddressParams(ValueAddressParams &value);
	~ValueAddressParams();

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	const char *getAddress() { return address; };
	const char *getTag() { return tag; };
	bool isLR() { return lr; };

	void setAddress(const char *address, bool lr = false);
	void setTag(const char *tag);
};

//-------------------------------------------

class ValueOptions : public SipHeader
{
	Simple_List<char> options;

public:
	ValueOptions() {};
	ValueOptions(ValueOptions &value);
	~ValueOptions();

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	int getOptionSize() { return (int) options.size(); };
	const char *getOption(int pos);
	void addOption(const char *option);
};

//-------------------------------------------

class SipHeaderAllow : public ValueOptions
{
public:
	//TODO: Allow pode ser vazio!
	SipHeaderType getHeaderType() { return SIP_HEADER_ALLOW; };

	void addAllowed(const SipMessageType allowed);
};

//-------------------------------------------

class SipHeaderAllowEvents : public ValueOptions
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_ALLOW_EVENTS; };
};

//-------------------------------------------

class SipHeaderCallID : public ValueString
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_CALLID; };
};

//-------------------------------------------

class SipHeaderContact : public ValueAddressParams
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_CONTACT; };
};

//-------------------------------------------

class SipHeaderContentDisposition : public ValueString
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_CONTENT_DISPOSITION; };

	bool parse(char *sipMsg);
};

//-------------------------------------------

class SipHeaderContentLength : public ValueNumber
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_CONTENT_LENGTH; };
};

//-------------------------------------------

class SipHeaderContentType : public SipHeader
{
private:
	SipBodyType type;

public:
	SipHeaderContentType() : type(SIP_BODY_TYPE_INVALID) {};
	SipHeaderContentType(SipHeaderContentType &header) { type = header.type; };
	~SipHeaderContentType() {};

	SipHeaderType getHeaderType() { return SIP_HEADER_CONTENT_TYPE; };

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	void setContentType(SipBodyType type) { this->type = type; };
	SipBodyType getContentType() { return type; };
};

//-------------------------------------------

class SipHeaderCSeq : public SipHeader
{
private:
	unsigned int sequence;
	SipMessageType method;

public:
	SipHeaderCSeq() : sequence(0), method(SIP_MESSAGE_TYPE_INVALID) {};
	SipHeaderCSeq(SipHeaderCSeq &header) { sequence = header.sequence; method = header.method; };
	~SipHeaderCSeq() {};

	SipHeaderType getHeaderType() { return SIP_HEADER_CSEQ; };

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	unsigned int getSequence() { return sequence; };
	SipMessageType getMethod() { return method; };
	void setCSeq(SipMessageType method, int sequence) { this->method = method; this->sequence = sequence; };
};

//-------------------------------------------

class SipHeaderEvent : public SipHeader
{
	SipEventPackage type;
	char *id;

public:
	SipHeaderEvent() : type(SIP_EVENT_PACKAGE_INVALID), id(0) {};
	SipHeaderEvent(SipHeaderEvent &header);
	~SipHeaderEvent();

	SipHeaderType getHeaderType() { return SIP_HEADER_EVENT; };

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	SipEventPackage getType() { return type; };
	const char *getId() { return id; };
	void setEvent(SipEventPackage type, const char *id);
};

//-------------------------------------------

class SipHeaderExpires : public ValueNumber
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_EXPIRES; };
};

//-------------------------------------------

class SipHeaderFrom : public ValueAddressParams
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_FROM; };
};

//-------------------------------------------

class SipHeaderMaxForwards : public ValueNumber
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_MAX_FORWARDS; };
};

//-------------------------------------------

class SipHeaderRecordRoute : public ValueAddressParams
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_RECORD_ROUTE; };
};

//-------------------------------------------

class SipHeaderRequire : public ValueOptions
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_REQUIRE; };
};

//-------------------------------------------

class SipHeaderRoute : public ValueAddressParams
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_ROUTE; };
};

//-------------------------------------------

class SipHeaderSubscriptionState : public SipHeader
{
	SipSubscriptionState state;
	int expires;

public:
	SipHeaderSubscriptionState() : state(SIP_SUBSCRIPTION_STATE_INVALID), expires(-1) {};
	SipHeaderSubscriptionState(SipHeaderSubscriptionState &header);
	~SipHeaderSubscriptionState() {};

	SipHeaderType getHeaderType() { return SIP_HEADER_SUBSCRIPTION_STATE; };

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	//SipSubscriptionState getState() { return state; };
	//int getExpires() { return expires; };
	void setSubscriptionState(SipSubscriptionState state, int expires = -1);
};

//-------------------------------------------

class SipHeaderTo : public ValueAddressParams
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_TO; };
};

//-------------------------------------------

class SipHeaderUnsupported : public ValueOptions
{
public:
	SipHeaderType getHeaderType() { return SIP_HEADER_UNSUPPORTED; };
};

//-------------------------------------------

class SipHeaderVia : public SipHeader
{
	char *protocolName;
	char *protocolVersion;
	SipTransportType transport;
	char *sentBy;
	int port;
	char *branch;
	char *received;

public:
	SipHeaderVia() : protocolName(0), protocolVersion(0), transport(SIP_TRANSPORT_TYPE_INVALID), sentBy(0), port(0), branch(0), received(0) {};
	SipHeaderVia(SipHeaderVia &header);
	~SipHeaderVia();

	SipHeaderType getHeaderType() { return SIP_HEADER_VIA; };

	bool parse(char *sipMsg);
	bool encode(char *sipMsg);

	//SipTransportType getTransport() { return transport; };
	const char *getSentBy() { return sentBy; };
	int getPort() { return port; };
	const char *getBranch() { return branch; };
	const char *getReceived() { return received; };
	void setVia(const char *protocolName, const char *protocolVersion, const SipTransportType transport,
			const char *sentBy, int port, const char *branch);
	void setReceived(const char *received);
};

__END_SYS

#endif
