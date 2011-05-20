#include <sip_header.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <sip_message.h>

__BEGIN_SYS


void SipHeader::decodeHeader(char *line, Simple_List<SipHeader> &ret)
{
	SipHeaderType headerType = SIP_HEADER_TYPE_INVALID;

	char type[255];
	bool matched = match(line, ":", type);
	if (!matched)
		return;

	if (!strcmp(type, "Allow"))
		headerType = SIP_HEADER_ALLOW;

	else if (!strcmp(type, "Allow-Events") || !strcmp(type, "u"))
		headerType = SIP_HEADER_ALLOW_EVENTS;

	else if (!strcmp(type, "Call-ID") || !strcmp(type, "i"))
		headerType = SIP_HEADER_CALLID;

	else if (!strcmp(type, "Contact") || !strcmp(type, "m"))
		headerType = SIP_HEADER_CONTACT;

	else if (!strcmp(type, "Content-Disposition"))
		headerType = SIP_HEADER_CONTENT_DISPOSITION;

	else if (!strcmp(type, "Content-Length") || !strcmp(type, "l"))
		headerType = SIP_HEADER_CONTENT_LENGTH;

	else if (!strcmp(type, "Content-Type") || !strcmp(type, "c"))
		headerType = SIP_HEADER_CONTENT_TYPE;

	else if (!strcmp(type, "CSeq"))
		headerType = SIP_HEADER_CSEQ;

	else if (!strcmp(type, "Event") || !strcmp(type, "o"))
		headerType = SIP_HEADER_EVENT;

	else if (!strcmp(type, "Expires"))
		headerType = SIP_HEADER_EXPIRES;

	else if (!strcmp(type, "From") || !strcmp(type, "f"))
		headerType = SIP_HEADER_FROM;

	else if (!strcmp(type, "Max-Forwards"))
		headerType = SIP_HEADER_MAX_FORWARDS;

	else if (!strcmp(type, "Record-Route"))
		headerType = SIP_HEADER_RECORD_ROUTE;

	else if (!strcmp(type, "Require"))
		headerType = SIP_HEADER_REQUIRE;

	else if (!strcmp(type, "Route"))
		headerType = SIP_HEADER_ROUTE;

	else if (!strcmp(type, "Subscription-State"))
		headerType = SIP_HEADER_SUBSCRIPTION_STATE;

	else if (!strcmp(type, "To") || !strcmp(type, "t"))
		headerType = SIP_HEADER_TO;

	else if (!strcmp(type, "Unsupported"))
		headerType = SIP_HEADER_UNSUPPORTED;

	else if (!strcmp(type, "Via") || !strcmp(type, "v"))
		headerType = SIP_HEADER_VIA;

	else
	{
		db<SipHeader>(WRN) << "SipHeader::decodeHeader -> Invalid header type (" << type << ")\n";
		return;
	}

	char result[MAX_LINE] = {0};
	matched = false;
	do
	{
		if ((headerType == SIP_HEADER_CONTACT) || (headerType == SIP_HEADER_RECORD_ROUTE) || (headerType == SIP_HEADER_ROUTE) || (headerType == SIP_HEADER_VIA))
			matched = match(line, ",", result);
		else
			strcpy(result, line);

		trim(result);

		SipHeader *header = createHeader(headerType);
		ret.insert(&header->link);

		if (!header->parse(result))
		{
			db<SipHeader>(WRN) << "SipHeader::decodeHeader -> Failed to parse " << type << " header type\n";

			Simple_List<SipHeader>::Iterator it = ret.begin();
			while (it != ret.end())
			{
				SipHeader *header = it->object();
				ret.remove(it++);
				delete header;
			}
			break;
		}
	}while (matched);

	return;
}

