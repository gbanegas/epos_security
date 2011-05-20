#include <sip_message.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <sip_header.h>

__BEGIN_SYS


/*SipMessage::SipMessage(SipMessage &msg) : link(this)
{
	canDelete = true;

	Simple_List<SipHeader>::Iterator it = msg.headers.begin();
	while (it != msg.headers.end())
	{
		SipHeader *header = it->object();
		it++;

		SipHeader *header2 = SipHeader::createHeader(header->getHeaderType(), header);
		addHeader(header2);
	}

	if (msg.body)
		body = SipBody::createBody(msg.body->getBodyType(), msg.body);
	else
		body = 0;
}*/

SipMessage::~SipMessage()
{
	Simple_List<SipHeader>::Iterator it = headers.begin();
	while (it != headers.end())
	{
		SipHeader *header = it->object();
		headers.remove(it++);
		delete header;
	}

	if (body)
		delete body;
}

SipMessage *SipMessage::decodeMsg(const char *sipMsg)
{
	char *pSipMsg = (char *) sipMsg;
	removeLWS(pSipMsg);

	char type[30];
	match(pSipMsg, " ", type);
	trim(type);
	SipMessageType msgType = getMsgType(type);

	SipMessage *msg = 0;

	switch (msgType)
	{
		case SIP_REQUEST_ACK:			msg = new SipRequestAck();			break;
		case SIP_REQUEST_BYE:			msg = new SipRequestBye();			break;
		//case SIP_REQUEST_CANCEL:		msg = new SipRequestCancel();		break;
		case SIP_REQUEST_INVITE:		msg = new SipRequestInvite();		break;
		case SIP_REQUEST_MESSAGE:		msg = new SipRequestMessage();		break;
		case SIP_REQUEST_NOTIFY:		msg = new SipRequestNotify();		break;
		//case SIP_REQUEST_OPTIONS:		msg = new SipRequestOptions();		break;
		//case SIP_REQUEST_REGISTER:	msg = new SipRequestRegister();		break;
		case SIP_REQUEST_SUBSCRIBE:		msg = new SipRequestSubscribe();	break;
		case SIP_RESPONSE:				msg = new SipResponse();			break;
		default: db<SipMessage>(WRN) << "SipMessage::decodeMsgType -> Invalid " << type << " message type\n"; break;
	}

	if ((msg) && (!msg->parse(pSipMsg)))
	{
		db<SipMessage>(WRN) << "SipMessage::decodeMsgType -> Failed to parse " << type << " message type\n";
		delete msg;
		msg = 0;
	}

	return msg;
}

SipMessageType SipMessage::getMsgType(const char *sipMsg)
{
	SipMessageType type = SIP_MESSAGE_TYPE_INVALID;

	if (!strcmp(sipMsg, "ACK"))
		type = SIP_REQUEST_ACK;

	else if (!strcmp(sipMsg, "BYE"))
		type = SIP_REQUEST_BYE;

	/*else if (!strcmp(sipMsg, "CANCEL"))
		type = SIP_REQUEST_CANCEL;*/

	else if (!strcmp(sipMsg, "INVITE"))
		type = SIP_REQUEST_INVITE;

	else if (!strcmp(sipMsg, "MESSAGE"))
			type = SIP_REQUEST_MESSAGE;

	else if (!strcmp(sipMsg, "NOTIFY"))
		type = SIP_REQUEST_NOTIFY;

	/*else if (!strcmp(sipMsg, "OPTIONS"))
		type = SIP_REQUEST_OPTIONS;

	else if (!strcmp(sipMsg, "REGISTER"))
		type = SIP_REQUEST_REGISTER;*/

	else if (!strcmp(sipMsg, "SUBSCRIBE"))
			type = SIP_REQUEST_SUBSCRIBE;

	else if (!strcmp(sipMsg, SIP_VERSION))
		type = SIP_RESPONSE;

	return type;
}

