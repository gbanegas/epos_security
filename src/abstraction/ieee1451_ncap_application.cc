#include <ieee1451_ncap_application.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS


NCAPApplication *NCAPApplication::app = 0;

NCAPApplication::NCAPApplication()
{
	IEEE1451dot0_NCAP::getInstance()->setApplication(this);

	SipManager::getInstance()->init();
	SipManager::registerUserHandler(messageCallback);
}

NCAPApplication::~NCAPApplication()
{
	Simple_List<TIMCache>::Iterator it1 = cache.begin();
	while (it1 != cache.end())
	{
		Simple_List<TIMCache>::Element *el = it1++;
		TIMCache *timCache = el->object();
		cache.remove(&timCache->link);
		delete timCache;
	}

	Simple_List<TEDSRetriever>::Iterator it2 = retrievers.begin();
	while (it2 != retrievers.end())
	{
		Simple_List<TEDSRetriever>::Element *el = it2++;
		TEDSRetriever *retriever = el->object();
		retrievers.remove(&retriever->link);
		delete retriever->tim; //Este tim não foi pra cache ainda!
		delete retriever;
	}
}

NCAPApplication *NCAPApplication::getInstance()
{
	if (!app)
		app = new NCAPApplication();
	return app;
}

NCAPApplication::TIMCache *NCAPApplication::getTIMCache(const IP::Address &address)
{
	Simple_List<TIMCache>::Iterator it = cache.begin();
	while (it != cache.end())
	{
		TIMCache *timCache = it->object();
		it++;

		if (timCache->tim->address == address)
			return timCache;
	}

	return 0;
}

NCAPApplication::TIMCache *NCAPApplication::getTIMCache(const char *uri)
{
	Simple_List<TIMCache>::Iterator it = cache.begin();
	while (it != cache.end())
	{
		TIMCache *timCache = it->object();
		it++;

		if (!strcmp(timCache->ua->getUri(), uri))
			return timCache;
	}

	return 0;
}

TEDSRetriever *NCAPApplication::getRetriever(unsigned short transId)
{
	Simple_List<TEDSRetriever>::Iterator it = retrievers.begin();
	while (it != retrievers.end())
	{
		TEDSRetriever *retriever = it->object();
		it++;

		if (retriever->lastTransId == transId)
			return retriever;
	}

	return 0;
}

void NCAPApplication::updateTIM(const IP::Address &address)
{
	TEDSRetriever *retriever = new TEDSRetriever(address, this);
	retrievers.insert(&retriever->link);
	retriever->execute();
}

void NCAPApplication::updateTIMCompleted(TEDSRetriever *retriever, IEEE1451TIMChannel *tim, IP::Address address)
{
	char uri[100], remote[20], local[20];
	address.to_string(remote);
	((IP::Address &) IP::instance(0)->address()).to_string(local);
	strcpy(uri, "sip:");
	strcat(uri, remote);
	strcat(uri, "@");
	strcat(uri, local);

	TIMCache *timCache = new TIMCache(tim, SipManager::getInstance()->createUserAgent(uri));
	cache.insert(&timCache->link);

	retrievers.remove(&retriever->link);
	delete retriever;

	reportTimConnected(address);

	//new Thread(&NCAPApplication::readDataSetThread, this, address, (IEEE1451TIMChannel *) tim);
}

void NCAPApplication::reportTimConnected(const IP::Address &address)
{
	TIMCache *timCache = getTIMCache(address);

	if (!timCache)
		updateTIM(address);
	else
	{
		db<NCAPApplication>(INF) << "++ TIM CONNECTED (address=" << address << ") ++\n";
		timCache->tim->connect();

		if (timCache->ua->hasSubscription())
			sendSipNotify(timCache->ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_OPEN);

		IEEE1451TransducerChannel *transducer = timCache->tim->getTransducer();
		sendOperate(address, transducer->getChannelNumber());
	}
}

void NCAPApplication::reportTimDisconnected(const IP::Address &address)
{
	TIMCache *timCache = getTIMCache(address);
	if (timCache)
	{
		timCache->tim->disconnect();

		if (timCache->ua->hasSubscription())
			sendSipNotify(timCache->ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_CLOSED);

		db<NCAPApplication>(INF) << "++ TIM DISCONNECTED (address=" << address << ") ++\n";
	}
}

void NCAPApplication::reportError(unsigned short transId, int errorCode)
{
	TEDSRetriever *retriever = getRetriever(transId);

	if (retriever)
		retriever->repeat();
	//else
	//	db<NCAPApplication>(INF) << "++ ERROR (" << transId << "): " << errorCode << " ++\n";
}

