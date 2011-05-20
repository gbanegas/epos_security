#include <sip_transactions.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <sip_user_agent.h>

__BEGIN_SYS


SipTransaction::SipTransaction(UserAgent *ua) : ua(ua), link(this)
{
	this->transport = SipManager::getInstance()->getTransport();
}

void SipTransaction::clear()
{
	ua->removeTransaction(this);
}

//-------------------------------------------

SipTransactionClientInvite::SipTransactionClientInvite(UserAgent *ua) : SipTransaction(ua)
{
	this->curState = sttIdle;
	this->requestMsg = 0;
}

SipTransactionClientInvite::~SipTransactionClientInvite()
{
	ua->stopTimer(SIP_TIMER_A);
	ua->stopTimer(SIP_TIMER_B);
	ua->stopTimer(SIP_TIMER_D);

	if (requestMsg)
		delete requestMsg;
}

void SipTransactionClientInvite::sendAck(SipResponse *Msg)
{
	const char *requestUri = requestMsg->getRequestLine()->getRequestURI();
	SipHeaderVia *via = (SipHeaderVia *) requestMsg->getHeader(SIP_HEADER_VIA);
	SipHeaderFrom *from = (SipHeaderFrom *) requestMsg->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) requestMsg->getHeader(SIP_HEADER_CALLID);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) requestMsg->getHeader(SIP_HEADER_CSEQ);
	SipHeaderTo *to = (SipHeaderTo *) Msg->getHeader(SIP_HEADER_TO);

	if ((!requestUri) || (!via) || (!from) || (!callId) || (!cseq) || (!to))
		return;

	SipRequestAck Ack;

	Ack.setRequestLine(SIP_REQUEST_ACK, requestUri, SIP_VERSION);
	Ack.addHeader(new SipHeaderVia(*via));
	Ack.addHeader(new SipHeaderTo(*to));
	Ack.addHeader(new SipHeaderFrom(*from));
	Ack.addHeader(new SipHeaderCallID(*callId));

	SipHeaderMaxForwards *headerMaxForwards = new SipHeaderMaxForwards();
	headerMaxForwards->setNumber(70);
	Ack.addHeader(headerMaxForwards);

	SipHeaderCSeq *headerCSeq = new SipHeaderCSeq();
	headerCSeq->setCSeq(SIP_REQUEST_ACK, cseq->getSequence());
	Ack.addHeader(headerCSeq);

	int routeNum = requestMsg->getNumHeader(SIP_HEADER_ROUTE);
	for (int i = 0; i < routeNum; i++)
	{
		SipHeaderRoute *header = (SipHeaderRoute *) requestMsg->getHeader(SIP_HEADER_ROUTE, i);
		Ack.addHeader(new SipHeaderRoute(*header));
	}

	transport->sendMessage(&Ack);
}

void SipTransactionClientInvite::sendInvite(SipRequestInvite *Msg)
{
	switch (curState)
	{
		case sttIdle:
			curState = sttCalling;
			requestMsg = Msg; //requestMsg = new SipRequestInvite(*Msg);
			Msg->setCanDelete(false);
			transport->sendMessage(requestMsg);
			ua->setTimerValue(SIP_TIMER_A, SIP_TIMER_1);
			ua->startTimer(SIP_TIMER_A, this);
			ua->setTimerValue(SIP_TIMER_B, SIP_TIMER_1 * 64);
			ua->startTimer(SIP_TIMER_B, this);
			break;

		default:
			break;
	}
}

void SipTransactionClientInvite::receive1xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttCalling:
			curState = sttProceeding;
			ua->stopTimer(SIP_TIMER_A);
			ua->stopTimer(SIP_TIMER_B);
			ua->uac.receiveMsg(requestMsg, Msg, this);
			break;

		case sttProceeding:
			//curState = sttProceeding;
			ua->uac.receiveMsg(requestMsg, Msg, this);
			break;

		default:
			break;
	}
}

void SipTransactionClientInvite::receive2xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttCalling:
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_A);
			ua->stopTimer(SIP_TIMER_B);
			ua->uac.receiveMsg(requestMsg, Msg, this);
			clear();
			break;

		case sttProceeding:
			//curState = sttTerminated;
			ua->uac.receiveMsg(requestMsg, Msg, this);
			clear();
			break;

		default:
			break;
	}
}

