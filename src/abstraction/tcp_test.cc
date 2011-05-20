#include <ip/tcp.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#include <utility/string.h>

__USING_SYS

OStream cout;

class HTTPServer : public TCP::ServerSocket {
public:
    HTTPServer(TCP * tcp) : TCP::ServerSocket(tcp,TCP::Address(tcp->ip()->address(),80)) {}

    TCP::Socket* incoming(const TCP::Address& from) {
        // we can clone here to accept multiple connections
        // or just return itself
        return this;
    }

    void connected() {
        cout << "Connection from " << remote() << endl;
    }

    void closed() {
        cout << "Disconnected from " << remote() << endl;
        
        listen();
    }

    void error(short err) {
        cout << "Connection error" << endl;
    }

    void sent(u16 size) {} 

    void received(const char *data,u16 size) {
        cout << "Received "<<size<<" bytes: " << data << endl;
        
        const char * msg = "200 HTTP 1.1\r\n\r\nHello world!\r\n";
        send(msg, 30);
        close();
    }
}; 

int main()
{
    IP ip(0);
    TCP tcp(&ip);

    ip.set_address(IP::Address(10,0,2,15));
    ip.set_gateway(IP::Address(10,0,2,2));
    ip.set_netmask(IP::Address(255,255,255,0));

    HTTPServer httpd(&tcp);

    Thread::self()->suspend();
}
