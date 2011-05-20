// EPOS-- MC13224V Default Application

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

#include <alarm.h>
#include <machine.h>
#include <sensor.h>

__USING_SYS

typedef IO_Map<Machine> IO;

void wait() {
    for(unsigned int i = 0; i < 0x000fffff; i++);
}

void turn_leds_off() {
    unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
    *GPIO_BASE = *GPIO_BASE & ~(1 << 23) & ~(1 << 24) & ~(1 << 25);
}

void turn_on_led1() {
    unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
    *GPIO_BASE = 1 << 23;
}

void turn_on_led2() {
    unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
    *GPIO_BASE = 1 << 24;
}

void turn_on_led3() {
    unsigned int *GPIO_BASE = (unsigned int*)0x80000000;
    *GPIO_BASE = 1 << 25;
}

inline void gpio_pad_dir_set(volatile unsigned long long int data) {
    CPU::out32(IO::GPIO_PAD_DIR_SET0, data & 0xffffffff);
    CPU::out32(IO::GPIO_PAD_DIR_SET1, data >> 32);
}

Temperature_Sensor * temp;

#define PAYLOAD_LEN 17
void fill_packet(volatile MC13224V_Transceiver::packet_t *p) {
    static char c = 0;
    p->length = PAYLOAD_LEN;
    p->offset = 0;

    p->data[0] = 'T';
    p->data[1] = 'x';
    p->data[2] = ' ';
    p->data[3] = 't';
    p->data[4] = 'e';
    p->data[5] = 's';
    p->data[6] = 't';
    p->data[7] = ' ';
    p->data[8] = 'H';
    p->data[9] = 'E';
    p->data[10] = 'L';
    p->data[11] = 'L';
    p->data[12] = 'W';
    p->data[13] = 'O';
    p->data[14] = 'R';
    p->data[15] = temp->sample();
    p->data[16] = 'D';

    c++;
}

void print_packet(volatile MC13224V_Transceiver::packet_t *p) {
    OStream cout;

    if (p) {
        int i = 16;
        cout << "Temperatura: " << (int) p->data[i] << "C\n";
        cout << "\n";
    }
}

int sink() {
    OStream cout;
    cout << "Sink\n";
    turn_on_led3();

    MC13224V_Transceiver * t;
    volatile MC13224V_Transceiver::packet_t * p;

    MC13224V_Transceiver::maca_init();

    t->set_channel(0);
    t->set_power(0x12);

    while (true) {
        t->check_maca();

        if (p = t->rx_packet()) {
            print_packet(p);
            t->free_packet(p);
        }
    }

    return 0;
}

int source() {
    temp = new Temperature_Sensor();
    OStream cout;
    cout << "Source\n";
    turn_on_led1();

    MC13224V_Transceiver * t;
    volatile MC13224V_Transceiver::packet_t * p;

    MC13224V_Transceiver::maca_init();

    t->set_channel(0);
    t->set_power(0x12);

    while (true) {
        cout << temp->sample() << "\n";

        t->check_maca();

        p = t->get_free_packet();

        if (p) {
            fill_packet(p);

            t->tx_packet(p);
            cout << "tx\n";

            for (unsigned long i = 0; i <= 2000 * (Traits<Machine>::CLOCK / 1000UL); i++)
                for (unsigned long i = 0; i <= 2000 * (Traits<Machine>::CLOCK / 1000UL); i++);
        }
    }

    return 0;
}

int main() {
    turn_on_led1();
    wait();
    turn_on_led2();
    wait();
    turn_on_led3();
    wait();
    turn_leds_off();
    wait();

    sink();
//    source();
}