void SipTransactionClientInvite::receive3xx6xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttCalling:
			curState = sttCompleted;
			sendAck(Msg);
			ua->setTimerValue(SIP_TIMER_D, SIP_TIMER_32s);
			ua->startTimer(SIP_TIMER_D, this);
			ua->stopTimer(SIP_TIMER_A);
			ua->stopTimer(SIP_TIMER_B);
			ua->uac.receiveMsg(requestMsg, Msg, this);
			break;

		case sttProceeding:
			curState = sttCompleted;
			sendAck(Msg);
			ua->setTimerValue(SIP_TIMER_D, SIP_TIMER_32s);
			ua->startTimer(SIP_TIMER_D, this);
			ua->uac.receiveMsg(requestMsg, Msg, this);
			break;

		case sttCompleted:
			//curState = sttCompleted;
			sendAck(Msg);
			break;

		default:
			break;
	}
}

void SipTransactionClientInvite::transportError()
{
	switch (curState)
	{
		case sttCalling:
		{
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_A);
			ua->stopTimer(SIP_TIMER_B);

			SipResponse response(503);
			ua->uac.receiveMsg(requestMsg, &response, this);
			clear();
			break;
		}

		case sttCompleted:
		{
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_D);

			SipResponse response(503);
			ua->uac.receiveMsg(requestMsg, &response, this);
			clear();
			break;
		}

		default:
			break;
	}
}

void SipTransactionClientInvite::timerAExpired()
{
	switch (curState)
	{
		case sttCalling:
			//curState = sttCalling;
			ua->setTimerValue(SIP_TIMER_A, ua->getTimerValue(SIP_TIMER_A) * 2);
			ua->startTimer(SIP_TIMER_A, this);
			transport->sendMessage(requestMsg);
			break;

		default:
			break;
	}
}

void SipTransactionClientInvite::timerBExpired()
{
	switch (curState)
	{
		case sttCalling:
		{
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_A);

			SipResponse response(408);
			ua->uac.receiveMsg(requestMsg, &response, this);
			clear();
			break;
		}

		default:
			break;
	}
}

void SipTransactionClientInvite::timerDExpired()
{
	switch (curState)
	{
		case sttCompleted:
			//curState = sttTerminated;
			clear();
			break;

		default:
			break;
	}
}

SipTransaction *SipTransactionClientInvite::matchingTransaction(SipMessage *msg)
{
	if ((!msg) || (msg->getMsgType() != SIP_RESPONSE))
		return 0;

	SipResponse *response = (SipResponse *) msg;

	SipHeaderVia *viaResponse = (SipHeaderVia *) response->getHeader(SIP_HEADER_VIA);
	SipHeaderCSeq *cseqResponse = (SipHeaderCSeq *) response->getHeader(SIP_HEADER_CSEQ);
	if ((!viaResponse) || (!cseqResponse))
		return 0;

	const char *branchResponse = viaResponse->getBranch();
	SipMessageType msgTypeResponse = cseqResponse->getMethod();
	//unsigned int sequenceResponse = cseqResponse->getSequence();

	if (!branchResponse)
		return 0;

	if (requestMsg)
	{
		SipHeaderVia *via = (SipHeaderVia *) requestMsg->getHeader(SIP_HEADER_VIA);
		SipHeaderCSeq *cseq = (SipHeaderCSeq *) requestMsg->getHeader(SIP_HEADER_CSEQ);

		if ((via) && (cseq))
		{
			const char *branch = via->getBranch();
			SipMessageType msgType = cseq->getMethod();
			//unsigned int sequence = cseq->getSequence();

			if ((!strcmp(branch, branchResponse)) && (msgType == msgTypeResponse)) // && (sequence == sequenceResponse))
				return this;
		}
	}

	return 0;
}

//-------------------------------------------

SipTransactionClientNonInvite::SipTransactionClientNonInvite(UserAgent *ua) : SipTransaction(ua)
{
	this->curState = sttIdle;
	this->requestMsg = 0;
}

SipTransactionClientNonInvite::~SipTransactionClientNonInvite()
{
	ua->stopTimer(SIP_TIMER_E);
	ua->stopTimer(SIP_TIMER_F);
	ua->stopTimer(SIP_TIMER_K);

	if (requestMsg)
		delete requestMsg;
}

