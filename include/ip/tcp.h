#ifndef __tcp_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __tcp_h
#include <alarm.h>
#include <ip/ip.h>
#include <ip/udp.h> // TCP::Address == UDP_Address
#include <utility/handler.h>
#include <utility/random.h>

__BEGIN_SYS

class TCP: public IP::Observer, public Data_Observed < UDP_Address > {
public:
    typedef UDP_Address Address;

    static const unsigned int ID_TCP = 6;

    class Header;
    class Socket;
    class ServerSocket;
    class ClientSocket;

    TCP(IP * ip = 0);
    ~TCP();
    
    IP * ip() { return _ip; }
    
    void update(Data_Observed<IP::Address>*, long, IP::Address,
                IP::Address, void*, unsigned int);
                
protected:

    IP * _ip;
};

class TCP::Header {
    friend class TCP;
    
 public:
    Header(u32 s = 0, u32 a = 0);

    u16 _src_port, _dst_port;
    u32 _seq_num, _ack_num;
#if !defined(i386) && !defined(AVR) && !defined(ARM)
    u8  _hdr_off:4, _un1:4;
    bool  _un3:1, _un2:1;
    bool  _urg:1, _ack:1, _psh:1, _rst:1, _syn:1, _fin:1;
#else
    u8  _un1:4, _hdr_off:4;
    bool  _fin:1, _syn:1, _rst:1, _psh:1, _ack:1, _urg:1;
    bool  _un2:1, _un3:1;
#endif
    u16 _wnd;
    volatile u16 _chksum;
    u16 _urgptr;

    // getters
    u16 size() const { return _hdr_off * 4; }
    u16 dst_port() const { return CPU::ntohs(_dst_port); }
    u16 src_port() const { return CPU::ntohs(_src_port); }
    u32 seq_num() const { return CPU::ntohl(_seq_num); }
    u32 ack_num() const { return CPU::ntohl(_ack_num); }
    u16 wnd() const { return CPU::ntohs(_wnd); }
    u16 chksum() const { return CPU::ntohs(_chksum); }
    u16 urgptr() const { return CPU::ntohs(_urgptr); }
    
    // setters
    void seq_num(u32 v) { _seq_num = CPU::htonl(v); }
    void ack_num(u32 v) { _ack_num = CPU::htonl(v); }
    void wnd(u16 v) { _wnd = CPU::htons(v); }
    void chksum(u16 v) { _chksum = CPU::htons(v); }
    void urgptr(u16 v) { _urgptr = CPU::htons(v); }
    void dst_port(u16 v) { _dst_port = CPU::htons(v); }
    void src_port(u16 v) { _src_port = CPU::htons(v); }
    
    u16 _checksum(IP::Address &src,IP::Address &dst,u16 len);
    void _checksum(IP::Address &src,IP::Address &dst,SegmentedBuffer * sb);
    void checksum(IP::Address &src,IP::Address &dst,u16 len);
    bool validate_checksum(IP::Address &src,IP::Address &dst,u16 len);

    // Ultimate unreadable coding style!
    friend Debug& operator<< (Debug & db, const Header& s)
    {
    db << "Header[SRC="<<s.src_port()<<",DST="<<s.dst_port()<<"] SEQ="<<
    s.seq_num()<<",ACK="<<s.ack_num()<<",off="<<s._hdr_off<<
    " CTL=["<<(s._urg ? "U" : "") <<(s._ack ? "A" : "") <<
    (s._psh ? "P" : "") <<(s._rst ? "R" : "") <<(s._syn ? "S" : "") <<
    (s._fin ? "F" : "") <<"],wnd="<<s.wnd()<<",chk="<<s.chksum()<<"]\n";
    return db;
    }

    // Pseudo header for checksum calculations
    struct Pseudo_Header {
        u32 src_ip, dst_ip;
        u8 zero, protocol;
        u16 length;
        
        Pseudo_Header(u32 src,u32 dst,u16 len) 
        : src_ip(src), dst_ip(dst), zero(0), protocol(ID_TCP),
          length(CPU::htons(len)) {};
    };
// Compact bitfields istead of using 1 char for each single bit attribute
} __attribute__((packed)); 

