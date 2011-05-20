

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <sip_user_agent.h>

__BEGIN_SYS


SipDialog::SipDialog(SipMessageType type) : type(type), link(this)
{
	//curState = sttIdle;

	callID = 0;
	localTag = 0;
	remoteTag = 0;
	localSequenceNumber = 0;
	remoteSequenceNumber = 0;
	localURI = 0;
	remoteURI = 0;
	remoteTarget = 0;
}

SipDialog::~SipDialog()
{
	if (callID)
		delete callID;

	if (localTag)
		delete localTag;

	if (remoteTag)
		delete remoteTag;

	if (localURI)
		delete localURI;

	if (remoteURI)
		delete remoteURI;

	if (remoteTarget)
		delete remoteTarget;

	clearRouteSet();
}

void SipDialog::setDialog(const char *callID, const char *localTag, const char *remoteTag,
				unsigned int localSequenceNumber, unsigned int remoteSequenceNumber,
				const char *localURI, const char *remoteURI, const char *remoteTarget)
{
	if (this->callID) delete this->callID;
	this->callID = createString(callID);

	if (this->localTag) delete this->localTag;
	this->localTag = createString(localTag);

	if (this->remoteTag) delete this->remoteTag;
	this->remoteTag = createString(remoteTag);

	this->localSequenceNumber = localSequenceNumber;

	this->remoteSequenceNumber = remoteSequenceNumber;

	if (this->localURI) delete this->localURI;
	this->localURI = createString(localURI);

	if (this->remoteURI) delete this->remoteURI;
	this->remoteURI = createString(remoteURI);

	if (this->remoteTarget) delete this->remoteTarget;
	this->remoteTarget = createString(remoteTarget);

	//this->curState = sttConfirmed;
}

void SipDialog::setRemoteTarget(const char *remoteTarget)
{
	if (this->remoteTarget) delete this->remoteTarget;
	this->remoteTarget = createString(remoteTarget);
}

void SipDialog::addRouteBack(SipHeaderRoute *route)
{
	SipHeaderRoute *route2 = new SipHeaderRoute(*route);
	routeSet.insert_tail(&route2->link);
}

void SipDialog::addRouteFront(SipHeaderRoute *route)
{
	SipHeaderRoute *route2 = new SipHeaderRoute(*route);
	routeSet.insert_head(&route2->link);
}

void SipDialog::clearRouteSet()
{
	Simple_List<SipHeader/*Route*/>::Iterator it = routeSet.begin();
	while (it != routeSet.end())
	{
		SipHeaderRoute *route = (SipHeaderRoute *) it->object();
		routeSet.remove(it++);
		delete route;
	}
}

SipHeaderRoute *SipDialog::getRoute(int pos)
{
	int i = 0;
	Simple_List<SipHeader/*Route*/>::Iterator it = routeSet.begin();
	while (it != routeSet.end())
	{
		if (i == pos)
			return (SipHeaderRoute *) it->object();
		i++; it++;
	}
	return 0;
}

//-------------------------------------------

void SipSubscription::setSubscription(SipEventPackage eventType, const char *eventId)
{
	this->eventType = eventType;

	if (this->eventId) delete this->eventId;
	this->eventId = createString(eventId);
}

/*void SipSubscription::startTimer(void *p)
{
	timerHandler = new Functor_Handler(&, p);
	timerAlarm = new Alarm(timerValue, timerHandler, 1);
}

void SipSubscription::stopTimer()
{
	if (timerHandler)
	{
		delete timerHandler;
		timerHandler = 0;
	}

	if (timerAlarm)
	{
		delete timerAlarm;
		timerAlarm = 0;
	}
}*/

void SipSubscription::clear()
{
	eventType = SIP_EVENT_PACKAGE_INVALID;

	if (eventId)
		delete eventId;
	eventId = 0;

	//stopTimer();
}

//-------------------------------------------