SipHeader *SipHeader::createHeader(SipHeaderType type/*, const SipHeader *copy*/)
{
	SipHeader *header = 0;

	switch (type)
	{
		case SIP_HEADER_ALLOW:					header = /*(!copy) ?*/ new SipHeaderAllow()					/*: new SipHeaderAllow(*((SipHeaderAllow *) copy))*/;								break;
		case SIP_HEADER_ALLOW_EVENTS:			header = /*(!copy) ?*/ new SipHeaderAllowEvents()			/*: new SipHeaderAllowEvents(*((SipHeaderAllowEvents *) copy))*/;					break;
		case SIP_HEADER_CALLID:					header = /*(!copy) ?*/ new SipHeaderCallID()				/*: new SipHeaderCallID(*((SipHeaderCallID *) copy))*/;								break;
		case SIP_HEADER_CONTACT:				header = /*(!copy) ?*/ new SipHeaderContact()				/*: new SipHeaderContact(*((SipHeaderContact *) copy))*/;							break;
		case SIP_HEADER_CONTENT_DISPOSITION:	header = /*(!copy) ?*/ new SipHeaderContentDisposition()	/*: new SipHeaderContentDisposition(*((SipHeaderContentDisposition *) copy))*/;		break;
		case SIP_HEADER_CONTENT_LENGTH:			header = /*(!copy) ?*/ new SipHeaderContentLength()			/*: new SipHeaderContentLength(*((SipHeaderContentLength *) copy))*/;				break;
		case SIP_HEADER_CONTENT_TYPE:			header = /*(!copy) ?*/ new SipHeaderContentType()			/*: new SipHeaderContentType(*((SipHeaderContentType *) copy))*/;					break;
		case SIP_HEADER_CSEQ:					header = /*(!copy) ?*/ new SipHeaderCSeq()					/*: new SipHeaderCSeq(*((SipHeaderCSeq *) copy))*/;									break;
		case SIP_HEADER_EVENT:					header = /*(!copy) ?*/ new SipHeaderEvent()					/*: new SipHeaderEvent(*((SipHeaderEvent *) copy))*/;								break;
		case SIP_HEADER_EXPIRES:				header = /*(!copy) ?*/ new SipHeaderExpires()				/*: new SipHeaderExpires(*((SipHeaderExpires *) copy))*/;							break;
		case SIP_HEADER_FROM:					header = /*(!copy) ?*/ new SipHeaderFrom()					/*: new SipHeaderFrom(*((SipHeaderFrom *) copy))*/;									break;
		case SIP_HEADER_MAX_FORWARDS:			header = /*(!copy) ?*/ new SipHeaderMaxForwards()			/*: new SipHeaderMaxForwards(*((SipHeaderMaxForwards *) copy))*/;					break;
		case SIP_HEADER_RECORD_ROUTE:			header = /*(!copy) ?*/ new SipHeaderRecordRoute()			/*: new SipHeaderRecordRoute(*((SipHeaderRecordRoute *) copy))*/;					break;
		case SIP_HEADER_REQUIRE:				header = /*(!copy) ?*/ new SipHeaderRequire()				/*: new SipHeaderRequire(*((SipHeaderRequire *) copy))*/;							break;
		case SIP_HEADER_ROUTE:					header = /*(!copy) ?*/ new SipHeaderRoute()					/*: new SipHeaderRoute(*((SipHeaderRoute *) copy))*/;								break;
		case SIP_HEADER_SUBSCRIPTION_STATE:		header = /*(!copy) ?*/ new SipHeaderSubscriptionState()		/*: new SipHeaderSubscriptionState(*((SipHeaderSubscriptionState *) copy))*/;		break;
		case SIP_HEADER_TO:						header = /*(!copy) ?*/ new SipHeaderTo()					/*: new SipHeaderTo(*((SipHeaderTo *) copy))*/;										break;
		case SIP_HEADER_UNSUPPORTED:			header = /*(!copy) ?*/ new SipHeaderUnsupported()			/*: new SipHeaderUnsupported(*((SipHeaderUnsupported *) copy))*/;					break;
		case SIP_HEADER_VIA:					header = /*(!copy) ?*/ new SipHeaderVia()					/*: new SipHeaderVia(*((SipHeaderVia *) copy))*/;									break;
		default:								break;
	}

	return header;
}

//-------------------------------------------

bool ValueString::parse(char *sipMsg)
{
	trim(sipMsg);
	string = createString(sipMsg);
	if (!string)
		return false;

	return true;
}

