#ifndef __sip_message_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_message_h

#include <sip_defs.h>
#include <sip_header.h>
#include <sip_body.h>
#include <utility/list.h>
#include <utility/malloc.h>

__BEGIN_SYS


class SipManager;

class SipMessage
{
private:
	friend class SipManager;

	Simple_List<SipHeader> headers;
	SipBody *body;
	bool canDelete;

	Simple_List<SipMessage>::Element link;

public:
	SipMessage() : body(0), canDelete(true), link(this) {};
	//SipMessage(SipMessage &msg);
	virtual ~SipMessage();

	static SipMessage *decodeMsg(const char *sipMsg);

	static SipMessageType getMsgType(const char *sipMsg);
	static const char *getMsgType(const SipMessageType type);
	virtual SipMessageType getMsgType() = 0;

	bool parse(const char *sipMsg);
	virtual bool parseStartLine(const char *sipMsg) = 0;
	bool parseHeader(const char *sipMsg);
	bool parseBody(const char *sipMsg);

	bool encode(char *sipMsg);
	virtual bool encodeStartLine(char *sipMsg) = 0;
	bool encodeHeader(char *sipMsg, char *bodyMsg);
	bool encodeBody(char *sipMsg);

	void addHeader(SipHeader *header);
	void addHeaders(Simple_List<SipHeader> &headers);
	SipHeader *getHeader(int headerType, int pos = 0);
	int getNumHeader(int headerType);

	void setBody(SipBody *body) { this->body = body; };
	SipBody *getBody() { return body; };

	static SipTransportType getTransportType(const char *type);
	static const char *getTransportType(const SipTransportType type);

	void setCanDelete(bool canDelete) { this->canDelete = canDelete; };
	bool getCanDelete() { return canDelete; };
};

//-------------------------------------------

class SipRequestLine
{
private:
	SipMessageType method;
	char *requestURI;
	char *sipVersion;

public:
	SipRequestLine() : method(SIP_MESSAGE_TYPE_INVALID), requestURI(0), sipVersion(0) {};
	//SipRequestLine(const SipRequestLine &request);
	//SipRequestLine(const SipMessageType msgType, const char *requestURI, const char *sipVersion = SIP_VERSION);
	~SipRequestLine();

	bool parse(const SipMessageType MsgType, char *sipMsg);
	bool encode(char *sipMsg);

	void setRequestLine(const SipMessageType msgType, const char *requestURI, const char *sipVersion);

	SipMessageType getMethod() const { return method; };
	const char *getRequestURI() const { return requestURI; };
	//const char *getSIPVersion() const { return sipVersion; };
};

//-------------------------------------------

class SipRequest : public SipMessage
{
private:
	SipRequestLine requestLine;

public:
	SipRequest() {};
	//SipRequest(SipRequest &request) : SipMessage(request), requestLine(request.requestLine) {};
	//SipRequest(SipMessageType msgType, char *requestURI, char *sipVersion = SIP_VERSION);
	virtual ~SipRequest() {};

	bool parseStartLine(const char *sipMsg);
	bool encodeStartLine(char *sipMsg) { return requestLine.encode(sipMsg); };

	void setRequestLine(const SipMessageType msgType, const char *requestURI, const char *sipVersion = SIP_VERSION)
		{ requestLine.setRequestLine(msgType, requestURI, sipVersion); };
	const SipRequestLine *getRequestLine() { return &requestLine; };
};

//-------------------------------------------

class SipRequestAck : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_ACK; };
};

//-------------------------------------------

class SipRequestBye : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_BYE; };
};

//-------------------------------------------

/*class SipRequestCancel : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_CANCEL; };
};*/

//-------------------------------------------

class SipRequestInvite : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_INVITE; };
};

//-------------------------------------------

class SipRequestMessage : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_MESSAGE; };
};

//-------------------------------------------

class SipRequestNotify : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_NOTIFY; };
};

//-------------------------------------------

/*class SipRequestOptions : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_OPTIONS; };
};*/

//-------------------------------------------

/*class SipRequestRegister : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_REGISTER; };
};*/

//-------------------------------------------

class SipRequestSubscribe : public SipRequest
{
public:
	SipMessageType getMsgType() { return SIP_REQUEST_SUBSCRIBE; };
};

//-------------------------------------------

class SipStatusLine
{
private:
	char *sipVersion;
	int statusCode;
	char *reasonPhrase;

public:
	SipStatusLine() : sipVersion(0), statusCode(0), reasonPhrase(0) {};
	//SipStatusLine(const SipStatusLine &status);
	//SipStatusLine(const char *sipVersion, int code, const char *reason);
	~SipStatusLine();

	bool parse(const char *version, char *sipMsg);
	bool encode(char *sipMsg);

	void setStatusLine(const char *sipVersion, int statusCode, const char *reasonPhrase);

	const char *getSIPVersion() const { return sipVersion; };
	int getStatusCode() const { return statusCode; };
	const char *getReasonPhrase() const { return reasonPhrase; };
};

//-------------------------------------------

class SipResponse : public SipMessage
{
private:
	SipStatusLine statusLine;

	static SipStatusCode statusCodes[];

public:
	SipResponse() {};
	//SipResponse(SipResponse &response) : SipMessage(response), statusLine(response.statusLine) {};
	SipResponse(int statusCode);
	~SipResponse() {};

	bool parseStartLine(const char *sipMsg);
	bool encodeStartLine(char *sipMsg) { return statusLine.encode(sipMsg); };

	SipMessageType getMsgType() { return SIP_RESPONSE; };

	const SipStatusLine *getStatusLine() { return &statusLine; };
};

__END_SYS

#endif