SipRequest *UserAgentClient::createRequest(SipMessageType msgType, SipDialog *dialog, const char *to, SipMessage *invite)
{
	SipRequest *request = 0;

	switch (msgType)
	{
		case SIP_REQUEST_ACK:			request = new SipRequestAck();			break;
		case SIP_REQUEST_BYE:			request = new SipRequestBye();			break;
		//case SIP_REQUEST_CANCEL:		request = new SipRequestCancel();		break;
		case SIP_REQUEST_INVITE:		request = new SipRequestInvite();		break;
		case SIP_REQUEST_MESSAGE:		request = new SipRequestMessage();		break;
		case SIP_REQUEST_NOTIFY:		request = new SipRequestNotify();		break;
		//case SIP_REQUEST_OPTIONS:		request = new SipRequestOptions();		break;
		//case SIP_REQUEST_REGISTER:	request = new SipRequestRegister();		break;
		case SIP_REQUEST_SUBSCRIBE:		request = new SipRequestSubscribe();	break;
		//case SIP_RESPONSE:			break;
		default:						break;
	}

	/*if (!request)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createRequest -> Failed to create request (message type = " << msgType << ")\n";
		return 0;
	}*/

	SipHeaderVia *headerVia = new SipHeaderVia();
	char branch[20];
	strcpy(branch, "z9hG4bK");
	SipManager::random(&branch[7]);
	headerVia->setVia("SIP", "2.0", SIP_TRANSPORT_UDP, SipManager::getInstance()->getTransport()->getHostIP(), SipManager::getInstance()->getTransport()->getHostPort(), branch);
	request->addHeader(headerVia);

	SipHeaderTo *headerTo = new SipHeaderTo();
	request->addHeader(headerTo);

	SipHeaderFrom *headerFrom = new SipHeaderFrom();
	request->addHeader(headerFrom);

	SipHeaderCallID *headerCallID = new SipHeaderCallID();
	request->addHeader(headerCallID);

	SipHeaderCSeq *headerCSeq = new SipHeaderCSeq();
	request->addHeader(headerCSeq);

	SipHeaderMaxForwards *headerMaxForwards = new SipHeaderMaxForwards();
	headerMaxForwards->setNumber(70);
	request->addHeader(headerMaxForwards);

	SipHeaderContact *headerContact = new SipHeaderContact();
	request->addHeader(headerContact);


	if (!dialog)
	{
		headerTo->setAddress(to);

		char tag[20];
		SipManager::random(tag);
		headerFrom->setAddress(ua->uri);
		headerFrom->setTag(tag);

		char callid[100], host[255], aux[255];
		//static int localidResgister = 2900;
		strcpy(host, ua->uri);
		match(host, ":" , aux);
		skip(host, " \t");
		match(host, "@" , aux);
		skip(host, " \t");
		//if (msgType == SIP_REQUEST_REGISTER)
		//	itoa(localidResgister, callid);
		//else
			SipManager::random(callid);
		strcat(callid, "@");
		strcat(callid, host);
		headerCallID->setString(callid);

		static unsigned int seq = 1;
		headerCSeq->setCSeq(msgType, seq++);

		headerContact->setAddress(ua->uri);

		request->setRequestLine(msgType, to, SIP_VERSION);
	}else
	{
		headerTo->setAddress(dialog->remoteURI);
		if (dialog->remoteTag)
			headerTo->setTag(dialog->remoteTag);

		headerFrom->setAddress(dialog->localURI);
		if (dialog->localTag)
			headerFrom->setTag(dialog->localTag);

		headerCallID->setString(dialog->callID);

		if (dialog->localSequenceNumber == 0)
			dialog->localSequenceNumber = 1;
		unsigned int seq = 0;
		if ((msgType == SIP_REQUEST_ACK)) //|| (msgType == SIP_REQUEST_CANCEL))
		{
			SipHeaderCSeq *cseq = (SipHeaderCSeq *) invite->getHeader(SIP_HEADER_CSEQ);
			seq = cseq->getSequence();
		}else
		{
			seq = dialog->localSequenceNumber + 1;
			dialog->localSequenceNumber = seq;
		}
		headerCSeq->setCSeq(msgType, seq);

		headerContact->setAddress(dialog->localURI);

		const char *remote = 0;
		int routeSetSize = dialog->getRouteSetSize();
		if (routeSetSize == 0)
			remote = dialog->remoteTarget;
		else if (routeSetSize > 0)
		{
			SipHeaderRoute *route = dialog->getRoute(0);
			if (route->isLR())
			{
				remote = dialog->remoteTarget;

				for (int i = 0; i < routeSetSize; i++)
					request->addHeader(new SipHeaderRoute(*(dialog->getRoute(i))));
			}else
			{
				remote = dialog->getRoute(0)->getAddress();

				for (int i = 1; i < routeSetSize; i++)
					request->addHeader(new SipHeaderRoute(*(dialog->getRoute(i))));

				SipHeaderRoute *header = new SipHeaderRoute();
				header->setAddress(dialog->remoteTarget, true);
				request->addHeader(header);
			}
		}

		char pRemote[512];
		strcpy(pRemote, remote);
		int size = strlen(pRemote);
		for (int i = size - 1; i >= size - 6; i--)
		{
			if (pRemote[i] == ':')
			{
				pRemote[i] = 0;
				break;
			}
		}
		request->setRequestLine(msgType, pRemote, SIP_VERSION);
	}

	return request;
}

SipRequestAck *UserAgentClient::createAck(const char *to, SipRequestInvite *invite)
{
	SipDialog *dialog = ua->matchingDialog(to, SIP_REQUEST_INVITE);

	if ((!dialog) || (!invite))
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createAck -> Failed to create request\n";
		return 0;
	}

	return (SipRequestAck *) createRequest(SIP_REQUEST_ACK, dialog, 0, invite);
}

SipRequestBye *UserAgentClient::createBye(const char *to)
{
	SipDialog *dialog = ua->matchingDialog(to, SIP_REQUEST_INVITE);

	if (!dialog)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createBye -> Failed to create request\n";
		return 0;
	}

	return (SipRequestBye *) createRequest(SIP_REQUEST_BYE, dialog);
}

