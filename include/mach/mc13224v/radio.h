// EPOS-- MC13224V (Transceiver) NIC Mediator Declarations

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

#ifndef __mc13224v_radio_h
#define __mc13224v_radio_h

#include <radio.h>
#include <cmac.h>
#include "transceiver.h"

__BEGIN_SYS

class Radio_Wrapper
{
public:
    static const int FRAME_BUFFER_SIZE = MC13224V_Transceiver::MAX_PACKET_SIZE;

    enum {
        SFD_DETECTED,
        FRAME_END
    };

    typedef MC13224V_Transceiver::Event Event;
    typedef unsigned char Address;

    Radio_Wrapper() {}
    ~Radio_Wrapper() {}

    static void init();

    static void set_event_handler(MC13224V_Transceiver::event_handler * handler);

    int send(unsigned char * data, unsigned int size);

    int receive(unsigned char * data);

    void on();

    void off();

    void sleep();

    void listen();

    void reset();

    bool cca();

    unsigned int lqi();

    unsigned int rssi();

private:
    static MC13224V_Transceiver * device;
};

class MC13224V_Radio : public Radio_Common, CMAC<Radio_Wrapper>
{
public:
    typedef CMAC<Radio_Wrapper>::Address Address;
    typedef CMAC<Radio_Wrapper>::Statistics Statistics;

    MC13224V_Radio(int unit = 0): CMAC<Radio_Wrapper>(unit) {}

    ~MC13224V_Radio() {}

    static void init(unsigned int n) {}

    static void set_event_handler(CMAC<Radio_Wrapper>::event_handler * handler);

    int send(const Radio_Common::Address & dst, const Radio_Common::Protocol & prot,
            const void *data, unsigned int size) {
        return CMAC<Radio_Wrapper>::send(dst, prot, data, size);
    }

    int receive(Radio_Common::Address * src, Radio_Common::Protocol * prot,
            void * data, unsigned int size) {
        return CMAC<Radio_Wrapper>::receive(src, prot, data, size);
    }

    void reset() {}

    unsigned int mtu() const {
        return Traits<CMAC<Radio_Wrapper> >::MTU;
    }

    const Address & address() { return CMAC<Radio_Wrapper>::address(); }

    const Statistics & statistics() { return CMAC<Radio_Wrapper>::statistics(); }
};

__END_SYS

#endif