bool ValueString::encode(char *sipMsg)
{
	switch (getHeaderType())
	{
		case SIP_HEADER_CALLID:					strcat(sipMsg, "Call-ID: ");				break;
		case SIP_HEADER_CONTENT_DISPOSITION:	strcat(sipMsg, "Content-Disposition: ");	break;
		default:								return false;
	}

	strcat(sipMsg, string);
	strcat(sipMsg, "\r\n");
	return true;
}

void ValueString::setString(const char *string)
{
	if (this->string)
		delete this->string;
	this->string = createString(string);
}

//-------------------------------------------

bool ValueNumber::parse(char *sipMsg)
{
	trim(sipMsg);
	number = (int) atol(sipMsg);
	return true;
}

bool ValueNumber::encode(char *sipMsg)
{
	switch (getHeaderType())
	{
		case SIP_HEADER_CONTENT_LENGTH:	strcat(sipMsg, "Content-Length: ");	break;
		case SIP_HEADER_EXPIRES:		strcat(sipMsg, "Expires: ");		break;
		case SIP_HEADER_MAX_FORWARDS:	strcat(sipMsg, "Max-Forwards: ");	break;
		default:						return false;
	}

	char value[11];
	itoa(number, value);

	strcat(sipMsg, value);
	strcat(sipMsg, "\r\n");
	return true;
}

//-------------------------------------------

ValueAddressParams::ValueAddressParams(ValueAddressParams &value)
{
	display = createString(value.display);
	address = createString(value.address);
	tag = createString(value.tag);
	lr = value.lr;
}

ValueAddressParams::~ValueAddressParams()
{
	if (display)
		delete display;
	if (address)
		delete address;
	if (tag)
		delete tag;
}

bool ValueAddressParams::parse(char *sipMsg)
{
	char result[512];
	bool matched;
	int i = 0;

	do
	{
		matched = match(sipMsg, ";", result);
		trim(result);

		if (i == 0)
		{
			char aux[255];
			bool matched1 = match(result, "<", aux, true);
			if (matched1)
			{
				trim(aux);
				display = createString(aux);
			}

			bool matched2 = match(result, ";", aux, true);
			if (matched2)
			{
				trim(aux);
				address = createString(aux);

				if (contains(result, "lr"))
					lr = true;

			}else if (matched1)
			{
				matched1 = match(result, ">", aux, true);
				if (!matched1)
					return false;

				trim(aux);
				address = createString(aux);
			}else
				address = createString(result);

			if (!address)
				return false;
		}else
		{
			if (startWith(result, "tag="))
			{
				tag = createString(&result[4]);
				if (!tag)
					return false;
			}
		}
		i++;
	}while (matched);

	return true;
}

bool ValueAddressParams::encode(char *sipMsg)
{
	switch (getHeaderType())
	{
		case SIP_HEADER_CONTACT:
			strcat(sipMsg, "Contact: ");
			if (!strcmp(address, "*"))
			{
				strcat(sipMsg, "*\r\n");
				return true;
			}
			break;
		case SIP_HEADER_FROM:			strcat(sipMsg, "From: ");			break;
		case SIP_HEADER_RECORD_ROUTE:	strcat(sipMsg, "Record-Route: ");	break;
		case SIP_HEADER_ROUTE:			strcat(sipMsg, "Route: ");			break;
		case SIP_HEADER_TO:				strcat(sipMsg, "To: ");				break;
		default:						return false;
	}

	if (display)
	{
		strcat(sipMsg, display);
		strcat(sipMsg, " ");
	}

	strcat(sipMsg, "<");
	strcat(sipMsg, address);

	if (lr)
		strcat(sipMsg, ";lr");

	strcat(sipMsg, ">");

	if (tag)
	{
		strcat(sipMsg, ";tag=");
		strcat(sipMsg, tag);
	}

	strcat(sipMsg, "\r\n");
	return true;
}

void ValueAddressParams::setAddress(const char *address, bool lr)
{
	if (this->address)
		delete this->address;
	this->address = createString(address);
	this->lr = lr;
}