SipRequestInvite *UserAgentClient::createInvite(const char *to)
{
	SipDialog *dialog = ua->matchingDialog(to, SIP_REQUEST_INVITE);

	if ((dialog) || (!to))
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createInvite -> Failed to create request\n";
		return 0;
	}

	SipRequestInvite *invite = (SipRequestInvite *) createRequest(SIP_REQUEST_INVITE, 0, to);

	SipHeaderAllow *headerAllow = new SipHeaderAllow();
	for (int i = 0; i < (SIP_MESSAGE_TYPE_INVALID - 1); i++)
		headerAllow->addAllowed((SipMessageType) i);
	invite->addHeader(headerAllow);

	SipHeaderContentDisposition *headerContentDisposition = new SipHeaderContentDisposition();
	headerContentDisposition->setString("session");
	invite->addHeader(headerContentDisposition);

	SipSdpBody *sdp = new SipSdpBody();
	invite->setBody(sdp);

	return invite;
}

SipRequestMessage *UserAgentClient::createMessage(const char *to, const char *data)
{
	//SipDialog *dialog = ua->matchingDialog(to);

	if ((!to) || (!data))
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createMessage -> Failed to create request\n";
		return 0;
	}

	SipRequestMessage *message = (SipRequestMessage *) createRequest(SIP_REQUEST_MESSAGE, 0, to);

	SipTextPlainBody *text = new SipTextPlainBody();
	text->setText(data);
	message->setBody(text);

	return message;
}

SipRequestNotify *UserAgentClient::createNotify(const char *to, SipSubscriptionState state, SipPidfXmlBasicElement pidfXmlElement, unsigned int expires)
{
	SipDialog *dialog = ua->matchingDialog(to, SIP_REQUEST_SUBSCRIBE);

	if ((!dialog) || (!ua->subscription.isActive()))
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createNotify -> Failed to create request\n";
		return 0;
	}

	SipRequestNotify *notify = (SipRequestNotify *) createRequest(SIP_REQUEST_NOTIFY, dialog);

	SipHeaderEvent *headerEvent = new SipHeaderEvent();
	headerEvent->setEvent(ua->subscription.eventType, ua->subscription.eventId);
	notify->addHeader(headerEvent);

	SipHeaderSubscriptionState *headerSubscriptionState = new SipHeaderSubscriptionState();
	headerSubscriptionState->setSubscriptionState(state, expires);
	notify->addHeader(headerSubscriptionState);

	SipPidfXmlBody *pidf = new SipPidfXmlBody();
	pidf->setElement(pidfXmlElement);
	notify->setBody(pidf);

	return notify;
}

void UserAgentClient::sendRequest(SipRequest *request)
{
	if (!request)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::sendRequest -> Invalid parameter\n";
		return;
	}

	if (request->getMsgType() == SIP_REQUEST_INVITE)
	{
		SipTransactionClientInvite *transaction = new SipTransactionClientInvite(ua);
		ua->addTransaction(transaction);
		transaction->sendInvite((SipRequestInvite *) request);
	}else
	{
		SipTransactionClientNonInvite *transaction = new SipTransactionClientNonInvite(ua);
		ua->addTransaction(transaction);
		transaction->sendRequest(request);
	}

	if (request->getCanDelete())
		delete request;
}

bool UserAgentClient::receiveMsg(SipResponse *response)
{
	if (response->getNumHeader(SIP_HEADER_VIA) != 1)
		return false;

	int statusCode = response->getStatusLine()->getStatusCode();
	SipTransaction *transaction = ua->matchingTransaction(response);

	if (!transaction)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::receiveMsg -> Ignoring invalid response\n";
		return false;
	}

	if (transaction->getTransactionType() == SIP_TRANSACTION_CLIENT_INVITE)
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((SipTransactionClientInvite *) transaction)->receive1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 299))
			((SipTransactionClientInvite *) transaction)->receive2xx(response);

		else if ((statusCode >= 300) && (statusCode <= 699))
			((SipTransactionClientInvite *) transaction)->receive3xx6xx(response);

	}else if (transaction->getTransactionType() == SIP_TRANSACTION_CLIENT_NON_INVITE)
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((SipTransactionClientNonInvite *) transaction)->receive1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 699))
			((SipTransactionClientNonInvite *) transaction)->receive2xx6xx(response);
	}

	return true;
}

bool UserAgentClient::receiveMsg(SipRequest *request, SipResponse *response, SipTransaction *transaction)
{
	int statusCode = response->getStatusLine()->getStatusCode();
	SipDialog *dialog = ua->matchingDialog(response, request->getMsgType());

	if ((statusCode >= 200) && (statusCode <= 299))
		receive2xx(request, response, transaction, dialog);

	else if ((statusCode >= 300) && (statusCode <= 699))
		receive3xx6xx(request, response, transaction, dialog);

	return true;
}

bool UserAgentClient::receive2xx(SipRequest *request, SipResponse *response, SipTransaction *transaction, SipDialog *dialog)
{
	switch (request->getMsgType())
	{
		case SIP_REQUEST_INVITE:
		{
			if (!dialog)
			{
				SipDialog *newDialog = createDialog(request, response);
				if (newDialog)
				{
					SipRequestAck *ack = createAck(newDialog->remoteURI, (SipRequestInvite *) request);
					SipManager::getInstance()->getTransport()->sendMessage(ack);
					delete ack;
				}
			}else
			{
				SipHeaderContact *contact = (SipHeaderContact *) response->getHeader(SIP_HEADER_CONTACT);
				if (contact)
				{
					const char *target = contact->getAddress();
					if (target)
						dialog->setRemoteTarget(target);
				}
			}
			break;
		}

		case SIP_REQUEST_BYE:
		{
			if (dialog)
			{
				SipManager::callback(SIP_SESSION_TERMINATED, ua, dialog->remoteURI);
				ua->removeDialog(dialog);
			}
			break;
		}

		default:
			break;
	}

	return true;
}

