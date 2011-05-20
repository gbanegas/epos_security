#ifndef __arp_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __arp_h

#include <utility/malloc.h>
#include <utility/hash.h>
#include <alarm.h>
#include <condition.h>

__BEGIN_SYS

template<typename NIC, typename Network>
class ARP
{
private:
    typedef typename Network::Address Address;
    typedef typename NIC::Address MAC_Address;

    class Entry {
    public:
	Entry(const Address & la, MAC_Address pa = NIC::BROADCAST):
	    _phy_addr(pa), _link(this, la) {}

	MAC_Address _phy_addr;
	Condition _condition;
	typename Simple_Hash<Entry, 8>::Element _link;
    };

public:
    // ARP Operations
    typedef unsigned short Oper;
    enum {
	REQUEST		= 1,
	REPLY		= 2,
	RARP_REQUEST	= 3,
	RARP_REPLY	= 4
    };

    class Table: public Simple_Hash<Entry, 8> {
    private:
	typedef Simple_Hash<Entry, 8> Base;

    public:
	typedef typename Base::Element Element;

    public:
	Condition * insert(const Address & la) {
	    Element * el = Base::search_key(la);
	    Entry * e;
	    
	    if(el)
		e = el->object();
	    else {
		e = new(kmalloc(sizeof(Entry))) Entry(la);
		Base::insert(&e->_link);
	    }
		    
	    return &e->_condition;
	}
    
	void update(const Address & la, const MAC_Address & pa) {
	    Element * el = Base::search_key(la);
	    Entry * e;
	    
	    if(el) {
		e = el->object();
		e->_phy_addr = pa;
		e->_condition.broadcast();
	    } else {
		e = new(kmalloc(sizeof(Entry))) Entry(la, pa);
		Base::insert(&e->_link);
	    }
	}
    
	MAC_Address search(const Address & la) {
	    Element * el = Base::search_key(la);
	    
	    if(el)
		return el->object()->_phy_addr;
	    else
		return 0;
	}
    };

    class Packet
    {
    public:
	// Hardware Types (HTYPE)
	enum { HTYPE_ETHERNET = 1 };

    public:
	Packet(Oper op, 
	       MAC_Address sha, Address spa, MAC_Address tha, Address tpa):
	    _htype(CPU::htons(HTYPE_ETHERNET)),
	    _ptype(CPU::htons(Network::PROT_IP)),
	    _hlen(sizeof(MAC_Address)),
	    _plen(sizeof(Address)),
	    _oper(CPU::htons(op)),
	    _sha(sha),
	    _spa(spa),
	    _tha(tha),
	    _tpa(tpa) {}
	
	void op(const Oper & o) { _oper = CPU::htons(o); }
	void sha(const MAC_Address & a) { _sha = a; }
	void spa(const Address & a) { _spa = a; }
	void tha(const MAC_Address & a) { _tha = a; }
	void tpa(const Address & a) { _tpa = a; }

	Oper op() const { return CPU::ntohs(_oper); }
	const MAC_Address & sha() const { return _sha; }
	const Address & spa() const { return _spa; }
	const MAC_Address & tha() const { return _tha; }
	const Address & tpa() const { return _tpa; }

	friend Debug & operator<<(Debug & db, const Packet & p) {
	    db << "{htype="  << CPU::ntohs(p._htype)
	       << ",ptype="  << hex << CPU::ntohs(p._ptype) << dec
	       << ",hlen="   << p._hlen
	       << ",plen="   << p._plen
	       << ",opcode=" << CPU::ntohs(p._oper)
	       << ",sha="    << p._sha
	       << ",spa="    << p._spa
	       << ",tha="    << p._tha
	       << ",tpa="    << p._tpa << "}";

	    return db;
	}	
	
    private:
	unsigned short	_htype;	// Hardware Type
	unsigned short	_ptype;	// Protocol Type
	unsigned char	_hlen;	// Hardware Address Length
	unsigned char	_plen;	// Protocol Address Length
	unsigned short	_oper;	// Operation
	MAC_Address	_sha;	// Sender Hardware Address (48 bits)
	Address		_spa;	// Sender Protocol Address (32 bits)
	MAC_Address	_tha;	// Target Hardware Address (48 bits)
	Address		_tpa;	// Target Protocol Address (32 bits)
    };
};

__END_SYS

#endif

