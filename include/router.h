#ifndef __router_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __router_h

#include <arp.h>
#include <adhop.h>
#include <utility/debug.h>
#include <utility/buffer.h>

__BEGIN_SYS

/// A dummy router that always resolves logical address to broadcast addressess
template <class LinkLayer, class NetworkLayer>
class BCast_Router {
    typedef typename LinkLayer::Address    MAC_Address;
    typedef typename NetworkLayer::Address Address;
    typedef typename LinkLayer::Protocol   Protocol;
    typedef typename LinkLayer::PDU        PDU;
public:

    BCast_Router(LinkLayer * nic, NetworkLayer * network) { }

    void update(const Address & addr, const MAC_Address & mac)
    
    {
        // do nothing
    }
    
    MAC_Address resolve(const Address& addr,SegmentedBuffer * pdu)
    {
        return MAC_Address(LinkLayer::BROADCAST);    
    }
    
    void received(const MAC_Address& src, Protocol proto,
                  const PDU& data, int size) {}

   
};

/// The ARP_Router will resolve address using the ARP protocol
template <class LinkLayer,class NetworkLayer>
class ARP_Router {
protected:
    typedef typename LinkLayer::Address    MAC_Address;
    typedef typename NetworkLayer::Address Address;
    typedef typename LinkLayer::Protocol   Protocol;
    typedef typename LinkLayer::PDU        PDU;
    typedef ARP< LinkLayer , NetworkLayer >  _ARP;
    typedef typename _ARP::Packet          Packet;
        
    typename ARP<LinkLayer, NetworkLayer>::Table    _arpt;
    LinkLayer    * _nic;
    NetworkLayer * _net;
    
public:
    ARP_Router(LinkLayer * nic, NetworkLayer * net) : _nic(nic), _net(net)
    {
        
    }
    
    MAC_Address resolve(Address addr,SegmentedBuffer * pdu)
    {
        for(unsigned int i = 0; i < Traits<Network>::ARP_TRIES; i++)
        {
            MAC_Address pa = _arpt.search(addr);
            if(pa) {
                db<NetworkLayer>(TRC) << "ARP_Router::resolve(addr=" << addr << ") => "
                        << pa << "\n";

                return pa;
            }

            Condition * cond = _arpt.insert(addr);
            Packet request(_ARP::REQUEST, _nic->address(), _net->address(),
                                LinkLayer::BROADCAST, addr);
            
            _nic->send(LinkLayer::BROADCAST, LinkLayer::ARP, &request, sizeof(Packet));
        
            db<NetworkLayer>(INF) << "ARP_Router::resolve:request sent!\n";

            Condition_Handler handler(cond);
            //Alarm alarm(Traits<Network>::ARP_TIMEOUT, &handler, 1);
            Alarm alarm(100000, &handler, 1);
            cond->wait();
        }

        db<NetworkLayer>(TRC) << "ARP_Router::resolve(addr=" << addr << ") => not found!\n";

        return 0;
    }
    
    void update(Address addr, MAC_Address mac) {
        _arpt.update(addr, mac);
    }
    
    void received(const MAC_Address& src, Protocol proto,
                  const PDU& data, int size)
    {
        if (proto == LinkLayer::ARP) {
            const Packet& packet = *reinterpret_cast<const Packet *>(data);
            db<NetworkLayer>(INF) << "IP::update:ARP_Packet=" << packet << "\n";

            if((packet.op() == _ARP::REQUEST) && (packet.tpa() == _net->address()))
            {
                Packet reply(_ARP::REPLY, _nic->address(), _net->address(),
                        packet.sha(), packet.spa());
                
                db<NetworkLayer>(INF) << "IP::update: ARP_Packet=" << reply << "\n";
                _nic->send(packet.sha(), LinkLayer::ARP, &reply, sizeof(Packet));

                db<NetworkLayer>(INF) << "IP::update: ARP request answered!\n";
            }
            else if((packet.op() == _ARP::REPLY) && (packet.tha() == _nic->address())) 
            {
                db<NetworkLayer>(INF) << "IP::update: ARP reply received!\n";

                _arpt.update(packet.spa(), packet.sha());
            }    
        }
    }
   
private:
    // disable default constructor
    ARP_Router();
    // disable copy constructor
    ARP_Router(const ARP_Router& r);
};

template <class LinkLayer, class NetworkLayer>
class ADHOP_Router {
    typedef typename LinkLayer::Address    Link_Address;
    typedef typename LinkLayer::Protocol   Protocol;
    typedef typename LinkLayer::PDU        PDU;
    typedef typename NetworkLayer::Address Network_Address;
    typedef typename NetworkLayer::Header  Network_Header;

    typedef typename ADHOP<LinkLayer,NetworkLayer>::Ant Ant;

public:
    ADHOP_Router(LinkLayer* nic, NetworkLayer* network)
    {
        _nic = nic;
        _network = network;
        _my_net_address = network->address();
        _my_nic_address = nic->address();
        _router.init(nic->address());
    }

    void update(const Network_Address& la, const Link_Address& pa)
    {
        // do nothing
    }

    Link_Address resolve(const Network_Address& dst, SegmentedBuffer * pdu)
    {
        Ant * ant = (Ant *) ((Network_Header *) pdu->data())->get_options();
        Link_Address result = _router.resolve(dst, (void *) ant);
        return result; 
    }

    void received(const Link_Address& src, Protocol proto,
            const PDU& data, int size)
    {
        if (proto == LinkLayer::IP) {
            Ant * ant = (Ant *) ((Network_Header *) data)->get_options();
            Network_Address net_dst = ((Network_Header *) data)->dst_ip();
            Network_Address net_src = ((Network_Header *) data)->src_ip();
            if (net_dst == _my_net_address) {
                if (_router.update(net_src, (void *) ant)) {
                    //TODO: it is not infoming the mode "return" to the ant
                    _network->send(net_src, (char *) 0, 0, 0);
                }
            } else {
                Link_Address next_hop = _router.resolve(net_src, net_dst, (void *) ant);
                ((Network_Header *) data)->calculate_checksum();
                int result = _nic->send(next_hop, proto, data, size); 
            }
        }
    }

private:
    LinkLayer* _nic;
    NetworkLayer* _network;
    Link_Address _my_nic_address;
    Network_Address _my_net_address;
    ADHOP<LinkLayer,NetworkLayer> _router;
};

__END_SYS

#endif