bool UserAgentClient::receive3xx6xx(SipRequest *request, SipResponse *response, SipTransaction *transaction, SipDialog *dialog)
{
	int statusCode = response->getStatusLine()->getStatusCode();

	if ((statusCode == 408) || (statusCode == 481) || (request->getMsgType() == SIP_REQUEST_BYE))
	{
		if (dialog)
		{
			SipManager::callback(SIP_SESSION_TERMINATED, ua, dialog->remoteURI);
			ua->removeDialog(dialog);
		}
		return true;
	}

	if (dialog)
	{
		SipRequestBye *bye = createBye(dialog->remoteURI);
		sendRequest(bye);
		//delete bye;

		SipManager::callback(SIP_SESSION_TERMINATED, ua, dialog->remoteURI);
		ua->removeDialog(dialog);
	}

	return true;
}

SipDialog *UserAgentClient::createDialog(SipRequest *request, SipResponse *response)
{
	SipHeaderFrom *fromRequest = (SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM);
	SipHeaderFrom *fromResponse = (SipHeaderFrom *) response->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) request->getHeader(SIP_HEADER_CALLID);
	SipHeaderContact *contact = (SipHeaderContact *) response->getHeader(SIP_HEADER_CONTACT);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
	SipHeaderTo *to = (SipHeaderTo *) response->getHeader(SIP_HEADER_TO);

	if ((!fromRequest) || (!fromResponse) || (!callId) || (!contact) || (!cseq) || (!to))
		return 0;

	const char *id = callId->getString();
	const char *localTag = fromRequest->getTag();
	const char *remoteTag = to->getTag(); //Pode ser NULL, compatibilidade com RFC 2543
	const char *localURI = fromResponse->getAddress();
	const char *remoteURI = to->getAddress();
	const char *target = contact->getAddress();
	unsigned int sequenceNumber = cseq->getSequence();

	if ((!id) || (!localTag) || (!localURI) || (!remoteURI) || (!target))
		return 0;

	SipDialog *dialog = ua->addDialog(SIP_REQUEST_INVITE);
	dialog->setDialog(id, localTag, remoteTag, sequenceNumber, 0, localURI, remoteURI, target);

	int recordRouteNum = response->getNumHeader(SIP_HEADER_RECORD_ROUTE);
	for (int i = 0; i < recordRouteNum; i++)
	{
		SipHeaderRoute *route = (SipHeaderRoute *) response->getHeader(SIP_HEADER_RECORD_ROUTE, i);
		dialog->addRouteFront(route);
	}

	SipManager::callback(SIP_SESSION_INITIATED, ua, remoteURI);
	return dialog;
}

//-------------------------------------------

SipResponse *UserAgentServer::createResponse(int statusCode, SipRequest *request)
{
	if ((statusCode < 100) || (statusCode > 699) || (!request))
	{
		db<UserAgentServer>(WRN) << "UserAgentServer::createResponse -> Invalid parameters\n";
		return 0;
	}

	int viaNum = request->getNumHeader(SIP_HEADER_VIA);
	SipHeaderFrom *from = (SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) request->getHeader(SIP_HEADER_CALLID);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
	SipHeaderTo *to = (SipHeaderTo *) request->getHeader(SIP_HEADER_TO);

	if ((viaNum == 0) || (!from) || (!callId) || (!cseq) || (!to))
		return 0;

	SipResponse *response = new SipResponse(statusCode);

	for (int i = 0; i < viaNum; i++)
	{
		SipHeaderVia *via = (SipHeaderVia *) request->getHeader(SIP_HEADER_VIA, i);
		response->addHeader(new SipHeaderVia(*via));
	}

	SipHeaderTo *headerTo = new SipHeaderTo(*to);
	response->addHeader(headerTo);
	const char *toTag = headerTo->getTag();
	if ((!toTag) && (statusCode != 100))
	{
		char tag[20];
		SipManager::random(tag);
		headerTo->setTag(tag);
	}

	response->addHeader(new SipHeaderFrom(*from));
	response->addHeader(new SipHeaderCallID(*callId));
	response->addHeader(new SipHeaderCSeq(*cseq));

	SipHeaderMaxForwards *headerMaxForwards = new SipHeaderMaxForwards();
	headerMaxForwards->setNumber(70);
	response->addHeader(headerMaxForwards);

	if ((request->getMsgType() == SIP_REQUEST_INVITE) || (request->getMsgType() == SIP_REQUEST_SUBSCRIBE))
	{
		SipHeaderContact *headerContact = new SipHeaderContact();
		const char *address = headerTo->getAddress();
		headerContact->setAddress(address);
		response->addHeader(headerContact);
	}

	if (request->getMsgType() == SIP_REQUEST_INVITE)
	{
		SipHeaderContentDisposition *headerContentDisposition = new SipHeaderContentDisposition();
		headerContentDisposition->setString("session");
		response->addHeader(headerContentDisposition);

		SipSdpBody *sdp = new SipSdpBody();
		response->setBody(sdp);
	}

	return response;
}

