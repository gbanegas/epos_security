#ifndef __sip_body_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __sip_body_h

#include <sip_defs.h>

__BEGIN_SYS


class SipBody
{
public:
	//SipBody() {};
	virtual ~SipBody() {}

	static SipBody *decodeBody(const char *sipMsg, SipBodyType bodyType);
	static SipBody *createBody(SipBodyType type/*, const SipBody *copy = 0*/);

	virtual SipBodyType getBodyType() = 0;

	virtual bool parse(const char *sipMsg) = 0;
	virtual bool encode(char *sipMsg) = 0;
};

//-------------------------------------------

class SipSdpBody : public SipBody
{
public:
	SipSdpBody() {};
	//SipSdpBody(SipSdpBody &body) {};
	~SipSdpBody() {};

	SipBodyType getBodyType() { return SIP_BODY_APPLICATION_SDP; };

	bool parse(const char *sipMsg) { return true; };
	bool encode(char *sipMsg);
};

//-------------------------------------------

class SipPidfXmlBody : public SipBody
{
	SipPidfXmlBasicElement element;

public:
	SipPidfXmlBody() {};
	//SipPidfXmlBody(SipPidfXmlBody &body) { element = body.element; };
	~SipPidfXmlBody() {};

	SipBodyType getBodyType() { return SIP_BODY_APPLICATION_PIDF_XML; };

	bool parse(const char *sipMsg) { return true; };
	bool encode(char *sipMsg);

	void setElement(SipPidfXmlBasicElement element) { this->element = element; };
};

//-------------------------------------------

class SipTextPlainBody : public SipBody
{
private:
	char *text;

public:
	SipTextPlainBody() : text(0) {};
	//SipTextPlainBody(SipTextPlainBody &body) { text = createString(body.text); };
	~SipTextPlainBody() { if (text) delete text; };

	SipBodyType getBodyType() { return SIP_BODY_TEXT_PLAIN; };

	bool parse(const char *sipMsg) { text = createString(sipMsg); return true; };
	bool encode(char *sipMsg) { strcat(sipMsg, text); return true; };

	void setText(const char *text);
	const char *getText() { return text; };
};

__END_SYS

#endif
