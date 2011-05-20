#include <ip/ip.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS

IP* IP::_instance[Traits<NIC>::NICS::Length];
const IP::Address IP::NULL = IP::Address((u32)0);

u16 IP::Header::pktid = 0; // incremental packet id

// IP::Header
void IP::Header::calculate_checksum() {
    _checksum = 0;
    _checksum = ~(IP::calculate_checksum(this, hlength()));
}


// IP

IP::IP(unsigned int unit) 
    : _nic(unit),
      _router(&_nic, this),
     _self(IP::NULL),
     _broadcast(255,255,255,255),
     _thread(0)
{
    if (_instance[unit])
    {
        db<IP>(ERR) << "IP::created IP object twice for the same NIC!";	
    }
    _router.update(_broadcast, NIC::BROADCAST);

    if (Traits<IP>::dynamic == false) {
        _self = Address(Traits<IP>::ADDRESS);
        _router.update(_self,_nic.address());
        _broadcast = Address(Traits<IP>::BROADCAST);
        _router.update(_broadcast, NIC::BROADCAST);
        _netmask = Address(Traits<IP>::NETMASK);
    }

    if (Traits<IP>::spawn_thread) {
        _thread = new Thread(IP::thread_main,this);
    }

    _instance[unit] = this;
}

IP::~IP() {
    if (Traits<IP>::spawn_thread) {
        delete _thread;
    }
}

void IP::process_ip(char *data, u16 size)
{
    Header &pck_h = *reinterpret_cast<Header*>(data);
    if((u32)_self != (u32)0 && // We MUST accept anything if our IP address is not set
       (u32)(pck_h.dst_ip()) != (u32)(_self) &&
       (u32)(pck_h.dst_ip()) != (u32)(_broadcast))
    {
        db<IP>(INF) << "IP Packet discarded. dst= " << pck_h.dst_ip() << "\n";
        return;
    }
    else {
        db<IP>(TRC) << "IP: " << pck_h << "\n" ;
    }

    if(pck_h.flags() != Header::MF_FLAG && pck_h.offset() == 0)
    {
        if (calculate_checksum(data,pck_h.hlength()) != 0xFFFF) {
            db<IP>(TRC) << "IP checksum failed for incoming packet\n";
        } else {
            notify(pck_h.src_ip(),pck_h.dst_ip(),(int)pck_h.protocol(),
                &data[pck_h.hlength()], pck_h.length() - pck_h.hlength());
            if (pck_h.ttl() > 0) {
                pck_h.ttl(pck_h.ttl() - 1);
            }
        }
    }
    else {
        db<IP>(WRN) << "Fragmented packet discarded" << endl;
         // TODO: reasemble fragmented packets
    }
}




/*void IP::update(NIC::Observed * o, int p)
{
	db<IP>(TRC) << "IP::update(o=" << o
		     << ",p=" << hex << p << dec << ")\n";

	//if (_thread) _thread->resume();
}*/

void IP::kill()
{
    _alive = false;
    if (_thread) {
        _thread->resume();
    }
}

void IP::process_incoming()
{
    db<IP>(TRC) << __PRETTY_FUNCTION__ << endl;
    NIC::Address src;
    NIC::Protocol prot;
    NIC::PDU data;
    int size = _nic.receive(&src, &prot, data, sizeof(NIC::PDU));
    if(size <= 0) {
        db<IP>(WRN) << "NIC::received error!" << endl;
        return;
    }

    if (prot == NIC::IP) {
        _router.update(reinterpret_cast<Header*>(data)->src_ip(), src);
        process_ip(data, size);
    }

    // notify routing algorithm
    _router.received(src, prot, data, size);
}

void IP::worker_loop()
{
    _alive = true;
    _thread = Thread::self();
    while (_alive) {
        process_incoming();
        Thread::yield();
    }
}

s32 IP::send(const Address & from,const Address & to,SegmentedBuffer * data,Protocol proto)
{
    Header hdr(from,to,proto,data->total_size());
    SegmentedBuffer pdu(&hdr,hdr.hlength(),data);

    MAC_Address mac = NIC::BROADCAST;
    if (from.is_neighbor(to,_netmask))
        mac = _router.resolve(to,&pdu);
    else
        mac = _router.resolve(_gateway,&pdu);

    //TODO: put fragmentation here
    int size = pdu.total_size();
    char sbuf[size];
    //TODO: possible stack overflow here, we must change NIC::send to accept SegmentedBuffers
    pdu.copy_to(sbuf,size);   

    db<IP>(TRC) << "IP::send() " << size << " bytes" << endl;
        
    if (_nic.send(mac,NIC::IP,sbuf,size) >= 0)
        return size;
    else
        return -1;

}

// From http://www.faqs.org/rfcs/rfc1071.html
u16 IP::calculate_checksum(const void* ptr, u16 count)
{
    u32 sum = 0;

    const unsigned char * _ptr = reinterpret_cast<const unsigned char *>(ptr);
    u16 i;

    for(i = 0; i < count-1; i+=2)
        sum += (((unsigned short)(_ptr[i+1]) & 0x00FF) << 8) | _ptr[i];
    if(count & 1) {
        sum += _ptr[count-1];
    }

    while(sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return sum;
}

int IP::thread_main(IP * thiz) {
    thiz->worker_loop();
    return 0;
}

__END_SYS