void UserAgentServer::sendResponse(SipResponse *response, SipMessageType requestType, SipTransaction *transaction)
{
	if ((!response) || (!transaction))
	{
		db<UserAgentServer>(WRN) << "UserAgentServer::sendResponse -> Invalid parameters\n";
		return;
	}

	int statusCode = response->getStatusLine()->getStatusCode();

	if (requestType == SIP_REQUEST_INVITE)
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((SipTransactionServerInvite *) transaction)->send1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 299))
			((SipTransactionServerInvite *) transaction)->send2xx(response);

		else if ((statusCode >= 300) && (statusCode <= 699))
			((SipTransactionServerInvite *) transaction)->send3xx6xx(response);
	}else
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((SipTransactionServerNonInvite *) transaction)->send1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 699))
			((SipTransactionServerNonInvite *) transaction)->send2xx6xx(response);
	}

	if (response->getCanDelete())
		delete response;
}

bool UserAgentServer::receiveMsg(SipRequest *request)
{
	SipMessageType requestType = request->getMsgType();
	SipTransaction *transaction = ua->matchingTransaction(request);

	if ((!transaction) && (requestType == SIP_REQUEST_ACK))
	{
		db<UserAgentServer>(WRN) << "UserAgentServer::receiveMsg -> Invalid ACK message\n";
		return false;

	}else if ((!transaction) && (requestType == SIP_REQUEST_INVITE))
	{
		//if (dialog) //TODO: Se já existe um dialog, não recebe mais INVITE!
		//{
		//	db<UserAgentServer>(WRN) << "UserAgentServer::receiveMsg -> There is already a dialog\n";
		//	return false;
		//}

		transaction = new SipTransactionServerInvite(ua);
		ua->addTransaction(transaction);

	}else if (!transaction) //&& (requestType != SIP_REQUEST_INVITE) && (requestType != SIP_REQUEST_ACK))
	{
		transaction = new SipTransactionServerNonInvite(ua);
		ua->addTransaction(transaction);
	}

	db<UserAgentServer>(INF) << "UserAgentServer::receiveMsg -> New request received\n";

	if (requestType == SIP_REQUEST_INVITE)
		((SipTransactionServerInvite *) transaction)->receiveInvite((SipRequestInvite *) request);

	else if (requestType == SIP_REQUEST_ACK)
		((SipTransactionServerInvite *) transaction)->receiveAck((SipRequestAck *) request);

	else
		((SipTransactionServerNonInvite *) transaction)->receiveRequest(request);

	return true;
}

bool UserAgentServer::receiveMsg(SipRequest *request, SipTransaction *transaction)
{
	SipMessageType requestType = request->getMsgType();
	SipDialog *dialog = ua->matchingDialog(request, requestType);

	if ((requestType != SIP_REQUEST_ACK)) //&& (requestType != SIP_REQUEST_CANCEL))
	{
		SipHeaderRequire *require = (SipHeaderRequire *) request->getHeader(SIP_HEADER_REQUIRE);
		if (require)
		{
			SipResponse *response = createResponse(420, request);
			SipHeaderUnsupported *unsupported = new SipHeaderUnsupported();
			response->addHeader(unsupported);

			for (int i = 0; i < require->getOptionSize(); i++)
			{
				const char *option = require->getOption(i);
				unsupported->addOption(option);
			}

			sendResponse(response, requestType, transaction);
			//delete response;
			return false;
		}
	}

	if (!dialog)
	{
		SipHeaderTo *headerTo = (SipHeaderTo *) request->getHeader(SIP_HEADER_TO);
		if (!headerTo)
			return false;

		const char *remoteTag = headerTo->getTag();
		if (remoteTag)
		{
			SipResponse *response = createResponse(481, request);
			sendResponse(response, requestType, transaction);
			//delete response;
			return false;
		}
	}else
	{
		SipHeaderCSeq *headerCSeq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
		if (!headerCSeq)
			return false;

		unsigned int sequenceNumber = headerCSeq->getSequence();
		unsigned int dialogSequenceNumber = dialog->remoteSequenceNumber;

		if ((dialogSequenceNumber == 0) || (sequenceNumber > dialogSequenceNumber))
			dialog->remoteSequenceNumber = sequenceNumber;
		/*else if (sequenceNumber < dialogSequenceNumber) //TODO: Descomentar quando verificar Dialog!
		{
			SipResponse *response = createResponse(500, request);
			sendResponse(response, requestType, transaction);
			//delete response;
			return false;
		}*/
	}


	switch (requestType)
	{
		case SIP_REQUEST_ACK:		return true; //receiveAck((SipRequestAck *) request, transaction, dialog);
		case SIP_REQUEST_BYE:		return receiveBye((SipRequestBye *) request, transaction, dialog);
		case SIP_REQUEST_INVITE:	return receiveInvite((SipRequestInvite *) request, transaction, dialog);
		case SIP_REQUEST_MESSAGE:	return receiveMessage((SipRequestMessage *) request, transaction, dialog);
		case SIP_REQUEST_NOTIFY:	return true; //receiveNotify((SipRequestNotify *) request, transaction, dialog);
		case SIP_REQUEST_SUBSCRIBE:	return receiveSubscribe((SipRequestSubscribe *) request, transaction, dialog);
		default:					break;
	}

	return false;
}

