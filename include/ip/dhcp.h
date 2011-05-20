#ifndef DHCPC_H

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define DHCPC_H

#include "udp.h"
#include <utility/random.h>

__BEGIN_SYS
/*
 * Reference:
 * DHCP:         http://www.ietf.org/rfc/rfc2131.txt
 * DHCP options: http://www.ietf.org/rfc/rfc2132.txt
 */
class DHCP {
public:
    // Limited to opt_size = 308

    template<int opt_size> class Packet {
    public:
        u8 _op, _htype, _hlen, _hopts;
        u32 _xid;
        u16 _secs, _flags;
        u32 _ciaddr, _yiaddr, _siaddr, _giaddr;
        u8 _chaddr[16];
        u8 _sname[64];
        u8 _file[128];
        u8 _magic[4];
        u8 _options[opt_size];
        u8 _end;
        u8 _padding[312 - 5 - opt_size];

        u8 op() const { return _op; }
        u32 xid() const { return _xid; }
        u16 secs() const { return CPU::ntohs(_secs); }
        u32 your_address() const { return CPU::ntohl(_yiaddr); }
        u32 server_address() const { return CPU::ntohl(_siaddr); }
        u8 * options() const { return const_cast<u8 * const>(_options); }

        Packet()
        {
            memset(&_op, 0, sizeof(Packet));
            _magic[0] =  99; // magic cookie
            _magic[1] = 130;
            _magic[2] =  83;
            _magic[3] =  99;
            _end = 255; // end of options
        }

    };

    class Discover : public Packet<3> {
    public:
        Discover(IP * _net) : Packet<3>() {
            _op = 1;
            _htype = 1;
            _hlen = _net->hw_address_len();
            _xid = Pseudo_Random::random();
            memcpy(_chaddr, &_net->hw_address(), _hlen);
            _options[0] = 53; // DHCPMSG
            _options[1] = 1;  // message size
            _options[2] = 1;  // dhcp discover
        }

    };

    class Request : public Packet<8> {
    public:
        Request(IP * _net,const Packet<255> * discovered) : Packet<8>() {
            _op = 1;
            _htype = 1;
            _hlen = _net->hw_address_len();
            _xid = discovered->_xid;
            _ciaddr = discovered->_ciaddr;
            _siaddr = discovered->_siaddr;
            memcpy(_chaddr, &_net->hw_address(), _hlen);
            _options[0] = 53; // DHCP message
            _options[1] = 1;  // size
            _options[2] = 3;  // dhcp discover
            _options[3] = 55; // parameter request
            _options[4] = 3;  // size
            _options[5] = 1;  // subnet
            _options[6] = 3;  // router
            _options[7] = 6;  // dns
        }
    };

    class Client;
};

class DHCP::Client : public UDP::Socket {
public:
    enum {
        IDLE,
        DISCOVER,
        REQUEST,
        RENEW,
        RELEASE
    };
protected:
    short _state;

    u32 _xid;

    IP::Address _ip, _mask, _gw, _bcast, _ns;
public:
    Client(UDP * udp) :
        UDP::Socket(udp,UDP::Address(0,68),UDP::Address(~0,67)),
        _state(IDLE)
    {

    }

    ~Client() {}

    // UDP::Socket virtual methods
    void received(const UDP::Address & src,const char *data, unsigned int size)
    {
        db<IP>(INF) << "DHCP::Client state: " << _state << "\n";
        const DHCP::Packet<255> * packet = reinterpret_cast<const DHCP::Packet<255> *>(data);

        if (_xid != packet->xid()) {
            db<IP>(TRC) << "This DHCP message does not belong to me\n";
        }

        switch(_state) {
        case DISCOVER:

            if (packet->your_address()) {
                set_remote(src);

                UDP::Address me(packet->your_address(),68);
            //  set_local(me);
            //  _udp->ip()->set_address(me.ip());

                _state = REQUEST;
                db<IP>(INF) << "Server " << src.ip() << " offered IP " << me.ip() << "\n";
                parse_options(packet);

                DHCP::Request req(_udp->ip(),packet);
                SegmentedBuffer sb(&req,sizeof(DHCP::Request));
                send(&sb);
            }

            break;

        case REQUEST:

            if (packet->your_address()) {
                _ip = IP::Address((u32)packet->your_address());

                parse_options(packet);
                _state = RENEW;

            }
        }
    }

    void configure() {
        db<IP>(INF) << "DHCP::Client sending discover msg\n";
        _state = DISCOVER;

        DHCP::Discover pkt(_udp->ip());
        SegmentedBuffer sb(&pkt,sizeof(DHCP::Discover));

        _xid = pkt.xid();
        send(&sb);
    }

    void renew() {

    }

    void release() {

    }

    void parse_options(const Packet<255> * packet) {
        db<IP>(TRC) << "DHCP::Parsing OPTIONS\n";
    
        u8 * opt = packet->options();
        int i;

        for(i=0;i < 255;i++) {
            switch(opt[i]) {
            case 0: // padding
                break;
            case 1: // netmask
                ++i;
                if (opt[i] == 4) // IPv4, good
                {
                   _mask = IP::Address(opt[i+1],opt[i+2],opt[i+3],opt[i+4]); 
                    db<IP>(TRC) << "Found netmask " << _mask << endl;
                }
                i += opt[i];
                break;
            case 3: // routers
                ++i;
                if (opt[i] >= 4) // one or more, let's get the first
                {
                   _gw = IP::Address(opt[i+1],opt[i+2],opt[i+3],opt[i+4]); 
                    db<IP>(TRC) << "Found gateway " << _gw << endl;
                }
                i += opt[i];
                break;
            case 6: // nameserver
                ++i;
                if (opt[i] >= 4) // same logic as routers
                {
                   _ns = IP::Address(opt[i+1],opt[i+2],opt[i+3],opt[i+4]); 
                    db<IP>(TRC) << "Found nameserver " << _ns << endl;
                }
                i += opt[i];
                break;
            case 28: // broadcast address
                ++i;
                if (opt[i] == 4) // IPv4, good
                {
                   _bcast = IP::Address(opt[i+1],opt[i+2],opt[i+3],opt[i+4]); 
                    db<IP>(TRC) << "Found bcast " << _bcast << endl;
                }
                i += opt[i];
                break;
            case 51: // lease time in secs
                ++i;
                if (opt[i] == 4) { // Good size!
                    _lease_time = (((u32)(opt[i+1]) << 24) & 0xFF000000) |
                                  (((u32)(opt[i+2]) << 16) & 0x00FF0000) |
                                  (((u32)(opt[i+3]) << 8 ) & 0x0000FF00) |
                                  (((u32)(opt[i+4])      ) & 0x000000FF);
                    db<IP>(TRC) << "Lease time " << _lease_time << endl;
                }
                i += opt[i];
                break;
            case 255: // end
                i = 500; // get out of the loop
                {
                    db<IP>(TRC) << "End of options " << endl;
                }
                break;
            default:
                {
                db<IP>(TRC) << "Skipping code " << (int)opt[i]  << " len: " << opt[i+1] << endl;
                    i += opt[i+1] + 1;
                }

            }
        }
    }

    unsigned long _lease_time;

    IP::Address address() { return _ip; }
    IP::Address netmask() { return _mask; }
    IP::Address gateway() { return _gw; }
    IP::Address brodcast() { return _bcast; }
    IP::Address nameserver() { return _ns; }
};

__END_SYS

#endif