/*void SipTransactionClientNonInvite::copyRequest(SipRequest *Msg)
{
	switch (Msg->getMsgType())
	{
		case SIP_REQUEST_BYE:			requestMsg = new SipRequestBye(*((SipRequestBye *) Msg));				break;
		//case SIP_REQUEST_CANCEL:		requestMsg = new SipRequestCancel(*((SipRequestCancel *) Msg));			break;
		case SIP_REQUEST_MESSAGE:		requestMsg = new SipRequestMessage(*((SipRequestMessage *) Msg));		break;
		case SIP_REQUEST_NOTIFY:		requestMsg = new SipRequestNotify(*((SipRequestNotify *) Msg));			break;
		//case SIP_REQUEST_OPTIONS:		requestMsg = new SipRequestOptions(*((SipRequestOptions *) Msg));		break;
		//case SIP_REQUEST_REGISTER:	requestMsg = new SipRequestRegister(*((SipRequestRegister *) Msg));		break;
		case SIP_REQUEST_SUBSCRIBE:		requestMsg = new SipRequestSubscribe(*((SipRequestSubscribe *) Msg));	break;
		default:						break;
	}
}*/

void SipTransactionClientNonInvite::sendRequest(SipRequest *Msg)
{
	switch (curState)
	{
		case sttIdle:
			curState = sttTrying;
			requestMsg = Msg; //copyRequest(Msg);
			Msg->setCanDelete(false);
			transport->sendMessage(requestMsg);
			ua->setTimerValue(SIP_TIMER_E, SIP_TIMER_1);
			ua->startTimer(SIP_TIMER_E, this);
			ua->setTimerValue(SIP_TIMER_F, SIP_TIMER_1 * 64);
			ua->startTimer(SIP_TIMER_F, this);
			break;

		default:
			break;
	}
}

void SipTransactionClientNonInvite::receive1xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttTrying:
		case sttProceeding:
			curState = sttProceeding;
			ua->uac.receiveMsg(requestMsg, Msg, this);
			break;

		default:
			break;
	}
}

void SipTransactionClientNonInvite::receive2xx6xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttTrying:
		case sttProceeding:
			curState = sttCompleted;
			ua->setTimerValue(SIP_TIMER_K, SIP_TIMER_4);
			ua->startTimer(SIP_TIMER_K, this);
			ua->stopTimer(SIP_TIMER_E);
			ua->stopTimer(SIP_TIMER_F);
			ua->uac.receiveMsg(requestMsg, Msg, this);
			break;

		default:
			break;
	}
}

void SipTransactionClientNonInvite::transportError()
{
	switch (curState)
	{
		case sttTrying:
		case sttProceeding:
		{
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_E);
			ua->stopTimer(SIP_TIMER_F);

			SipResponse response(503);
			ua->uac.receiveMsg(requestMsg, &response, this);
			clear();
			break;
		}

		default:
			break;
	}
}

void SipTransactionClientNonInvite::timerEExpired()
{
	switch (curState)
	{
		case sttTrying:
		{
			//curState = sttTrying;
			int timerValue = ((ua->getTimerValue(SIP_TIMER_E) * 2) < SIP_TIMER_2) ? ua->getTimerValue(SIP_TIMER_E) * 2 : SIP_TIMER_2;
			ua->setTimerValue(SIP_TIMER_E, timerValue);
			ua->startTimer(SIP_TIMER_E, this);
			transport->sendMessage(requestMsg);
			break;
		}

		case sttProceeding:
			//curState = sttProceeding;
			ua->setTimerValue(SIP_TIMER_E, SIP_TIMER_2);
			ua->startTimer(SIP_TIMER_E, this);
			transport->sendMessage(requestMsg);
			break;

		default:
			break;
	}
}

void SipTransactionClientNonInvite::timerFExpired()
{
	switch (curState)
	{
		case sttTrying:
		case sttProceeding:
		{
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_E);

			SipResponse response(408);
			ua->uac.receiveMsg(requestMsg, &response, this);
			clear();
			break;
		}

		default:
			break;
	}
}

void SipTransactionClientNonInvite::timerKExpired()
{
	switch (curState)
	{
		case sttCompleted:
			//curState = sttTerminated;
			clear();
			break;

		default:
			break;
	}
}

