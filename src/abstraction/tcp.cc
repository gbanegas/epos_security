#include <ip/tcp.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__BEGIN_SYS

// static data
TCP::Socket::Handler TCP::Socket::handlers[13] = { 
    &TCP::Socket::__LISTEN,     &TCP::Socket::__SYN_SENT,
    &TCP::Socket::__SYN_RCVD,   &TCP::Socket::__ESTABLISHED,
    &TCP::Socket::__FIN_WAIT1,  &TCP::Socket::__FIN_WAIT2,
    &TCP::Socket::__CLOSE_WAIT, &TCP::Socket::__CLOSING,
    &TCP::Socket::__LAST_ACK,   &TCP::Socket::__TIME_WAIT,
    &TCP::Socket::__CLOSED };

TCP::TCP(IP * ip) : _ip(ip)
{
    if (!ip)
        _ip = IP::instance();
    _ip->attach(this, ID_TCP);
}

TCP::~TCP()
{
    _ip->detach(this, ID_TCP);
}

// Called by IP's notify(...)

void TCP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
                 IP::Address dst, void *data, unsigned int size)
{
    Header& hdr = *reinterpret_cast<Header*>(data);

    db<TCP>(TRC) << "TCP::update: "<< hdr << endl;

    if (!(hdr.validate_checksum(src,dst,size - hdr.size()))) {
        db<TCP>(INF) << "TCP checksum failed for incoming packet!\n";
        return;
    }

    int len = size - hdr.size();
    if (len < 0) {
        db<TCP>(INF) << "Misformed TCP segment received\n";
        return;
    }

    notify(TCP::Address(src,hdr.src_port()),
           TCP::Address(dst,hdr.dst_port()),
           (int) hdr.dst_port(), data, size);
}


// Called by TCP's notify(...)

void TCP::Socket::update(Data_Observed<TCP::Address> *o, long c, TCP::Address src,
                         TCP::Address dst, void *data, unsigned int size)
{
    Header& hdr = *reinterpret_cast<Header*>(data);
    int len = size - hdr.size();

    if ((_remote == src) || (_remote.port() == 0))
    {
        if (state() == LISTEN) _remote = src;
        (this->*state_handler)(hdr,&((char*)data)[hdr.size()],len);
    } else {
        db<TCP>(TRC) << "TCP Segment does not belong to us\n";          
    }
}

// Header stuff

TCP::Header::Header(u32 seq,u32 ack)
{
    memset(this,0,sizeof(Header));
    seq_num(seq);
    ack_num(ack);
}

bool TCP::Header::validate_checksum(IP::Address &src,IP::Address &dst,u16 len)
{
    db<IP>(TRC) << __PRETTY_FUNCTION__ << endl;
    len += size();
    
    Pseudo_Header phdr((u32)src,(u32)dst,len);

    u32 sum = 0;

    sum = IP::calculate_checksum(this, len);
    sum += IP::calculate_checksum(&phdr, sizeof(phdr));
    
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return sum == 0xFFFF;
}

void TCP::Header::_checksum(IP::Address &src,IP::Address &dst,SegmentedBuffer * sb)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    u16 len;
    len = size();

    if (sb) len += sb->total_size();

    Pseudo_Header phdr((u32)src,(u32)dst,len);

    _chksum = 0;

    u32 sum = 0;

    sum = IP::calculate_checksum(&phdr, sizeof(phdr));
    sum += IP::calculate_checksum(this, size());

    while (sb) {
        sum += IP::calculate_checksum(sb->data(), sb->size());
        sb = sb->next();
    }
    
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    _chksum = ~sum;
}

// Socket stuff

TCP::Socket::Socket(TCP * tcp,const Address &remote,const Address &local)
    : _tcp(tcp), _remote(remote), _local(local), _rtt(5000000), _timeout(0)
{
    state(CLOSED);
    _tcp->attach(this, _local.port());
}

TCP::Socket::Socket(const TCP::Socket& socket)
    : _tcp(socket._tcp), _remote(socket._remote), _local(socket._local), _rtt(5000000), _timeout(0)
{
    state(CLOSED);
    _tcp->attach(this, _local.port());
}

TCP::Socket::~Socket()
{
    _tcp->detach(this, _local.port());
}