bool UserAgentServer::receiveBye(SipRequestBye *request, SipTransaction *transaction, SipDialog *dialog)
{
	if (!dialog)
	{
		SipResponse *response = createResponse(481, request);
		sendResponse(response, SIP_REQUEST_BYE, transaction);
		//delete response;
	}else
	{
		SipResponse *response = createResponse(200, request);
		sendResponse(response, SIP_REQUEST_BYE, transaction);
		//delete response;

		SipManager::callback(SIP_SESSION_TERMINATED, ua, dialog->remoteURI);
		ua->removeDialog(dialog);
	}

	return true;
}

bool UserAgentServer::receiveInvite(SipRequestInvite *request, SipTransaction *transaction, SipDialog *dialog)
{
	if (!dialog)
	{
		SipResponse *response = createResponse(200, request);

		int recordRouteNum = request->getNumHeader(SIP_HEADER_RECORD_ROUTE);
		for (int i = 0; i < recordRouteNum; i++)
		{
			SipHeaderRecordRoute *headerRecordRoute = (SipHeaderRecordRoute *) request->getHeader(SIP_HEADER_RECORD_ROUTE, i);
			response->addHeader(new SipHeaderRecordRoute(*headerRecordRoute));
		}

		SipHeaderAllow *headerAllow = new SipHeaderAllow();
		for (int i = 0; i < (SIP_MESSAGE_TYPE_INVALID - 1); i++)
			headerAllow->addAllowed((SipMessageType) i);
		response->addHeader(headerAllow);

		createDialog(request, response);
		sendResponse(response, SIP_REQUEST_INVITE, transaction);
		//delete response;

		//TODO: 13.3.1.4 The INVITE is Accepted: Reenviar periodicamente a resposta até receber ACK
	}else
	{
		SipHeaderContact *contact = (SipHeaderContact *) request->getHeader(SIP_HEADER_CONTACT);
		if (contact)
		{
			const char *target = contact->getAddress();
			if ((target) && (!strcmp(dialog->remoteTarget, target)))
				dialog->setRemoteTarget(target);
		}
	}

	return true;
}

bool UserAgentServer::receiveMessage(SipRequestMessage *request, SipTransaction *transaction, SipDialog *dialog)
{
	SipResponse *response = createResponse(200, request);
	sendResponse(response, SIP_REQUEST_MESSAGE, transaction);
	//delete response;

	SipBody *body = request->getBody();
	if ((!body) || (body->getBodyType() != SIP_BODY_TEXT_PLAIN))
		return false;

	const char *remoteURI = ((SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM))->getAddress();
	ua->textReceived = ((SipTextPlainBody *) body)->getText();

	SipManager::callback(SIP_MESSAGE_RECEIVED, ua, remoteURI);

	ua->textReceived = 0;
	return true;
}

bool UserAgentServer::receiveSubscribe(SipRequestSubscribe *request, SipTransaction *transaction, SipDialog *dialog)
{
	SipHeaderEvent *headerEvent = (SipHeaderEvent *) request->getHeader(SIP_HEADER_EVENT);
	if (!headerEvent)
		return false;

	SipEventPackage eventType = headerEvent->getType();

	if (((!ua->subscription.isActive()) && (eventType == SIP_EVENT_PRESENCE)) ||
		((ua->subscription.isActive()) && (eventType == ua->subscription.eventType)))
	{
		SipResponse *response = createResponse(200, request);

		SipHeaderExpires *expires = (SipHeaderExpires *) request->getHeader(SIP_HEADER_EXPIRES);
		SipHeaderExpires *headerExpires = new SipHeaderExpires();
		unsigned int time = (expires) ? expires->getNumber() : 3600;
		headerExpires->setNumber(time);
		response->addHeader(headerExpires);

		if ((!dialog) && (time > 0))
			createDialog(request, response);

		sendResponse(response, SIP_REQUEST_SUBSCRIBE, transaction);
		//delete response;

		if (dialog)
		{
			if (time > 0)
			{
				//TODO: Acionar ou atualizar timer por tempo do Expires
			}else
			{
				SipManager::callback(SIP_SUBSCRIPTION_TERMINATED, ua, dialog->remoteURI);
				ua->subscription.clear();
				ua->removeDialog(dialog); //TODO: Deletar mesmo se tem INVITE session?
			}
		}
	}else
	{
		SipResponse *response = createResponse(489, request);
		sendResponse(response, SIP_REQUEST_SUBSCRIBE, transaction);
		//delete response;
	}

	return true;
}