void ValueAddressParams::setTag(const char *tag)
{
	if (this->tag)
		delete this->tag;
	this->tag = createString(tag);
}

//-------------------------------------------

ValueOptions::ValueOptions(ValueOptions &value)
{
	Simple_List<char>::Iterator it = value.options.begin();
	while (it != value.options.end())
	{
		char *option = it->object();
		it++;

		if (!option)
			continue;

		char *option2 = createString(option);
		if (!option2)
			continue;

		Simple_List<char>::Element *eOption2 = new Simple_List<char>::Element(option2);
		options.insert(eOption2);
	}
}

ValueOptions::~ValueOptions()
{
	Simple_List<char>::Iterator it = options.begin();
	while (it != options.end())
	{
		Simple_List<char>::Element *el = it++;
		options.remove(el);
		delete el->object();
		delete el;
	}
}

bool ValueOptions::parse(char *sipMsg)
{
	char result[512];
	bool matched;

	do
	{
		matched = match(sipMsg, ",", result);
		trim(result);
		char *option = createString(result);
		if (!option)
			return false;

		Simple_List<char>::Element *eOption = new Simple_List<char>::Element(option);
		options.insert(eOption);
	}while (matched);

	return true;
}

bool ValueOptions::encode(char *sipMsg)
{
	switch (getHeaderType())
	{
		case SIP_HEADER_ALLOW:			strcat(sipMsg, "Allow: ");			break;
		case SIP_HEADER_ALLOW_EVENTS:	strcat(sipMsg, "Allow-Events: ");	break;
		case SIP_HEADER_REQUIRE:		strcat(sipMsg, "Require: ");		break;
		case SIP_HEADER_UNSUPPORTED:	strcat(sipMsg, "Unsupported: ");	break;
		default:						return false;
	}

	Simple_List<char>::Iterator it = options.begin();
	while (it != options.end())
	{
		if (it != options.begin())
			strcat(sipMsg, ", ");

		char *option = it->object();
		it++;

		if (!option)
			continue;

		strcat(sipMsg, option);
	}

	strcat(sipMsg, "\r\n");
	return true;
}

const char *ValueOptions::getOption(int pos)
{
	int i = 0;
	Simple_List<char>::Iterator it = options.begin();
	while (it != options.end())
	{
		if (i == pos)
			return it->object();
		i++; it++;
	}
	return 0;
}

void ValueOptions::addOption(const char *option)
{
	char *option2 = createString(option);
	if (!option2)
		return;
	Simple_List<char>::Element *eOption2 = new Simple_List<char>::Element(option2);
	options.insert(eOption2);
}

//-------------------------------------------

void SipHeaderAllow::addAllowed(const SipMessageType allowed)
{
	const char *allowed2 = SipMessage::getMsgType(allowed);
	if (allowed2)
		ValueOptions::addOption(allowed2);
}

//-------------------------------------------

bool SipHeaderContentDisposition::parse(char *sipMsg)
{
	char result[255];

	match(sipMsg, ";", result);
	trim(result);
	string = createString(result);
	if (!string)
		return false;

	return true;
}

//-------------------------------------------

bool SipHeaderContentType::parse(char *sipMsg)
{
	char result[255];

	match(sipMsg, ";", result);
	trim(result);
	if (!strcmp(result, "application/sdp"))
		type = SIP_BODY_APPLICATION_SDP;
	else if (!strcmp(result, "application/pidf+xml"))
		type = SIP_BODY_APPLICATION_PIDF_XML;
	else if (!strcmp(result, "text/plain"))
			type = SIP_BODY_TEXT_PLAIN;
	else
		return false;

	return true;
}

bool SipHeaderContentType::encode(char *sipMsg)
{
	strcat(sipMsg, "Content-Type: ");

	const char *contentType = 0;
	switch (type)
	{
		case SIP_BODY_APPLICATION_SDP:		contentType = "application/sdp";		break;
		case SIP_BODY_APPLICATION_PIDF_XML:	contentType = "application/pidf+xml";	break;
		case SIP_BODY_TEXT_PLAIN:			contentType = "text/plain";				break;
		default: 							break;
	}

	strcat(sipMsg, contentType);
	strcat(sipMsg, "\r\n");
	return true;
}

