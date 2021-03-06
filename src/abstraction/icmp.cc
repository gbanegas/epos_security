#include <ip/icmp.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS

ICMP::ICMP(IP* ip = 0) : _ip(ip)
{
    // if no IP is given we try to pick a global one
    if (!_ip)
        _ip = IP::instance();

    if (!_ip)
        db<ICMP>(ERR) << "ICMP::Cannot get default IP object, ICMP will not work\n";
    else
        _ip->attach(this, ICMP_ID);
}

ICMP::~ICMP()
{
    if (_ip)
        _ip->detach(this, ICMP_ID);
}

void ICMP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
                  IP::Address dst, void *data, unsigned int size)
{
    Packet& packet = *reinterpret_cast<Packet*>(data);
    if (IP::calculate_checksum(data,size) != 0xFFFF) {
        db<ICMP>(TRC) << "ICMP::checksum error\n";
        return;
    }

    if (Traits<ICMP>::echo_reply && (packet.type() == ECHO)) { // PONG
        db<ICMP>(TRC) << "ICMP::echo sending automatic reply to " << src << endl;
        Packet reply(ECHO_REPLY,0,packet.id(),packet.sequence(),packet._data);
        send(dst,src,reply);
    }

    if (packet.type() == ECHO_REPLY) {
        db<ICMP>(TRC) << "ICMP::echo reply from " << src << endl;
    }

    notify(src,dst,packet.type(),data,size);
}

ICMP::Packet::Packet(Type type,Code code, unsigned short id,unsigned short seq,
                     const char * data,short size)
    : _type(type),
      _code(code),
      _checksum(0),
      _id(CPU::htons(id)),
      _sequence(CPU::htons(seq))
{
    memset(_data, 0, 56);
    if (data) memcpy(_data,data,size < 56 ? size : 56);
}

void ICMP::send(IP::Address from,IP::Address to,Packet& pkt)
{
    if (!_ip)
        return;

    // Thou shall not calculate the checksum in ctor body!
    pkt._checksum = 0;
    pkt._checksum = ~(IP::calculate_checksum(&pkt, sizeof(pkt)));
    SegmentedBuffer sb(pkt.raw(),sizeof(pkt));
    _ip->send(from,to,&sb,ICMP_ID);
}

__END_SYS