const char *SipMessage::getMsgType(const SipMessageType type)
{
	const char *sipMsg = 0;

	switch (type)
	{
		case SIP_REQUEST_ACK:			sipMsg = "ACK";			break;
		case SIP_REQUEST_BYE:			sipMsg = "BYE";			break;
		//case SIP_REQUEST_CANCEL:		sipMsg = "CANCEL";		break;
		case SIP_REQUEST_INVITE:		sipMsg = "INVITE";		break;
		case SIP_REQUEST_MESSAGE:		sipMsg = "MESSAGE";		break;
		case SIP_REQUEST_NOTIFY:		sipMsg = "NOTIFY";		break;
		//case SIP_REQUEST_OPTIONS:		sipMsg = "OPTIONS";		break;
		//case SIP_REQUEST_REGISTER:	sipMsg = "REGISTER";	break;
		case SIP_REQUEST_SUBSCRIBE:		sipMsg = "SUBSCRIBE";	break;
		case SIP_RESPONSE:				sipMsg = SIP_VERSION;	break;
		default:						break;
	}

	return sipMsg;
}

bool SipMessage::parse(const char *sipMsg)
{
	if (!parseStartLine(sipMsg))
		return false;

	if (!parseHeader(sipMsg))
		return false;

	if (!parseBody(sipMsg))
		return false;

	return true;
}

bool SipMessage::parseHeader(const char *sipMsg)
{
	char line[MAX_LINE];
	Simple_List<SipHeader> header;
	while (true)
	{
		bool ret = getLine(sipMsg, line);
		if ((!ret) || (!strcmp(line, "")))
			return true;

		SipHeader::decodeHeader(line, header);
		addHeaders(header);
	}

	return true;
}

bool SipMessage::parseBody(const char *sipMsg)
{
	SipHeaderContentType *contentType = (SipHeaderContentType *) getHeader(SIP_HEADER_CONTENT_TYPE);
	if (contentType)
		body = SipBody::decodeBody(sipMsg, contentType->getContentType());
	return true;
}

bool SipMessage::encode(char *sipMsg)
{
	char bodyMsg[512];
	bodyMsg[0] = 0;

	if (!encodeStartLine(sipMsg))
		return false;

	if (!encodeBody(bodyMsg))
		return false;

	if (!encodeHeader(sipMsg, bodyMsg))
		return false;

	return true;
}

bool SipMessage::encodeHeader(char *sipMsg, char *bodyMsg)
{
	char *pSipMsg;

	if (body)
	{
		SipHeaderContentType *headerContentType = (SipHeaderContentType *) getHeader(SIP_HEADER_CONTENT_TYPE);
		if (!headerContentType)
		{
			headerContentType = new SipHeaderContentType();
			addHeader(headerContentType);
		}
		headerContentType->setContentType(body->getBodyType());
	}

	SipHeaderContentLength *headerContentLength = (SipHeaderContentLength *) getHeader(SIP_HEADER_CONTENT_LENGTH);
	if (!headerContentLength)
	{
		headerContentLength = new SipHeaderContentLength();
		addHeader(headerContentLength);
	}
	headerContentLength->setNumber(strlen(bodyMsg));


	Simple_List<SipHeader>::Iterator it = headers.begin();
	while (it != headers.end())
	{
		SipHeader *header = it->object();
		it++;

		pSipMsg = sipMsg + strlen(sipMsg);

		if (header)
			header->encode(pSipMsg);
	}

	strcat(sipMsg, "\r\n");
	strcat(sipMsg, bodyMsg);
	return true;
}

bool SipMessage::encodeBody(char *sipMsg)
{
	if (body)
		body->encode(sipMsg);
    return true;
}

void SipMessage::addHeader(SipHeader *header)
{
	if (header)
		headers.insert(&header->link);
}

void SipMessage::addHeaders(Simple_List<SipHeader> &headers)
{
	Simple_List<SipHeader>::Iterator it = headers.begin();
	while (it != headers.end())
	{
		SipHeader *header = it->object();
		it++;
		headers.remove_head();
		this->headers.insert(&header->link);
	}
}

SipHeader *SipMessage::getHeader(int headerType, int pos)
{
	int count = -1;
	Simple_List<SipHeader>::Iterator it = headers.begin();
	while (it != headers.end())
	{
		SipHeader *header = it->object();
		it++;

		if (header->getHeaderType() == headerType)
		{
			count++;
			if (count == pos)
				return header;
		}
	}
	return 0;
}