void NCAPApplication::reportCommandReply(const IP::Address &address, unsigned short transId, const char *message, unsigned int length)
{
	TEDSRetriever *retriever = getRetriever(transId);

	if (retriever)
	{
		TEDSReadReply *reply = (TEDSReadReply *) message;
		const char *buffer = message + sizeof(TEDSReadReply);

		if ((!reply->header.success) || (reply->header.length < sizeof(reply->offset)))
		{
			db<NCAPApplication>(INF) << "Error in reportCommandReply (teds)\n";
			//reportError(transId, 0);
			return;
		}

		db<NCAPApplication>(INF) << "++ REPLY RECEIVED (teds) (transId=" << transId << ", address=" << address << ", length=" << reply->header.length << ") ++\n";

		retriever->process(buffer, length - sizeof(TEDSReadReply));
	}else
	{
		DataSetReadReply *reply = (DataSetReadReply *) message;
		const char *buffer = message + sizeof(DataSetReadReply);

		if ((!reply->header.success) || (reply->header.length < sizeof(reply->offset)))
		{
			db<NCAPApplication>(INF) << "Error in reportCommandReply (data set)\n";
			//reportError(transId, 0);
			return;
		}

		db<NCAPApplication>(INF) << "++ REPLY RECEIVED (data set) (transId=" << transId << ", address=" << address << ", length=" << reply->header.length << ") ++\n";

		readTemperature(address, buffer);
	}
}

void NCAPApplication::reportTimInitiatedMessage(const IP::Address &address, const char *message, unsigned int length)
{
	Command *cmd = (Command *) message;
	const char *buffer = message + sizeof(Command);

	db<NCAPApplication>(INF) << "++ TIM MESSAGE RECEIVED (address=" << address << ") ++\n";

	if ((cmd->channelNumber == 1) && (cmd->command == COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT))
		readTemperature(address, buffer);
}

void NCAPApplication::readTemperature(const IP::Address &address, const char *buffer)
{
	TIMCache *timCache = getTIMCache(address);
	if (!timCache)
		return;

	IEEE1451TransducerChannel *transducer = timCache->tim->getTransducer();
	TEDS_NCAP *channel = transducer->getTEDS(0x03);

	TLV *sample = channel->getTLV(0x12);
	TEDS_NCAP auxSample(0xff, sample->value, sample->length, true);

	TLV *dataModel = auxSample.getTLV(0x28);
	unsigned short model = dataModel->value[0];

	if (model == 1)
	{
		TLV *dataSet = channel->getTLV(0x13);
		TEDS_NCAP auxDataSet(0xff, dataSet->value, dataSet->length, true);

		TLV *maxDataRepetitions = auxDataSet.getTLV(0x2b);
		unsigned short repeats = (maxDataRepetitions->value[0] << 8) | maxDataRepetitions->value[1];

		char data[50]; data[0] = 0;
		char aux[11];

		for (unsigned short i = 0, j = 0; i < repeats; i++, j += 4)
		{
			float value = *((float *)(&buffer[j]));

			itoa((int) value, aux);
			strcat(data, aux);
			if (i != (repeats - 1))
				strcat(data, ", ");
		}

		db<NCAPApplication>(INF) << "Read temperature: " << data << "\n";

		if (timCache->ua->hasSubscription())
			sendSipMessage(timCache->ua, data);
	}
}

unsigned short NCAPApplication::sendOperate(const IP::Address &address, unsigned short channelNumber)
{
	char *cmd = IEEE1451dot0_NCAP::getInstance()->createCommand(channelNumber, COMMAND_CLASS_TRANSDUCER_CHANNEL_OPERATE);
	unsigned short transId = IEEE1451dot0_NCAP::getInstance()->sendCommand(address, cmd, sizeof(Command));
	delete[] cmd;
	return transId;
}

unsigned short NCAPApplication::sendIdle(const IP::Address &address, unsigned short channelNumber)
{
	char *cmd = IEEE1451dot0_NCAP::getInstance()->createCommand(channelNumber, COMMAND_CLASS_TRANSDUCER_CHANNEL_IDLE);
	unsigned short transId = IEEE1451dot0_NCAP::getInstance()->sendCommand(address, cmd, sizeof(Command));
	delete[] cmd;
	return transId;
}

unsigned short NCAPApplication::sendReadTEDS(const IP::Address &address, unsigned short channelNumber, char tedsId)
{
	char *cmd = IEEE1451dot0_NCAP::getInstance()->createCommand(channelNumber, COMMAND_CLASS_READ_TEDS_SEGMENT, &tedsId, 1);
	unsigned short transId = IEEE1451dot0_NCAP::getInstance()->sendCommand(address, cmd, sizeof(Command) + 1);
	delete[] cmd;
	return transId;
}

unsigned short NCAPApplication::sendReadDataSet(const IP::Address &address, unsigned short channelNumber)
{
	unsigned int offset = 0;
	char *cmd = IEEE1451dot0_NCAP::getInstance()->createCommand(channelNumber, COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT, (char *) &offset, sizeof(offset));
	unsigned short transId = IEEE1451dot0_NCAP::getInstance()->sendCommand(address, cmd, sizeof(Command) + sizeof(offset));
	delete[] cmd;
	return transId;
}

void NCAPApplication::sendSipMessage(UserAgent *ua, const char *data)
{
	SipRequestMessage *message = ua->getUserAgentClient()->createMessage(ua->getSubscriber(), data);
	if (!message)
		return;

	ua->getUserAgentClient()->sendRequest(message);
	//delete message;
}