SipTransaction *SipTransactionClientNonInvite::matchingTransaction(SipMessage *msg)
{
	if ((!msg) || (msg->getMsgType() != SIP_RESPONSE))
		return 0;

	SipResponse *response = (SipResponse *) msg;

	SipHeaderVia *viaResponse = (SipHeaderVia *) response->getHeader(SIP_HEADER_VIA);
	SipHeaderCSeq *cseqResponse = (SipHeaderCSeq *) response->getHeader(SIP_HEADER_CSEQ);
	if ((!viaResponse) || (!cseqResponse))
		return 0;

	const char *branchResponse = viaResponse->getBranch();
	SipMessageType msgTypeResponse = cseqResponse->getMethod();
	//unsigned int sequenceResponse = cseqResponse->getSequence();

	if (!branchResponse)
		return 0;

	if (requestMsg)
	{
		SipHeaderVia *via = (SipHeaderVia *) requestMsg->getHeader(SIP_HEADER_VIA);
		SipHeaderCSeq *cseq = (SipHeaderCSeq *) requestMsg->getHeader(SIP_HEADER_CSEQ);

		if ((via) && (cseq))
		{
			const char *branch = via->getBranch();
			SipMessageType msgType = cseq->getMethod();
			//unsigned int sequence = cseq->getSequence();

			if ((!strcmp(branch, branchResponse)) && (msgType == msgTypeResponse)) // && (sequence == sequenceResponse))
				return this;
		}
	}

	return 0;
}

//-------------------------------------------

SipTransactionServerInvite::SipTransactionServerInvite(UserAgent *ua) : SipTransaction(ua)
{
	this->curState = sttIdle;
	this->requestMsg = 0;
	this->lastResponse = 0;
}

SipTransactionServerInvite::~SipTransactionServerInvite()
{
	ua->stopTimer(SIP_TIMER_G);
	ua->stopTimer(SIP_TIMER_H);
	ua->stopTimer(SIP_TIMER_I);

	if (requestMsg)
		delete requestMsg;

	if (lastResponse)
		delete lastResponse;
}