int SipMessage::getNumHeader(int headerType)
{
	int count = 0;
	Simple_List<SipHeader>::Iterator it = headers.begin();
	while (it != headers.end())
	{
		SipHeader *header = it->object();
		it++;

		if (header->getHeaderType() == headerType)
			count++;
	}
	return count;
}

SipTransportType SipMessage::getTransportType(const char *type)
{
	SipTransportType transport = SIP_TRANSPORT_TYPE_INVALID;

	if (!strcmp(type, "UDP"))
		transport = SIP_TRANSPORT_UDP;

	else if (!strcmp(type, "TCP"))
		transport = SIP_TRANSPORT_TCP;

	return transport;
}

const char *SipMessage::getTransportType(const SipTransportType type)
{
	const char *transport = 0;

	switch (type)
	{
		case SIP_TRANSPORT_UDP:	transport = "UDP";	break;
		case SIP_TRANSPORT_TCP:	transport = "TCP";	break;
		default:			break;
	}

	return transport;
}

//-------------------------------------------

/*SipRequestLine::SipRequestLine(const SipRequestLine &request)
{
	method = request.method;
	requestURI = createString(request.requestURI);
	sipVersion = createString(request.sipVersion);
}*/

/*SipRequestLine::SipRequestLine(const SipMessageType msgType, const char *requestURI, const char *sipVersion)
{
	this->method = msgType;
	this->requestURI = createString(requestURI);
	this->sipVersion = createString(sipVersion);
}*/

SipRequestLine::~SipRequestLine()
{
	if (requestURI)
		delete requestURI;

	if (sipVersion)
		delete sipVersion;
}

bool SipRequestLine::parse(const SipMessageType MsgType, char *sipMsg)
{
	method = MsgType;

	char result[255];
	skip(sipMsg, " \t");
	match(sipMsg, " ", result);
	requestURI = createString(result);
	if (!requestURI)
		return false;

	trim(sipMsg);
	sipVersion = createString(sipMsg);
	if (!sipVersion)
		return false;

	return true;
}

bool SipRequestLine::encode(char *sipMsg)
{
	strcat(sipMsg, SipMessage::getMsgType(method));
	strcat(sipMsg, " ");
	strcat(sipMsg, requestURI);
	strcat(sipMsg, " ");
	strcat(sipMsg, sipVersion);
	strcat(sipMsg, "\r\n");
	return true;
}

void SipRequestLine::setRequestLine(const SipMessageType msgType, const char *requestURI, const char *sipVersion)
{
	this->method = msgType;

	if (this->requestURI)
		delete this->requestURI;
	this->requestURI = createString(requestURI);

	if (this->sipVersion)
		delete this->sipVersion;
	this->sipVersion = createString(sipVersion);
}

//-------------------------------------------

/*SipRequest::SipRequest(SipMessageType msgType, char *requestURI, char *sipVersion = SIP_VERSION)
{
	setRequestLine(msgType, requestURI, sipVersion);
}*/

bool SipRequest::parseStartLine(const char *sipMsg)
{
	char line[MAX_LINE];
	getLine(sipMsg, line);

	if (!requestLine.parse(getMsgType(), line))
	{
		db<SipRequest>(WRN) << "SipRequest::parseStartLine -> parse failed\n";
		return false;
	}

	return true;
}

//-------------------------------------------

