#include <ip/udp.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS

// TCP's checksum was added to UDP, we should merge this to avoid code redundancy
void UDP::Header::checksum(IP::Address src,IP::Address dst,SegmentedBuffer * sb)
{
    if (!Traits<UDP>::checksum) {
        _checksum = 0;
        return;
    }

    db<UDP>(TRC) << __PRETTY_FUNCTION__ << endl;
    u16 len;
    len = sizeof(this);

    if (sb) len += sb->total_size();

    Pseudo_Header phdr((u32)src,(u32)dst,len);

    _checksum = 0;

    u32 sum = 0;

    sum = IP::calculate_checksum(&phdr, sizeof(phdr));
    sum += IP::calculate_checksum(this, sizeof(this));

    while (sb) {
        sum += IP::calculate_checksum(sb->data(), sb->size());
        sb = sb->next();
    }
    
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    _checksum = ~sum;
}

// Assembles data and sends to IP layer

s32 UDP::send(Address _local, Address _remote, SegmentedBuffer * data) {
	UDP::Header hdr(_local.port(), _remote.port(),
			data->total_size());
	SegmentedBuffer sb(&hdr, sizeof(UDP::Header), data);
	hdr.checksum(_local.ip(),_remote.ip(),&sb);
	return _ip->send(_local.ip(), _remote.ip(), &sb, ID_UDP) - 8;	// discard header
}

// Called by IP's notify(...)

void UDP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
	         IP::Address dst, void *data, unsigned int size)
{
	Header& hdr = *reinterpret_cast<Header*>(data);

	db<UDP>(INF) << "UDP::update: received "<< size <<" bytes from " 
	            << src << " to " << dst << "\n";

    if (Traits<UDP>::checksum && hdr._checksum != 0) {
        SegmentedBuffer sb(data + sizeof(Header), size - sizeof(Header));
        u16 csum = hdr._checksum;
        hdr.checksum(src,dst,&sb);
        if (hdr._checksum != csum) {
            db<UDP>(INF) << "UDP::checksum failed for incomming data\n";
            return;
        }
    }
	notify(UDP::Address(src,hdr.src_port()),UDP::Address(dst,hdr.dst_port()),
	       (int) hdr.dst_port(), &((char*)data)[sizeof(Header)],
	       size - sizeof(Header));
}

// Called by UDP's notify(...)

void UDP::Socket::update(Observed *o, long c, UDP_Address src, UDP_Address dst,
                         void *data, unsigned int size)
{
	db<UDP>(TRC) << __PRETTY_FUNCTION__ << "\n";
	
	// virtual call
	received(src,(const char*)data,size);
}

__END_SYS