s32 TCP::Socket::_send(Header * hdr, SegmentedBuffer * sb)
{
    // fill header
    hdr->src_port(_local.port());
    hdr->dst_port(_remote.port());
    hdr->_hdr_off = 5; // our header is always 20 bytes
    hdr->wnd(rcv_wnd);
    hdr->chksum(0);
    hdr->_checksum(_local,_remote,sb);

    // hdr + sb
    SegmentedBuffer nsb(hdr,hdr->size());
    nsb.append(sb);


    return _tcp->ip()->send(_local,_remote,&nsb,TCP::ID_TCP) - hdr->size();
}

void TCP::Socket::send(const char *data,u16 len)
{
    Header hdr(snd_nxt,rcv_nxt-1);
    hdr._ack = true;
    snd_nxt += len;
    SegmentedBuffer sb(data,len);
    _send(&hdr,&sb);
}

void TCP::Socket::set_timeout() {
    //TODO: Not working!
    //if (_timeout) delete _timeout;
    //_timeout = new Alarm(2 * _rtt, this, 1);
}

void TCP::Socket::clear_timeout() {
    if (_timeout) delete _timeout;
}

void TCP::Socket::operator()() {
    // timeout occured

    error(ERR_TIMEOUT);
}

void TCP::Socket::close()
{
    send_fin();
    if (state() == ESTABLISHED)
        state(FIN_WAIT1);
    else if (state() == CLOSE_WAIT) {
        state(LAST_ACK);    
    }
    else if (state() == SYN_SENT) {
        state(CLOSED);
        closed();    
    }
}


TCP::ClientSocket::ClientSocket(TCP * tcp,const Address& remote,const Address& local) 
	: Socket(tcp,remote,local)
{
    connect(remote);
}

void TCP::ClientSocket::connect(const Address& to) {
    _remote = to;
    
    state(SYN_SENT);
    snd_ini = Pseudo_Random::random() & 0x00FFFFFF;
    snd_una = snd_ini;
    snd_nxt = snd_ini + 1;
    rcv_wnd = 512; // TODO: change to MTU minus headers

    Header hdr(snd_ini, 0);
    hdr._syn = true;
    _send(&hdr,0);
    set_timeout();
}

TCP::ServerSocket::ServerSocket(TCP * tcp,const Address& local) 
    : Socket(tcp,Address(0,0),local)
{
    listen();
    rcv_wnd = 1024; // TODO: change to MTU minus headers
}

TCP::ServerSocket::ServerSocket(const TCP::ServerSocket &socket) : Socket(socket)
{
    rcv_wnd = 1024; // TODO: change to MTU minus headers };
}

void TCP::ServerSocket::listen()
{
    _remote = Address(0,0);
    state(LISTEN);
}

void TCP::Socket::__LISTEN(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

    if (r._syn && !r._rst && !r._fin) {
        Socket * n;
        ServerSocket *ss = static_cast<ServerSocket*>(this);
        if (!ss) {
            db<TCP>(ERR) << "TCP::Non-ServerSocket in LISTEN state!\n";
            Machine::panic();
        }
        if (ss && (n = ss->incoming(_remote))) {
            n->rcv_nxt = r.seq_num()+1;
            n->rcv_ini = r.seq_num();
            n->snd_wnd = r.wnd();

            n->state(SYN_RCVD);

            n->snd_ini = Pseudo_Random::random() & 0x0000FFFF;

            Header s(n->snd_ini,n->rcv_nxt);
            s._syn = true;
            s._ack = true;
            n->_send(&s,0);
        
            n->snd_nxt = n->snd_ini+1;
            n->snd_una = n->snd_ini;

            n->set_timeout();
        }
    } 
    if (state() == LISTEN) {
        // a new socket was created to handle the incomming connection
        // and we stay in the listening state
        _remote = Address((u32)0,(u16)0);
    }
}