SipStatusCode SipResponse::statusCodes[] =
{
	//Informational
	{ 100, "Trying" },
	{ 180, "Ringing" },
	{ 181, "Call is Being Forwarded" },
	{ 182, "Queued" },
	{ 183, "Session Progress" },

	//Success
	{ 200, "OK" },
	{ 202, "Accepted" },

	//Redirection
	{ 300, "Multiple Choices" },
	{ 301, "Moved Permanently" },
	{ 302, "Moved Temporarily" },
	//{ 303, "See Other" },
	{ 305, "Use Proxy" },
	{ 380, "Alternative Service" },

	//Client-Error
	{ 400, "Bad Request" },
	{ 401, "Unauthorized" },
	{ 402, "Payment Required" },
	{ 403, "Forbidden" },
	{ 404, "Not Found" },
	{ 405, "Method Not Allowed" },
	{ 406, "Not Acceptable" },
	{ 407, "Proxy Authentication Required" },
	{ 408, "Request Timeout" },
	{ 409, "Conflict" },
	{ 410, "Gone" },
	//{ 411, "Length Required" },
	{ 413, "Request Entity Too Large" },
	{ 414, "Request-URI Too Large" },
	{ 415, "Unsupported Media Type" },
	{ 420, "Bad Extension" },
	{ 480, "Temporarily Unavailable" },
	{ 481, "Call Leg/Transaction Does Not Exist" },
	{ 482, "Loop Detected" },
	{ 483, "Too Many Hops" },
	{ 484, "Address Incomplete" },
	{ 485, "Ambiguous" },
	{ 486, "Busy Here" },
	{ 487, "Request Terminated" },
	{ 488, "Not Acceptable Here" },
	{ 489, "Bad Event" },

	//Server-Error
	{ 500, "Server Internal Error" },
	{ 501, "Not Implemented" },
	{ 502, "Bad Gateway" },
	{ 503, "Service Unavailable" },
	{ 504, "Server Timed-out" },
	{ 505, "Version Not Supported" },
	{ 513, "Message Too Large" },

	//Global-Failure
	{ 600, "Busy Everywhere"},
	{ 603, "Decline"},
	{ 604, "Does Not Exist Anywhere"},
	{ 606, "Not Acceptable"},
	{ 0, "Unknown"},
};

/*SipStatusLine::SipStatusLine(const SipStatusLine &status)
{
	sipVersion = createString(status.sipVersion);
	statusCode = status.statusCode;
	reasonPhrase = createString(status.reasonPhrase);
}*/

/*SipStatusLine::SipStatusLine(const char *sipVersion, int code, const char *reason)
{
	this->sipVersion = createString(sipVersion);
	this->statusCode = code;
	this->reasonPhrase = createString(reason);
}*/

SipStatusLine::~SipStatusLine()
{
	if (sipVersion)
		delete sipVersion;

	if (reasonPhrase)
		delete reasonPhrase;
}

bool SipStatusLine::parse(const char *Version, char *sipMsg)
{
	sipVersion = createString(Version);
	if (!sipVersion)
		return false;

	char result[255];
	skip(sipMsg, " \t");
	match(sipMsg, " ", result);
	int size = strlen(result);
	if (size == 0)
		return false;
	statusCode = (int) atol(result);

	trim(sipMsg);
	reasonPhrase = createString(sipMsg);
	if (!reasonPhrase)
		return false;

	return true;
}

bool SipStatusLine::encode(char *sipMsg)
{
	strcat(sipMsg, sipVersion);
	strcat(sipMsg, " ");

	char value[11];
	itoa(statusCode, value);
	strcat(sipMsg, value);
	strcat(sipMsg, " ");

	strcat(sipMsg, reasonPhrase);
	strcat(sipMsg, "\r\n");
	return true;
}

void SipStatusLine::setStatusLine(const char *sipVersion, int statusCode, const char *reasonPhrase)
{
	if (this->sipVersion)
		delete this->sipVersion;
	this->sipVersion = createString(sipVersion);

	this->statusCode = statusCode;

	if (this->reasonPhrase)
		delete this->reasonPhrase;
	this->reasonPhrase = createString(reasonPhrase);
}

//-------------------------------------------

SipResponse::SipResponse(int statusCode)
{
	int i = 0;
	while (statusCodes[i].code != 0)
	{
		if (statusCodes[i].code == statusCode)
		{
			statusLine.setStatusLine(SIP_VERSION, statusCode, statusCodes[i].reasonPhrase);
			break;
		}
		i++;
	}
}

bool SipResponse::parseStartLine(const char *sipMsg)
{
	char line[MAX_LINE];
	getLine(sipMsg, line);

	if (!statusLine.parse(SIP_VERSION, line))
	{
		db<SipResponse>(WRN) << "SipResponse::parseStartLine -> parse failed\n";
		return false;
	}

	return true;
}

__END_SYS
