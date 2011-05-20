// EPOS ATMega1281 Radio (AT86RF230) NIC Mediator Declarations

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

#ifndef __at86rf230_nic_h
#define __at86rf230_nic_h

#include "at86rf230.h"
#include <radio.h>
#include <cmac.h>

__BEGIN_SYS

class Radio_Wrapper
{
public:
    static const int FRAME_BUFFER_SIZE = AT86RF230::FRAME_MAX_LENGTH;

    enum {
	SFD_DETECTED,
	FRAME_END
    };

    typedef AT86RF230::Event Event;
    typedef unsigned char Address;

    Radio_Wrapper() {}
    ~Radio_Wrapper() {}

    static void init();

    static void set_event_handler(AT86RF230::event_handler * handler);

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
    static AT86RF230 * device;
};

class ATMega1281_Radio: public Radio_Common, CMAC<Radio_Wrapper>
{
public:
    typedef CMAC<Radio_Wrapper>::Address Address;
    typedef CMAC<Radio_Wrapper>::Statistics Statistics;

    ATMega1281_Radio(int unit = 0): CMAC<Radio_Wrapper>(unit) {}

    ~ATMega1281_Radio() {}

    static void init(unsigned int n) {}

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

