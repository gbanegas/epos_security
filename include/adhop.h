#ifndef ADHOP_H_

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define ADHOP_H_

#include <system/kmalloc.h>
#include <utility/hash.h>

#define MAX_LIST 3
#define MAX_HASH 5

__BEGIN_SYS

template<typename Link_Layer, typename Network_Layer>
class ADHOP
{
public:
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long u32;
    typedef unsigned long long u64;

    typedef typename Network_Layer::Address Network_Address;
    typedef typename Link_Layer::Address Link_Address;

    static const unsigned int INITIAL_PHEROMONE = 100;
    static const unsigned int TIMER_CHECK_ROUTING_TABLES = 5000000; // 5s

    ADHOP(): seqNO(0)
    {}

    void init(const Link_Address& pa)
    {
        my_address = pa;
    }

    enum {
        ITA = 0,
        ETA = 1
    };

    class Ant
    {
    public:
        u32 type:1;
        u32 returning:1;
        u32 sequenceNO:14;
        u32 heuristic:16;
        Link_Address neighbor;
    } __attribute__((packed));

private:
    class RTable_Neighbor
    {
    public:
        typedef List_Elements::Singly_Linked_Ordered<RTable_Neighbor, u16> Element;

        RTable_Neighbor(const Network_Address& la, const Link_Address& pa):
            _link(this, INITIAL_PHEROMONE)
        {
            this->restart();
            _destination_node = la;
            _neighbor = pa;
        }

        bool evaporate() //EVAPORATION
        {
            _link.rank((u16) (_link.rank() >> 1));
            if(_link.rank() > 4)
                return false;
            else
                return true;
        }
        void update(u16 heuristic) //UPDATE_PHEROMONE
        {
            if (!heuristic)
                _link.rank( _link.rank() + (u16) (_link.rank() >> 1));
        }

        bool match(const Network_Address& la) { return (la == _destination_node); }
        bool match(const Network_Address& la, const Link_Address& pa)
        {
            return ((la == _destination_node) && (pa == _neighbor));
        }

        bool verify(u16 seq)
        {
            if (seq > this->_seqNO) {
                this->_seqNO = seq;
                return true;
            } else {
                if (seq == 0) {
                    this->restart();
                }
                return false;
            }
            return false;
        }

    private:
        void restart()
        {
            _seqNO = 0;
        }

        u16 _seqNO:14;
    public:
        Link_Address _neighbor;
        Network_Address _destination_node;
        Element _link;
    };

    class RTable_Neighbors:
        Simple_List<RTable_Neighbor, typename RTable_Neighbor::Element>
    {
    private:
        typedef Simple_List<RTable_Neighbor, typename RTable_Neighbor::Element> Base;
        typedef typename Base::Object_Type Object_Type;
        typedef typename Base::Element Element;

    public:
        RTable_Neighbors(): Base()
        {}

        void clear()
        {
            while(Base::size()) {
                Element* element = Base::remove();
                if(element) {
                    delete element->object();
                }
            }
        }

        void insert(Element * e)
        {
            if(Base::size() >= MAX_LIST) {
                Element* element = Base::remove_tail();
                if(element) {
                    delete element->object();
                }
            }
            if(Base::empty())
                insert_first(e);
            else {
                Element * next, * prev;
                for(
                        next = Base::head(), prev = 0;
                        (next->rank() > e->rank()) && next->next();
                        prev = next, next = next->next()
                   );
                if(next->rank() > e->rank()) {
                    insert_tail(e);
                } else if(!prev) {
                    insert_head(e);
                } else {
                    Base::insert(e, prev, next);
                }
            }
        }

        void update()
        {
            for(Element* e = Base::head(); e; e = e->next()) {
                if (e->object()->evaporate()) {
                    Base::remove(e);
                    delete e->object();
                }
            }
        }

        void update(const Network_Address& la, const Link_Address& pa, u16 heuristic)
        {
            Element* element = search_entry(la, pa);
            if(element) {
                Base::remove(element);
                element->object()->update(heuristic);
                this->insert(element);
            } else {
                Object_Type* neighbor =
                    new(kmalloc(sizeof(Object_Type))) Object_Type(la, pa);
                this->insert(&neighbor->_link);
            }
        }

        Element* search_entry(const Network_Address& la, const Link_Address& pa)
        {
            Element * e;
            for(e = Base::head(); e && !e->object()->match(la, pa); e = e->next());
            return e;
        }

        Link_Address search_address(const Network_Address& la)
        {
            Element * e;
            for(e = Base::head(); e && !e->object()->match(la); e = e->next());
            if(e)
                return e->object()->_neighbor;
            else
                return Link_Layer::BROADCAST;
        }

        bool verify(const Network_Address& la, u16 seq)
        {
            Element * e;
            for(e = Base::head(); e && !e->object()->match(la); e = e->next());
            if(e)
                return e->object()->verify(seq);
            else
                return true;
        }
    };