void SipTransactionServerInvite::receiveInvite(SipRequestInvite *Msg)
{
	switch (curState)
	{
		case sttIdle:
			curState = sttProceeding;
			requestMsg = Msg; //requestMsg = new SipRequestInvite(*Msg);
			Msg->setCanDelete(false);
			ua->uas.receiveMsg(Msg, this);
			break;

		case sttProceeding:
			//curState = sttProceeding;
			if (lastResponse)
				transport->sendMessage(lastResponse);
			break;

		case sttCompleted:
			//curState = sttCompleted;
			if (lastResponse)
				transport->sendMessage(lastResponse);
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::receiveAck(SipRequestAck *Msg)
{
	switch (curState)
	{
		case sttCompleted:
			curState = sttConfirmed;
			ua->setTimerValue(SIP_TIMER_I, SIP_TIMER_4);
			ua->startTimer(SIP_TIMER_I, this);
			ua->stopTimer(SIP_TIMER_G);
			ua->stopTimer(SIP_TIMER_H);
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::send1xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttProceeding:
			//curState = sttProceeding;
			if (lastResponse)
				delete lastResponse;
			lastResponse = Msg; //lastResponse = new SipResponse(*Msg);
			Msg->setCanDelete(false);
			transport->sendMessage(lastResponse);
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::send2xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttProceeding:
			//curState = sttTerminated;
			transport->sendMessage(Msg);
			clear();
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::send3xx6xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttProceeding:
			curState = sttCompleted;
			if (lastResponse)
				delete lastResponse;
			lastResponse = Msg; //lastResponse = new SipResponse(*Msg);
			Msg->setCanDelete(false);
			transport->sendMessage(lastResponse);
			ua->setTimerValue(SIP_TIMER_G, SIP_TIMER_1);
			ua->startTimer(SIP_TIMER_G, this);
			ua->setTimerValue(SIP_TIMER_H, SIP_TIMER_1 * 64);
			ua->startTimer(SIP_TIMER_H, this);
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::transportError()
{
	switch (curState)
	{
		case sttProceeding:
			//curState = sttTerminated;
			//ua->uas.receiveMsg(); //TODO: InformTU("Transport Error");
			clear();
			break;

		case sttCompleted:
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_G);
			ua->stopTimer(SIP_TIMER_H);
			//ua->uas.receiveMsg(); //TODO: InformTU("Transport Error");
			clear();
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::timerGExpired()
{
	switch (curState)
	{
		case sttCompleted:
		{
			//curState = sttCompleted;
			int timerValue = ((ua->getTimerValue(SIP_TIMER_G) * 2) < SIP_TIMER_2) ? ua->getTimerValue(SIP_TIMER_G) * 2 : SIP_TIMER_2;
			ua->setTimerValue(SIP_TIMER_G, timerValue);
			ua->startTimer(SIP_TIMER_G, this);
			if (lastResponse)
				transport->sendMessage(lastResponse);
			break;
		}

		default:
			break;
	}
}

void SipTransactionServerInvite::timerHExpired()
{
	switch (curState)
	{
		case sttCompleted:
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_G);
			//ua->uas.receiveMsg(); //TODO: InformTU("Timeout occurred - TimerH");
			clear();
			break;

		default:
			break;
	}
}

void SipTransactionServerInvite::timerIExpired()
{
	switch (curState)
	{
		case sttConfirmed:
			//curState = sttTerminated;
			clear();
			break;

		default:
			break;
	}
}

SipTransaction *SipTransactionServerInvite::matchingTransaction(SipMessage *msg)
{
	if ((!msg) || ((msg->getMsgType() != SIP_REQUEST_INVITE) && (msg->getMsgType() != SIP_REQUEST_ACK)))
		return 0;

	SipRequest *request = (SipRequest *) msg;

	SipHeaderVia *viaRequest = (SipHeaderVia *) request->getHeader(SIP_HEADER_VIA);
	if (!viaRequest)
		return 0;

	SipMessageType methodRequest = request->getRequestLine()->getMethod();
	const char *branchRequest = viaRequest->getBranch();
	const char *sentbyRequest = viaRequest->getSentBy();

	if ((!sentbyRequest) || (methodRequest == SIP_MESSAGE_TYPE_INVALID))
		return 0;

	bool magicCookie = false;
	if ((branchRequest) && (startWith(branchRequest, "z9hG4bK")))
		magicCookie = true;

	if (magicCookie)
	{
		SipHeaderVia *via = (SipHeaderVia *) requestMsg->getHeader(SIP_HEADER_VIA);
		if (via)
		{
			SipMessageType method = requestMsg->getRequestLine()->getMethod();
			const char *branch = via->getBranch();
			const char *sentby = via->getSentBy();

			if ((branch) && (sentby) && (method != SIP_MESSAGE_TYPE_INVALID))
			{
				if ((!strcmp(branch, branchRequest)) && (!strcmp(sentby, sentbyRequest)) &&
					((method == methodRequest) || ((methodRequest == SIP_REQUEST_ACK) && (method == SIP_REQUEST_INVITE))))
				{
					return this;
				}
			}
		}
	}

	return 0;
}

//-------------------------------------------

SipTransactionServerNonInvite::SipTransactionServerNonInvite(UserAgent *ua) : SipTransaction(ua)
{
	this->curState = sttIdle;
	this->requestMsg = 0;
	this->lastResponse = 0;
}

SipTransactionServerNonInvite::~SipTransactionServerNonInvite()
{
	ua->stopTimer(SIP_TIMER_J);

	if (requestMsg)
		delete requestMsg;

	if (lastResponse)
		delete lastResponse;
}

/*void SipTransactionServerNonInvite::copyRequest(SipRequest *Msg)
{
	switch (Msg->getMsgType())
	{
		case SIP_REQUEST_BYE:			requestMsg = new SipRequestBye(*((SipRequestBye *) Msg));				break;
		//case SIP_REQUEST_CANCEL:		requestMsg = new SipRequestCancel(*((SipRequestCancel *) Msg));			break;
		case SIP_REQUEST_MESSAGE:		requestMsg = new SipRequestMessage(*((SipRequestMessage *) Msg));		break;
		case SIP_REQUEST_NOTIFY:		requestMsg = new SipRequestNotify(*((SipRequestNotify *) Msg));			break;
		//case SIP_REQUEST_OPTIONS:		requestMsg = new SipRequestOptions(*((SipRequestOptions *) Msg));		break;
		//case SIP_REQUEST_REGISTER:	requestMsg = new SipRequestRegister(*((SipRequestRegister *) Msg));		break;
		case SIP_REQUEST_SUBSCRIBE:		requestMsg = new SipRequestSubscribe(*((SipRequestSubscribe *) Msg));	break;
		default:						break;
	}
}*/

void SipTransactionServerNonInvite::receiveRequest(SipRequest *Msg)
{
	switch (curState)
	{
		case sttIdle:
			curState = sttTrying;
			requestMsg = Msg; //copyRequest(Msg);
			Msg->setCanDelete(false);
			ua->uas.receiveMsg(Msg, this);
			break;

		case sttProceeding:
			//curState = sttProceeding;
			if (lastResponse)
				transport->sendMessage(lastResponse);
			break;

		case sttCompleted:
			//curState = sttCompleted;
			if (lastResponse)
				transport->sendMessage(lastResponse);
			break;

		default:
			break;
	}
}

void SipTransactionServerNonInvite::send1xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttTrying:
		case sttProceeding:
			curState = sttProceeding;
			if (lastResponse)
				delete lastResponse;
			lastResponse = Msg; //lastResponse = new SipResponse(*Msg);
			Msg->setCanDelete(false);
			transport->sendMessage(lastResponse);
			break;

		default:
			break;
	}
}

void SipTransactionServerNonInvite::send2xx6xx(SipResponse *Msg)
{
	switch (curState)
	{
		case sttTrying:
		case sttProceeding:
			curState = sttCompleted;
			if (lastResponse)
				delete lastResponse;
			lastResponse = Msg; //lastResponse = new SipResponse(*Msg);
			Msg->setCanDelete(false);
			transport->sendMessage(lastResponse);
			ua->setTimerValue(SIP_TIMER_J, SIP_TIMER_1 * 64);
			ua->startTimer(SIP_TIMER_J, this);
			break;

		default:
			break;
	}
}

void SipTransactionServerNonInvite::transportError()
{
	switch (curState)
	{
		case sttProceeding:
			//curState = sttTerminated;
			//ua->uas.receiveMsg(); //TODO: InformTU("Transport Error");
			clear();
			break;

		case sttCompleted:
			//curState = sttTerminated;
			ua->stopTimer(SIP_TIMER_J);
			//ua->uas.receiveMsg(); //TODO: InformTU("Transport Error");
			clear();
			break;

		default:
			break;
	}
}

void SipTransactionServerNonInvite::timerJExpired()
{
	switch (curState)
	{
		case sttCompleted:
			//curState = sttTerminated;
			clear();
			break;

		default:
			break;
	}
}

SipTransaction *SipTransactionServerNonInvite::matchingTransaction(SipMessage *msg)
{
	if ((!msg) || (msg->getMsgType() == SIP_RESPONSE) || (msg->getMsgType() == SIP_REQUEST_INVITE) ||
		(msg->getMsgType() == SIP_REQUEST_ACK))
	{
		return 0;
	}

	SipRequest *request = (SipRequest *) msg;

	SipHeaderVia *viaRequest = (SipHeaderVia *) request->getHeader(SIP_HEADER_VIA);
	if (!viaRequest)
		return 0;

	SipMessageType methodRequest = request->getRequestLine()->getMethod();
	const char *branchRequest = viaRequest->getBranch();
	const char *sentbyRequest = viaRequest->getSentBy();

	if ((!sentbyRequest) || (methodRequest == SIP_MESSAGE_TYPE_INVALID))
		return 0;

	bool magicCookie = false;
	if ((branchRequest) && (startWith(branchRequest, "z9hG4bK")))
		magicCookie = true;

	if (magicCookie)
	{
		SipHeaderVia *via = (SipHeaderVia *) requestMsg->getHeader(SIP_HEADER_VIA);
		if (via)
		{
			SipMessageType method = requestMsg->getRequestLine()->getMethod();
			const char *branch = via->getBranch();
			const char *sentby = via->getSentBy();

			if ((branch) && (sentby) && (method != SIP_MESSAGE_TYPE_INVALID))
			{
				if ((!strcmp(branch, branchRequest)) && (!strcmp(sentby, sentbyRequest)) &&
					((method == methodRequest) || ((methodRequest == SIP_REQUEST_ACK) && (method == SIP_REQUEST_INVITE))))
				{
					return this;
				}
			}
		}
	}

	return 0;
}

__END_SYS
