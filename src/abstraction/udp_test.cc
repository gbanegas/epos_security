#include <ip/dhcp.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <alarm.h>


__USING_SYS

OStream cout;

int main()
{
    IP ip(0);

    ip.set_address(IP::Address(0,0,0,0));
    ip.set_gateway(IP::Address(0,0,0,0));
    ip.set_netmask(IP::Address(0,0,0,0));

    UDP udp(&ip);
    DHCP::Client dhcpc(&udp);
    dhcpc.configure();

    Alarm::delay(5000000);

    cout << "IP Addr: " << dhcpc.address() << endl;
    cout << "Netmask: " << dhcpc.netmask() << endl;
    cout << "Gateway: " << dhcpc.gateway() << endl;
    cout << "DNS:     " << dhcpc.nameserver() << endl;
}