SipDialog *UserAgentServer::createDialog(SipRequest *request, SipResponse *response)
{
	SipHeaderFrom *from = (SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) request->getHeader(SIP_HEADER_CALLID);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
	SipHeaderContact *contact = (SipHeaderContact *) request->getHeader(SIP_HEADER_CONTACT);
	SipHeaderTo *toRequest = (SipHeaderTo *) request->getHeader(SIP_HEADER_TO);
	SipHeaderTo *toResponse = (SipHeaderTo *) response->getHeader(SIP_HEADER_TO);

	if ((!from) || (!callId) || (!cseq) || (!toRequest) || (!toResponse) || (!contact))
		return 0;

	const char *id = callId->getString();
	const char *localTag = toResponse->getTag();
	const char *remoteTag = from->getTag(); //Pode ser NULL, compatibilidade com RFC 2543
	const char *localURI = toRequest->getAddress();
	const char *remoteURI = from->getAddress();
	const char *target = contact->getAddress();
	unsigned int sequenceNumber = cseq->getSequence();

	if ((!id) || (!localTag) || (!localURI) || (!remoteURI) || (!target))
		return 0;

	SipMessageType type = request->getMsgType();
	if (type == SIP_REQUEST_SUBSCRIBE)
	{
		SipHeaderEvent *event = (SipHeaderEvent *) request->getHeader(SIP_HEADER_EVENT);
		if (!event)
			return 0;

		SipEventPackage eventType = event->getType();
		const char *eventId = event->getId();

		if (eventType == SIP_EVENT_PACKAGE_INVALID)
			return 0;

		ua->subscription.setSubscription(eventType, eventId);
	}

	SipDialog *dialog = ua->addDialog(type);
	dialog->setDialog(id, localTag, remoteTag, 0, sequenceNumber, localURI, remoteURI, target);

	int recordRouteNum = response->getNumHeader(SIP_HEADER_RECORD_ROUTE);
	for (int i = 0; i < recordRouteNum; i++)
	{
		SipHeaderRoute *route = (SipHeaderRoute *) response->getHeader(SIP_HEADER_RECORD_ROUTE, i);
		dialog->addRouteBack(route);
	}

	if (type == SIP_REQUEST_SUBSCRIBE)
		SipManager::callback(SIP_SUBSCRIPTION_INITIATED, ua, remoteURI);
	else //if (type == SIP_REQUEST_INVITE)
		SipManager::callback(SIP_SESSION_INITIATED, ua, remoteURI);
	return dialog;
}

//-------------------------------------------

UserAgent::UserAgent(const char *uri) : uac(this), uas(this), link(this)
{
	this->uri = createString(uri);
	this->textReceived = 0;

	for (int i = 0; i < SIP_TIMER_COUNT; i++)
	{
		timerValues[i] = 0;
		timerHandlers[i] = 0;
		timerAlarms[i] = 0;
	}
}

UserAgent::~UserAgent()
{
	Simple_List<SipTransaction>::Iterator it = transactions.begin();
	while (it != transactions.end())
	{
		SipTransaction *transaction = it->object();
		transactions.remove(it++);
		delete transaction;
	}

	for (int i = 0; i < SIP_TIMER_COUNT; i++)
	{
		if (timerHandlers[i])
			delete timerHandlers[i];
		if (timerAlarms[i])
			delete timerAlarms[i];
	}
}

SipDialog *UserAgent::matchingDialog(SipMessage *msg, SipMessageType type)
{
	if ((type == SIP_REQUEST_ACK) || (type == SIP_REQUEST_BYE) || (type == SIP_REQUEST_INVITE))
		type = SIP_REQUEST_INVITE;
	else //if ((type == SIP_REQUEST_MESSAGE) || (type == SIP_REQUEST_NOTIFY) || (type == SIP_REQUEST_SUBSCRIBE))
		type = SIP_REQUEST_SUBSCRIBE;

	Simple_List<SipDialog>::Iterator it = dialogs.begin();
	while (it != dialogs.end())
	{
		SipDialog *dialog = it->object();
		it++;

		if ((dialog->type != type) || (!dialog->callID) || (!dialog->localTag)) //|| (!dialog->remoteTag))
			continue;

		SipHeaderFrom *headerFrom = (SipHeaderFrom *) msg->getHeader(SIP_HEADER_FROM);
		SipHeaderCallID *headerCallId = (SipHeaderCallID *) msg->getHeader(SIP_HEADER_CALLID);
		SipHeaderTo *headerTo = (SipHeaderTo *) msg->getHeader(SIP_HEADER_TO);

		if ((!headerFrom) || (!headerCallId) || (!headerTo))
			continue;

		const char *callId = headerCallId->getString();
		const char *localTag = 0; //No INVITE não possui ainda!
		const char *remoteTag = 0; //Pode ser NULL, compatibilidade com RFC 2543

		if (msg->getMsgType() == SIP_RESPONSE)
		{
			localTag = headerFrom->getTag();
			remoteTag = headerTo->getTag();
		}else
		{
			localTag = headerTo->getTag();
			remoteTag = headerFrom->getTag();
		}

		if ((!callId)) //|| (!localTag) || (!remoteTag))
			continue;

		bool ret1 = !strcmp(dialog->callID, callId);
		bool ret2 = (localTag) ? !strcmp(dialog->localTag, localTag) : true;
		bool ret3 = (remoteTag) ? !strcmp(dialog->remoteTag, remoteTag) : true;

		if ((localTag) && (remoteTag) && (!strcmp(localTag, remoteTag)) && ((ret2) || (ret3)))
		{
			ret2 = true;
			ret3 = true;
		}

		if ((ret1) && (ret2) && (ret3))
			return dialog;
	}

	return 0;
}

