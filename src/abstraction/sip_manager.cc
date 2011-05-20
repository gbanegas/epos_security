#include <sip_manager.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <sip_user_agent.h>
#include <utility/random.h>

__BEGIN_SYS


bool SipTransportLayer::init()
{
	hostPort = SIP_PORT;

	char local[20];
	((IP::Address &) IP::instance(0)->address()).to_string(local);
	hostIP = createString(local);
	if (!hostIP)
    {
		db<SipTransportLayer>(WRN) << "SipTransportLayer::init -> Failed to get the host ip\n";
		return false;
	}

	return true;
}

int SipTransportLayer::sendMessage(SipMessage *message)
{
	char *destination = 0;
	char auxDestination[512];
	int port = 0;

	SipHeaderVia *via = (SipHeaderVia *) message->getHeader(SIP_HEADER_VIA);
	if (!via)
		return -1;

	if (message->getMsgType() == SIP_RESPONSE)
	{
		destination = (char *) via->getReceived();
		if (!destination)
			destination = (char *) via->getSentBy();
		port = via->getPort();
	}else
	{
		//headerVia->setSentBy(0, hostIP);
		//headerVia->setPort(0, hostPort);
		//headerVia->setTransport(0, SIP_TRANSPORT_UDP);

		const char *requestURI = ((SipRequest *) message)->getRequestLine()->getRequestURI();

		SipHeaderRoute *route = (SipHeaderRoute *) message->getHeader(SIP_HEADER_ROUTE);
		const char *dest = 0;

		if ((route) && (route->isLR()))
			dest = route->getAddress();
		else //if ((!route) || (!route->isLR()) /*Strict Route*/ )
			dest = requestURI;

		char aux[255];
		strcpy(auxDestination, dest);
		match(auxDestination, ":" , aux);
		skip(auxDestination, " \t");
		match(auxDestination, "@" , aux);
		skip(auxDestination, " \t");
		destination = auxDestination;
		port = 5060;
	}

	if ((!destination) || (port == 0))
		return -1;

	sendBuffer[0] = 0;
	message->encode(sendBuffer);
	int length = strlen(sendBuffer);

	return sendData(destination, port, sendBuffer, length);
}

int SipTransportLayer::sendData(const char *destination, int port, const char *data, int length)
{
	UDP::Address dst(IP::Address(destination), port);
	socket.set_remote(dst);

	//db<SipTransportLayer>(INF) << "SipTransportLayer::sendData -> Sending data to " << dst << " (size = " << length << ")..\n" << data << "\n";

	if (socket.send(data, length) <= 0)
	{
		db<SipTransportLayer>(WRN) << "SipTransportLayer::sendData -> Failed to send data\n";
		return -1;
	}

	return 0;
}

void SipTransportLayer::MySocket::received(const UDP::Address &src, const char *data, unsigned int size)
{
	db<SipTransportLayer::MySocket>(INF) << "SipTransportLayer::MySocket::received..\n";

	char remoteAddr[20];
	src.ip().to_string(remoteAddr);
	int remotePort = src.port();
	((char *) data)[size] = 0;

	db<SipTransportLayer>(INF) << "SipTransportLayer::received -> Received message from " << remoteAddr <<
			":" << remotePort << " (size = " << size << ")..\n" << data << "\n";

	SipMessage *msg = SipMessage::decodeMsg(data);

	if (!msg)
	{
		db<SipTransportLayer>(WRN) << "SipTransportLayer::received -> Failed to decode SIP message received\n";
		return;
	}

	bool ok = false;
	SipHeaderVia *via = (SipHeaderVia *) msg->getHeader(SIP_HEADER_VIA);
	if (via)
	{
		const char *sentby = via->getSentBy();
		if (sentby)
		{
			if (msg->getMsgType() == SIP_RESPONSE)
			{
				if (!strcmp(sentby, SipManager::getInstance()->getTransport()->getHostIP()))
					ok = true;
			}else
			{
				if (strcmp(sentby, remoteAddr))
					via->setReceived(remoteAddr);
				ok = true;
			}
		}
	}

	if (!ok)
	{
		db<SipTransportLayer>(WRN) << "SipTransportLayer::received -> Incorrect message...\n";
		delete msg;
		return;
	}

	SipManager::getInstance()->addMessageList(msg);
}