//-------------------------------------------

bool SipHeaderCSeq::parse(char *sipMsg)
{
	char result[255];
	bool matched;

	skip(sipMsg, " \t");
	matched = match(sipMsg, " \t", result);
	if (!matched)
		return false;
	trim(result);
	sequence = (unsigned int) atol(result);


	trim(sipMsg);
	method = SipMessage::getMsgType(sipMsg);
	if (method == SIP_MESSAGE_TYPE_INVALID)
		return false;

	return true;
}

bool SipHeaderCSeq::encode(char *sipMsg)
{
	char value[11];
	itoa(sequence, value);

	strcat(sipMsg, "CSeq: ");
	strcat(sipMsg, value);
	strcat(sipMsg, " ");
	strcat(sipMsg, SipMessage::getMsgType(method));
	strcat(sipMsg, "\r\n");
	return true;
}

//-------------------------------------------

SipHeaderEvent::SipHeaderEvent(SipHeaderEvent &header)
{
	type = header.type;
	id = createString(header.id);
}

SipHeaderEvent::~SipHeaderEvent()
{
	if (id)
		delete id;
}

bool SipHeaderEvent::parse(char *sipMsg)
{
	char result[255];
	bool matched;

	matched = match(sipMsg, ";", result);
	trim(result);
	if (!strcmp(result, "presence"))
		type = SIP_EVENT_PRESENCE;
	else
		return false;


	while (matched)
	{
		matched = match(sipMsg, ";", result);
		trim(result);

		if (startWith(result, "id="))
		{
			id = createString(&result[3]);
			if (!id)
				return false;
		}
	}

	return true;
}

bool SipHeaderEvent::encode(char *sipMsg)
{
	const char *eventType = 0;
	switch (type)
	{
		case SIP_EVENT_PRESENCE:	eventType = "presence";	break;
		default: 					break;
	}

	strcat(sipMsg, "Event: ");
	strcat(sipMsg, eventType);

	if (id)
	{
		strcat(sipMsg, ";id=");
		strcat(sipMsg, id);
	}

	strcat(sipMsg, "\r\n");
	return true;
}

void SipHeaderEvent::setEvent(SipEventPackage type, const char *id)
{
	this->type = type;

	if (this->id)
		delete this->id;
	this->id = createString(id);
}

//-------------------------------------------

SipHeaderSubscriptionState::SipHeaderSubscriptionState(SipHeaderSubscriptionState &header)
{
	state = header.state;
	expires = header.expires;
}

bool SipHeaderSubscriptionState::parse(char *sipMsg)
{
	char result[255];
	bool matched;

	matched = match(sipMsg, ";", result);
	trim(result);
	if (!strcmp(result, "active"))
		state = SIP_SUBSCRIPTION_STATE_ACTIVE;
	//else if (!strcmp(result, "pending"))
	//	state = SIP_SUBSCRIPTION_STATE_PENDING;
	else if (!strcmp(result, "terminated"))
		state = SIP_SUBSCRIPTION_STATE_TERMINATED;
	else
		return false;


	while (matched)
	{
		matched = match(sipMsg, ";", result);
		trim(result);

		if (startWith(result, "expires="))
			expires = (int) atol(&result[8]);
	}

	return true;
}

bool SipHeaderSubscriptionState::encode(char *sipMsg)
{
	char value[11];
	itoa(expires, value);

	const char *subscriptionState = 0;
	switch (state)
	{
		case SIP_SUBSCRIPTION_STATE_ACTIVE:		subscriptionState = "active";		break;
		//case SIP_SUBSCRIPTION_STATE_PENDING:	subscriptionState = "pending";		break;
		case SIP_SUBSCRIPTION_STATE_TERMINATED:	subscriptionState = "terminated";	break;
		default: 								break;
	}

	strcat(sipMsg, "Subscription-State: ");
	strcat(sipMsg, subscriptionState);

	if (expires >= 0)
	{
		strcat(sipMsg, ";expires=");
		strcat(sipMsg, value);
	}

	strcat(sipMsg, "\r\n");
	return true;
}