    class RTable:
        Hash<RTable_Neighbor, MAX_HASH, int, typename RTable_Neighbor::Element, RTable_Neighbors>
    {
    private:
        typedef
            Hash<RTable_Neighbor, MAX_HASH, int, typename RTable_Neighbor::Element, RTable_Neighbors>
            Base;

    public:
        typedef typename Base::Element Element;
        typedef typename Base::List List;

        RTable()
        {}

        ~RTable()
        {
            for(int i = 0; i < MAX_HASH; i++) {
                List * list = (*this)[i]; 
                list->clear();
            }
        }

        void update()
        {
            for(int i = 0; i < MAX_HASH; i++) {
                List * list = (*this)[i]; 
                list->update();
            }
        }

        void update(const Network_Address& la, const Link_Address& pa, u16 heuristic = 0)
        {
            List * list = (*this)[la];
            list->update(la, pa, heuristic);
        }

        Link_Address search(const Network_Address& la)
        {
            List * list = (*this)[la];
            return list->search_address(la);
        }

        bool verify(const Network_Address& la, u16 seq)
        {
            List * list = (*this)[la];
            return list->verify(la, seq);
        }
    };

public:
    bool update(const Network_Address& src, void* header)
    {
        Ant* ant = (Ant*) header;
        if (rtable.verify(src, ant->sequenceNO)) {
            rtable.update(src, ant->neighbor, ant->sequenceNO);
        }
        if (ant->type == ETA && ant->returning == 0)
        {
            ant->returning = 1;
            return true;
        }
        return false;
    }

    Link_Address resolve(const Network_Address& dst, void* header)
    {
        Ant* ant = (Ant*) header;
        Link_Address nic_addr = rtable.search(dst);
        if (nic_addr == Link_Layer::BROADCAST) {
            ant->type = ETA;
        } else {
            ant->type = ITA;
        }
        ant->sequenceNO = seqNO++;
        ant->neighbor = my_address;
        ant->heuristic = 0;
        ant->returning = 0;
        return nic_addr;
    }

    Link_Address resolve(const Network_Address& src, const Network_Address& dst, void* header)
    {
        Ant* ant = (Ant*) header;
        if (rtable.verify(src, ant->sequenceNO)) {
            rtable.update(src, ant->neighbor, ant->sequenceNO);
            switch ((int) ant->type) {
                case ITA:
                    return receiving_ITA(src, dst, ant);
                case ETA:
                    return receiving_ETA(src, dst, ant);
                default:
                    return Link_Layer::BROADCAST;
            }
        }
        return Link_Layer::BROADCAST;
    }

private:
    Link_Address receiving_ITA(const Network_Address& src, const Network_Address& dst, Ant* ant)
    {
        ant->neighbor = my_address;
        ant->heuristic = 0;
        Link_Address neighbor = rtable.search(dst);
        if (neighbor == Link_Layer::BROADCAST) {
            ant->type = ETA;
            ant->sequenceNO = seqNO++;
            return Link_Layer::BROADCAST;
        }
        return neighbor;
    }

    Link_Address receiving_ETA(const Network_Address& src, const Network_Address& dst, Ant* ant)
    {
        ant->neighbor = my_address;
        ant->heuristic = 0;
        return rtable.search(dst);
    }

    Link_Address my_address;
    RTable rtable;
    u16 seqNO:14;
};

__END_SYS

#endif /* ADHOP_H_ */