//-------------------------------------------

USER_CALLBACK SipManager::callback = 0;
SipManager *SipManager::instance = 0;
bool SipManager::terminated = false;


SipManager::SipManager()
{
	threadReceiveMessage = 0;
}

SipManager::~SipManager()
{
	terminated = true;

	if (threadReceiveMessage)
	{
		delete threadReceiveMessage;
		threadReceiveMessage = 0;
	}

	Simple_List<UserAgent>::Iterator it = ua.begin();
	while (it != ua.end())
	{
		UserAgent *userAgent = it->object();
		ua.remove(it++);
		delete userAgent;
	}
}

SipManager *SipManager::getInstance()
{
	if (!instance)
	{
		terminated = false;
		instance = new SipManager();
	}

	return instance;
}

void SipManager::deleteInstance()
{
	if (instance)
	{
		delete instance;
		instance = 0;
	}
}

bool SipManager::init()
{
	if (!transport.init())
		return false;

	threadReceiveMessage = new Thread(SipManager::receiveMessageThread);
	return true;
}

int SipManager::receiveMessageThread()
{
	db<SipManager>(INF) << "SipManager::receiveMessageThread -> Started..\n";

	SipManager *manager = SipManager::getInstance();

	while (!terminated)
	{
		while (manager->messages.size() == 0)
		{
			if (terminated)
				break;
			Alarm::delay(500); //Alarm::delay(500 * 1000);
		}

		if (terminated)
			break;

		SipMessage *msg = manager->getMessageList();

		if (!msg)
		{
			db<SipManager>(WRN) << "SipManager::receiveMessageThread -> Invalid message\n";
			continue;
		}

		SipMessageType msgType = msg->getMsgType();
		db<SipManager>(WRN) << "SipManager::receiveMessageThread -> Message received (type=" << msgType << ")\n";

		if (msgType == SIP_RESPONSE)
		{
			const char *uri = ((SipHeaderFrom *) msg->getHeader(SIP_HEADER_FROM))->getAddress();
			UserAgent *userAgent = manager->getUserAgent(uri);

			if (!userAgent)
				db<SipManager>(WRN) << "SipManager::receiveMessageThread -> Ignoring invalid response\n";
			else
				userAgent->getUserAgentClient()->receiveMsg((SipResponse *) msg);
		}else
		{
			const char *uri = ((SipRequest *) msg)->getRequestLine()->getRequestURI();
			UserAgent *userAgent = manager->getUserAgent(uri);

			if (!userAgent)
				db<SipManager>(WRN) << "SipManager::receiveMessageThread -> Ignoring invalid request\n";
			else
				userAgent->getUserAgentServer()->receiveMsg((SipRequest *) msg);
		}

		if (msg->getCanDelete())
			delete msg;
	}

	db<SipManager>(INF) << "SipManager::receiveMessageThread -> Stopped..\n";
	return 0;
}

UserAgent *SipManager::createUserAgent(const char *uri)
{
	UserAgent *userAgent = new UserAgent(uri);
	ua.insert(&userAgent->link);
	return userAgent;
}

UserAgent *SipManager::getUserAgent(const char *uri)
{
	Simple_List<UserAgent>::Iterator it = ua.begin();
	while (it != ua.end())
	{
		UserAgent *userAgent = it->object();
		it++;

		if (!strcmp(userAgent->getUri(), uri))
			return userAgent;
	}
	return 0;
}

void SipManager::addMessageList(SipMessage *msg)
{
	messagesMutex.lock();
	messages.insert(&msg->link);
	messagesMutex.unlock();
}

SipMessage *SipManager::getMessageList()
{
	messagesMutex.lock();
	SipMessage *msg = messages.head()->object();
	messages.remove_head();
	messagesMutex.unlock();
	return msg;
}

void SipManager::random(char *buffer)
{
	unsigned long value = Pseudo_Random::random();
	itoa(value, buffer);
}

__END_SYS