void TCP::Socket::__SYN_SENT(const Header& r,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

    if (r._rst || r._fin) {
            error(ERR_REFUSED);
            state(CLOSED);
            closed();
    }
    else if (r._ack) {
        if ((r.ack_num() <= snd_ini) || (r.ack_num() > snd_nxt)) {
            error(ERR_RESET);
            state(CLOSED);
            closed();
        } else if ((r.ack_num() >= snd_una) && (r.ack_num() <= snd_nxt)) {
            if (r._syn) {
                rcv_nxt = r.seq_num() + 1;
                rcv_ini = r.seq_num();
                snd_una = r.ack_num();
                snd_wnd = r.wnd();
                if (snd_una <= snd_ini) {
                    state(SYN_RCVD);
                } else {
                    state(ESTABLISHED);
                    send_ack();
                    connected();
                }
            } else {
                // TODO: discover what to do here
            }
        }
    } else if (!r._rst && r._syn) {
        rcv_nxt = r.seq_num() + 1;
        snd_ini = r.seq_num();
        snd_wnd = r.wnd();
        state(SYN_RCVD);
    }

}

void TCP::Socket::__SYN_RCVD(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

    if (!check_seq(r,len)) 
        return;

    if (r._rst || r._fin) {
        error(ERR_RESET);
        state(CLOSED);
        closed();
    }
    else if (r._ack) {
        snd_wnd = r.wnd();
        state(ESTABLISHED);
        connected();
    }
}

void TCP::Socket::__RCVING(const Header &r,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (len) {
        rcv_nxt += len - 1;
        send_ack();
        ++rcv_nxt;
        received(data,len);
    } else {
        send_ack();
    }
}

void TCP::Socket::__SNDING(const Header &r,const char* data, u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

    if (r._ack) {
        //TODO: notify how many bytes were sent
        sent(0);
    }
}

void TCP::Socket::__ESTABLISHED(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

    if (!check_seq(r,len))
        return;

    if (r._rst) {
        error(ERR_RESET);
        state(CLOSED);
        closed();
    }
    else if (r.seq_num() == rcv_nxt) { // implicit reject out-of-order segments
        snd_wnd = r.wnd();

        if (len)
            __RCVING(r,data,len);

        else
            __SNDING(r,data,len);

        if (r._fin) {
            send_ack();
            state(CLOSE_WAIT);
            closing();
        }
    }
    else {
        db<TCP>(TRC) << "TCP::out of order segment received\n";
    }
}


void TCP::Socket::__FIN_WAIT1(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

    if (!check_seq(r,len))
        return;

    if (r._ack && !r._fin) { // TODO: check snd_una
        rcv_nxt = r.seq_num() + len;
        state(FIN_WAIT2);
        send_ack();
    }
    if (r._ack && r._fin) {
        state(CLOSED); // no TIME_WAIT
        send_ack();
        closed();
    }
    if (!r._ack && r._fin) {
        state(CLOSING);
        send_ack();
    }
}

void TCP::Socket::__FIN_WAIT2(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (!check_seq(r,len))
        return;
    
    if (r._fin) {
        state(CLOSED); // no TIME_WAIT
        send_ack();
        closed();
    }
}

void TCP::Socket::__CLOSE_WAIT(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (!check_seq(r,len))
        return;

    if (r._rst || len) {
        if (len)
            //TODO: send_reset()
        error(ERR_RESET);
        state(CLOSED);
        closed();
    } 
}

void TCP::Socket::__CLOSING(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (!check_seq(r,len))
        return;
    
    if (r._ack) {
        state(CLOSED); // no TIME_WAIT
        closed();
    }
}

void TCP::Socket::__LAST_ACK(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (!check_seq(r,len))
        return;
    if (r._ack) {
        state(CLOSED);
        closed();
    }
}

void TCP::Socket::__TIME_WAIT(const Header& r ,const char* data,u16 len)
{
    db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (!check_seq(r,len))
        return;
    
    if (r._fin && r._ack) {
        state(CLOSED);
        snd_nxt++; // ?
        closed();
    }
}

void TCP::Socket::__CLOSED(const Header&,const char*,u16)
{
    // does nothing
}

void TCP::Socket::send_ack()
{
    Header s(snd_nxt,rcv_nxt);
    s._ack = true;
    _send(&s,0);
}

void TCP::Socket::send_fin()
{
    Header s(snd_nxt,rcv_nxt);
    s._fin = true;
    s._ack = true;
    _send(&s,0);
}

bool TCP::Socket::check_seq(const Header &h,u16 len)
{
    if ((len <= rcv_wnd) &&
        (h.seq_num() == rcv_nxt))
    {
        return true;
    }

    db<TCP>(TRC) << "TCP: check_seq() == false\n";
    return false;
}

__END_SYS

