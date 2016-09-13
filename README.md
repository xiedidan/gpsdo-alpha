# gpsdo-alpha
a home-made GPSDO (GPS disciplined oscillator)  
  
[GPSDO](https://en.wikipedia.org/wiki/GPS_disciplined_oscillator) is useful for Audio and HAM DIYers. The goal of this project is to build a GPSDO prototype that anyone could build at home.  
# Structure
There are 3 software sub-projects in this project.  
FreqCounter and LCDBridge are CPLD projects written in verilog to count oscillator pulses and bridge SPI to LCD1602. They are created seperately for easier verification and will be merged into one unified project to burn into CPLD.  
GPSDOController is an Arduino project that talks with CPLD and commands DAC (to adjust OCXO freq).  
There's a hardware project for pcb design of the prototype. I'm not good at hardware, but it does work.  
# Status
For now, the prototype is working on my desk but it does need some more enhancements and adjustments.