void SipHeaderSubscriptionState::setSubscriptionState(SipSubscriptionState state, int expires)
{
	this->state = state;
	this->expires = expires;
}

//-------------------------------------------

SipHeaderVia::SipHeaderVia(SipHeaderVia &header)
{
	protocolName = createString(header.protocolName);
	protocolVersion = createString(header.protocolVersion);
	transport = header.transport;
	sentBy = createString(header.sentBy);
	port = header.port;
	branch = createString(header.branch);
	received = createString(header.received);
}

SipHeaderVia::~SipHeaderVia()
{
	if (protocolName)
		delete protocolName;
	if (protocolVersion)
		delete protocolVersion;
	if (sentBy)
		delete sentBy;
	if (branch)
		delete branch;
	if (received)
		delete received;
}

bool SipHeaderVia::parse(char *sipMsg)
{
	char result[255];
	bool matched;

	matched = match(sipMsg, "/", result);
	if (!matched)
		return false;
	trim(result);
	protocolName = createString(result);
	if (!protocolName)
		return false;


	matched = match(sipMsg, "/", result);
	if (!matched)
		return false;
	trim(result);
	protocolVersion = createString(result);
	if (!protocolVersion)
		return false;


	skip(sipMsg, " \t");
	matched = match(sipMsg, " \t", result);
	if (!matched)
		return false;
	trim(result);
	transport = SipMessage::getTransportType(result);
	if (transport == SIP_TRANSPORT_TYPE_INVALID)
		return false;


	matched = match(sipMsg, ":", result);
	if (matched)
	{
		trim(result);
		sentBy = createString(result);
		if (!sentBy)
			return false;

		matched = match(sipMsg, ";", result);
		trim(result);
		port = (int) atol(result);
	}else
	{
		matched = match(sipMsg, ";", result);
		trim(result);
		sentBy = createString(result);
		if (!sentBy)
			return false;

		port = 5060; //TODO
	}


	while (matched)
	{
		matched = match(sipMsg, ";", result);
		trim(result);

		if (startWith(result, "branch="))
		{
			branch = createString(&result[7]);
			if (!branch)
				return false;
		}else if (startWith(result, "received="))
		{
			received = createString(&result[9]);
			if (!received)
				return false;
		}
	}

	return true;
}

bool SipHeaderVia::encode(char *sipMsg)
{
	char value[11];
	itoa(port, value);

	const char *type = SipMessage::getTransportType(transport);

	strcat(sipMsg, "Via: ");
	strcat(sipMsg, protocolName);
	strcat(sipMsg, "/");
	strcat(sipMsg, protocolVersion);
	strcat(sipMsg, "/");
	strcat(sipMsg, type);
	strcat(sipMsg, " ");
	strcat(sipMsg, sentBy);
	strcat(sipMsg, ":");
	strcat(sipMsg, value);

	if (branch)
	{
		strcat(sipMsg, ";branch=");
		strcat(sipMsg, branch);
	}

	if (received)
	{
		strcat(sipMsg, ";received=");
		strcat(sipMsg, received);
	}

	strcat(sipMsg, "\r\n");
	return true;
}

void SipHeaderVia::setVia(const char *protocolName, const char *protocolVersion,
	const SipTransportType transport, const char *sentBy, int port,
	const char *branch)
{
	if (this->protocolName)
		delete this->protocolName;
	this->protocolName = createString(protocolName);

	if (this->protocolVersion)
		delete this->protocolVersion;
	this->protocolVersion = createString(protocolVersion);

	this->transport = transport;

	if (this->sentBy)
		delete this->sentBy;
	this->sentBy = createString(sentBy);

	this->port = port;

	if (this->branch)
		delete this->branch;
	this->branch = createString(branch);
}

void SipHeaderVia::setReceived(const char *received)
{
	if (this->received)
		delete this->received;
	this->received = createString(received);
}

__END_SYS
