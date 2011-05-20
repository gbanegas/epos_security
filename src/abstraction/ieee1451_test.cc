#include <machine.h>

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.

__USING_SYS


#include <ieee1451_tim_temperature_sensor.h>

int tim()
{
    kout << "+++++ Iniciando wtim +++++\n";

    IEEE1451TemperatureSensor sensor(true, true);
    sensor.execute();
}


/*#include <ieee1451_ncap_application.h>

int ncap()
{
    kout << "+++++ Iniciando ncap +++++\n";

    NCAPApplication::getInstance();

    Thread::self()->suspend();
    //while(1);
}*/


int main()
{
    tim();
    //ncap();
}
