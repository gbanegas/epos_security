#include <sip_body.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <ip/ip.h>

__BEGIN_SYS


SipBody *SipBody::decodeBody(const char *sipMsg, SipBodyType bodyType)
{
	SipBody *body = createBody(bodyType);

	if (!body)
	{
		db<SipBody>(WRN) << "SipBody::decodeBody -> Invalid body type\n";
		return 0;
	}

	if (!body->parse(sipMsg))
	{
		db<SipBody>(WRN) << "SipBody::decodeBody -> Failed to parse " << bodyType << " body type\n";
		delete body;
		body = 0;
	}

	return body;
}

SipBody *SipBody::createBody(SipBodyType type/*, const SipBody *copy*/)
{
	SipBody *body = 0;

	switch (type)
	{
		case SIP_BODY_APPLICATION_SDP:		body = /*(!copy) ?*/ new SipSdpBody()		/*: new SipSdpBody(*((SipSdpBody *) copy))*/;				break;
		case SIP_BODY_APPLICATION_PIDF_XML:	body = /*(!copy) ?*/ new SipPidfXmlBody()	/*: new SipPidfXmlBody(*((SipPidfXmlBody *) copy))*/;		break;
		case SIP_BODY_TEXT_PLAIN:			body = /*(!copy) ?*/ new SipTextPlainBody()	/*: new SipTextPlainBody(*((SipTextPlainBody *) copy))*/;	break;
		default:							break;
	}

	return body;
}

//-------------------------------------------

bool SipSdpBody::encode(char *sipMsg)
{
	char local[20];
	((IP::Address &) IP::instance(0)->address()).to_string(local);

	strcat(sipMsg, "v=0\r\n");
	strcat(sipMsg, "o=- 0 0 IN IP4 ");
	strcat(sipMsg, local);
	strcat(sipMsg, "\r\n");
	strcat(sipMsg, "s=-\r\n");
	strcat(sipMsg, "c=IN IP4 ");
	strcat(sipMsg, local);
	strcat(sipMsg, "\r\n");
	strcat(sipMsg, "t=0 0\r\n");
	strcat(sipMsg, "m=audio 8000 RTP/AVP 0\r\n");
	strcat(sipMsg, "a=rtpmap:0 pcmu/8000\r\n");
	return true;
}

//-------------------------------------------

bool SipPidfXmlBody::encode(char *sipMsg)
{
	char local[20];
	((IP::Address &) IP::instance(0)->address()).to_string(local);

	if (element == SIP_PIDF_XML_OPEN)
	{
		strcat(sipMsg, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		strcat(sipMsg, "<presence xmlns=\"urn:ietf:params:xml:ns:pidf\" entity=\"pres:1111@"); //TODO: pres
		strcat(sipMsg, local);
		strcat(sipMsg, "\">");
		strcat(sipMsg, "<tuple id=\"sg89ae\">");
		strcat(sipMsg, "<status>");
		strcat(sipMsg, "<basic>open</basic>");
		strcat(sipMsg, "</status>");
		strcat(sipMsg, "</tuple>");
		strcat(sipMsg, "</presence>");

	}else if (element == SIP_PIDF_XML_CLOSED)
	{
		strcat(sipMsg, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
		strcat(sipMsg, "<presence xmlns=\"urn:ietf:params:xml:ns:pidf\" entity=\"pres:1111@"); //TODO: pres
		strcat(sipMsg, local);
		strcat(sipMsg, "\">");
		strcat(sipMsg, "<tuple id=\"sg89ae\">");
		strcat(sipMsg, "<status>");
		strcat(sipMsg, "<basic>closed</basic>");
		strcat(sipMsg, "</status>");
		strcat(sipMsg, "</tuple>");
		strcat(sipMsg, "</presence>");
	}
	return true;
}

//-------------------------------------------

void SipTextPlainBody::setText(const char *text)
{
	if (this->text)
		delete this->text;
	this->text = createString(text);
}

__END_SYS