void NCAPApplication::sendSipNotify(UserAgent *ua, SipSubscriptionState state, SipPidfXmlBasicElement pidfXml)
{
	SipRequestNotify *notify = ua->getUserAgentClient()->createNotify(ua->getSubscriber(), state, pidfXml, 3600);
	if (!notify)
		return;

	ua->getUserAgentClient()->sendRequest(notify);
	//delete notify;
}

/*int NCAPApplication::readDataSetThread(NCAPApplication *ncap, IP::Address address, IEEE1451TIMChannel *tim)
{
	db<NCAPApplication>(INF) << "== NCAP request data thread created (address=" << address << ") ==\n";

	while (1)
	{
		Alarm::delay(2200000);
		if (tim->isConnected())
		{
			db<NCAPApplication>(INF) << "-- Reading DataSet (address=" << address << ")...\n";
			ncap->sendReadDataSet(address, 0x01);
		}else
			db<NCAPApplication>(INF) << "-- Address " << address << " is disconnected.\n";
	}
	return 0;
}*/

int NCAPApplication::messageCallback(SipEventCallback event, UserAgent *ua, const char *remote)
{
	NCAPApplication *app = NCAPApplication::getInstance();
	TIMCache *timCache = app->getTIMCache(ua->getUri());
	if (!timCache)
	{
		db<NCAPApplication>(INF) << "+++++ Invalid User Agent +++++\n";
		return 1;
	}

	switch (event)
	{
		case SIP_SESSION_INITIATED: //TODO
			db<NCAPApplication>(INF) << "+++++ Session Initiated +++++\n";
			break;

		case SIP_SESSION_TERMINATED: //TODO
			db<NCAPApplication>(INF) << "+++++ Session Terminated +++++\n";
			break;

		case SIP_SUBSCRIPTION_INITIATED:
		{
			db<NCAPApplication>(INF) << "+++++ Subscription Initiated +++++\n";

			if (timCache->tim->isConnected())
				app->sendSipNotify(ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_OPEN);
			else
				app->sendSipNotify(ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_CLOSED);
			break;
		}

		case SIP_SUBSCRIPTION_TERMINATED:
			db<NCAPApplication>(INF) << "+++++ Subscription Terminated +++++\n";
			break;

		case SIP_MESSAGE_RECEIVED:
		{
			db<NCAPApplication>(INF) << "+++++ Message Received +++++\n";

			if (!strncmp(ua->getTextReceived(), "request data", 12))
				app->sendReadDataSet(timCache->tim->address, 0x01);
			break;
		}

		default:
			break;
	}

	return 0;
}

//-------------------------------------------

TEDSRetriever::TEDSRetriever(const IP::Address &address, NCAPApplication *app)
	: address(address), app(app), link(this)
{
	this->transducer = new IEEE1451TransducerChannel(address, 0x01); //Só suporta um transdutor por mote!
	this->tim = new IEEE1451TIMChannel(address, transducer);
	this->state = meta_teds;
	this->lastTransId = 0;
}

void TEDSRetriever::process(const char *message, unsigned int length)
{
	TEDS_NCAP *teds = new TEDS_NCAP(tedsId, message, length);

	switch (state)
	{
		case meta_teds:
		case tim_transducer_name_teds:
		case phy_teds:
			tim->addTEDS(teds);
			break;

		case transducer_channel_teds:
		case transducer_name_teds:
			transducer->addTEDS(teds);
			break;

		default:
			delete teds;
			break;
	}

	state++;
	execute();
}

void TEDSRetriever::execute()
{
	unsigned short channelNumber = 0xff;

	switch (state)
	{
		case meta_teds: //Ler meta teds
			db<TEDSRetriever>(INF) << ">> Getting meta teds...\n";
			tedsId = 0x01;
			channelNumber = 0x00;
			break;

		case tim_transducer_name_teds: //Ler tim user transducer name teds
			db<TEDSRetriever>(INF) << ">> Getting tim's transducer name teds...\n";
			tedsId = 0x0c;
			channelNumber = 0x00;
			break;

		case phy_teds: //Ler phy teds
			db<TEDSRetriever>(INF) << ">> Getting phy teds...\n";
			tedsId = 0x0d;
			channelNumber = 0x00;
			break;

		case transducer_channel_teds: //Ler todos os transducer channel teds
			db<TEDSRetriever>(INF) << ">> Getting transducer channel teds...\n";
			tedsId = 0x03;
			channelNumber = 0x01;
			break;

		case transducer_name_teds: //Ler transducer user transducer name teds
			db<TEDSRetriever>(INF) << ">> Getting transducer's transducer name teds...\n";
			tedsId = 0x0c;
			channelNumber = 0x01;
			break;

		default:
			db<TEDSRetriever>(INF) << ">> TEDS completed...\n";
			tedsId = 0;
			app->updateTIMCompleted(this, tim, address);
			return;
	}

	if (tedsId > 0)
		lastTransId = app->sendReadTEDS(address, channelNumber, tedsId);
}

__END_SYS