class TCP::Socket : public Data_Observer<TCP::Address>, public Handler {
    friend class TCP;
 public:
    typedef void (Socket::* Handler)(const Header&,const char*,u16);
    
    enum { // Erros
        ERR_NOT_CONNECTED,
        ERR_TIMEOUT,
        ERR_RESET,
        ERR_CLOSING,
        ERR_NO_ROUTE,
        ERR_NOT_STARTED,
        ERR_REFUSED
    };

    Socket(const Socket& socket);    
    Socket(TCP * tcp,const Address &local,const Address &remote);
    virtual ~Socket();
    
    // Data_Observer callback
    void update(Data_Observed<TCP::Address> *ob, long c, TCP::Address src,
            TCP::Address dst, void *data, unsigned int size);
            
    //* Called when connection handshake is complete
    virtual void connected() {};
    
    //* Called when data arrives
    virtual void received(const char* data,u16 size) {};
    
    //* Called when data was sucessfuly sent
    virtual void sent(u16 size) {};
    
    //* Called when there is an error
    virtual void error(short errorcode) {};
    
    //* Called when the connection is closed
    // Default action deletes the Socket object
    virtual void closed() { delete this; };
    
    //* Called when the peer closed his side of the connection
    virtual void closing() {} 
    
    void close();
    void send(const char* data,u16 len);
    void send(SegmentedBuffer * sb);
    
    const Address & remote() { return _remote; }
    const Address & local()  { return _local; }
    
    enum { // Connection state
        LISTEN,     SYN_SENT,
        SYN_RCVD,   ESTABLISHED,
        FIN_WAIT1,  FIN_WAIT2,
        CLOSE_WAIT, CLOSING,
        LAST_ACK,   TIME_WAIT,
        CLOSED,
    };
    
    u8 state() { return _state; }
    void state(u8 s) { _state = s; state_handler = handlers[s]; }

    // state-processing functions
    void __LISTEN(const Header&,const char*,u16);
    void __SYN_SENT(const Header&,const char*,u16);
    void __SYN_RCVD(const Header&,const char*,u16);
    void __ESTABLISHED(const Header&,const char*,u16);
    void __FIN_WAIT1(const Header&,const char*,u16);
    void __FIN_WAIT2(const Header&,const char*,u16);
    void __CLOSE_WAIT(const Header&,const char*,u16);
    void __CLOSING(const Header&,const char*,u16);
    void __LAST_ACK(const Header&,const char*,u16);
    void __TIME_WAIT(const Header&,const char*,u16);
    void __CLOSED(const Header&,const char*,u16);
    void __SNDING(const Header&,const char*,u16);
    void __RCVING(const Header&,const char*,u16);
    
    Handler state_handler;

    void operator()();
 protected: 
    // methods
    bool check_seq(const Header &h,u16 len);
    void send_ack();
    void send_fin();
    s32 _send(Header * hdr, SegmentedBuffer * sb);
    void set_timeout();
    void clear_timeout();
    
    // attributes
    TCP * _tcp;
    TCP::Address _remote;
    TCP::Address _local;
    volatile u8 _state;
    volatile u32 _rtt;
    
    volatile u32 snd_una, snd_nxt, snd_ini, snd_wnd;
    volatile u32 rcv_una, rcv_nxt, rcv_ini, rcv_wnd;

    Alarm * _timeout;

    // class attributes
    static Handler handlers[13];
};

class TCP::ClientSocket : public Socket {
 public:
    ClientSocket(TCP * tcp,const Address &remote,const Address &local);
    
    void reconnect() { connect(_remote); }
    void connect(const Address& to);
};

class TCP::ServerSocket : public Socket {
 public:
    ServerSocket(TCP * tcp,const Address &local);
    ServerSocket(const TCP::ServerSocket &socket);
    
    void listen();
    
    //* Called to notify an incoming connection
    //* Should return a copy of itself to accept the connection
    virtual Socket* incoming(const Address& from) = 0;
    
};

__END_SYS
#endif