SipDialog *UserAgent::matchingDialog(const char *to, SipMessageType type)
{
	Simple_List<SipDialog>::Iterator it = dialogs.begin();
	while (it != dialogs.end())
	{
		SipDialog *dialog = it->object();
		it++;

		if ((dialog->type != type) || (!dialog->remoteURI))
			continue;

		if (!strcmp(to, dialog->remoteURI))
			return dialog;
	}
	return 0;
}

SipTransaction *UserAgent::matchingTransaction(SipMessage *msg)
{
	Simple_List<SipTransaction>::Iterator it = transactions.begin();
	while (it != transactions.end())
	{
		SipTransaction *transaction = it->object();
		it++;

		if (transaction->matchingTransaction(msg))
			return transaction;
	}
	return 0;
}

const char *UserAgent::getSubscriber()
{
	Simple_List<SipDialog>::Iterator it = dialogs.begin();
	while (it != dialogs.end())
	{
		SipDialog *dialog = it->object();
		it++;

		if (dialog->type != SIP_REQUEST_SUBSCRIBE)
			continue;

		return dialog->remoteURI;
	}
	return 0;
}

SipDialog *UserAgent::addDialog(SipMessageType type)
{
	SipDialog *dialog = new SipDialog(type);
	dialogs.insert(&dialog->link);
	return dialog;
}

void UserAgent::startTimer(SipTimer timer, SipTransaction *p)
{
	switch (timer)
	{
		case SIP_TIMER_A: timerHandlers[SIP_TIMER_A] = new Functor_Handler<SipTransaction>(&SipTransactionClientInvite::timerACallback, p);		break;
		case SIP_TIMER_B: timerHandlers[SIP_TIMER_B] = new Functor_Handler<SipTransaction>(&SipTransactionClientInvite::timerBCallback, p);		break;
		//case SIP_TIMER_C: timerHandlers[SIP_TIMER_C] = new Functor_Handler<SipTransaction>(&timerCCallback);									break;
		case SIP_TIMER_D: timerHandlers[SIP_TIMER_D] = new Functor_Handler<SipTransaction>(&SipTransactionClientInvite::timerDCallback, p);		break;
		case SIP_TIMER_E: timerHandlers[SIP_TIMER_E] = new Functor_Handler<SipTransaction>(&SipTransactionClientNonInvite::timerECallback, p);	break;
		case SIP_TIMER_F: timerHandlers[SIP_TIMER_F] = new Functor_Handler<SipTransaction>(&SipTransactionClientNonInvite::timerFCallback, p);	break;
		case SIP_TIMER_G: timerHandlers[SIP_TIMER_G] = new Functor_Handler<SipTransaction>(&SipTransactionServerInvite::timerGCallback, p);		break;
		case SIP_TIMER_H: timerHandlers[SIP_TIMER_H] = new Functor_Handler<SipTransaction>(&SipTransactionServerInvite::timerHCallback, p);		break;
		case SIP_TIMER_I: timerHandlers[SIP_TIMER_I] = new Functor_Handler<SipTransaction>(&SipTransactionServerInvite::timerICallback, p);		break;
		case SIP_TIMER_J: timerHandlers[SIP_TIMER_J] = new Functor_Handler<SipTransaction>(&SipTransactionServerNonInvite::timerJCallback, p);	break;
		case SIP_TIMER_K: timerHandlers[SIP_TIMER_K] = new Functor_Handler<SipTransaction>(&SipTransactionClientNonInvite::timerKCallback, p);	break;
		default: return;
	}

	timerAlarms[timer] = new Alarm(timerValues[timer], timerHandlers[timer], 1);
}

void UserAgent::stopTimer(SipTimer timer)
{
	if (timerHandlers[timer])
	{
		delete timerHandlers[timer];
		timerHandlers[timer] = 0;
	}

	if (timerAlarms[timer])
	{
		delete timerAlarms[timer];
		timerAlarms[timer] = 0;
	}
}

//-------------------------------------------

void SendRTP::sendData(const char *destination, unsigned short port, char *data, unsigned int size)
{
	data[3] = sequence;
	data[2] = sequence >> 8;

	data[7] = timestamp;
	data[6] = timestamp >> 8;
	data[5] = timestamp >> 16;
	data[4] = timestamp >> 24;

	//db<SendRTP>(INF) << "SendRTP::sendData -> Sending data to " << destination << ":" << port << " (size: " <<
	//					size << ", seq: " << sequence << ", timestamp: " << timestamp << ")\n";

	UDP::Address dst(IP::Address(destination), port);
	socket.set_remote(dst);

	if (socket.send(data, size) <= 0)
	{
		db<SendRTP>(WRN) << "SendRTP::sendData -> Failed to send data\n";
		//return;
	}

	sequence++;
	timestamp += 0xA0;
}

__END_SYS
