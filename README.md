# adc-test-stm32f4-libopencm3
Test for ADC stm32f429

# How to

```
$ git submodule init ; git submodule update
# compile and install stlink. Reload udev and so on...
# compile libopencm3
# then
$ make
$ sudo stlink/build/Release/st-flash write led.bin 0x8000000
```

* Connect a USB UART in PORTA PIN9 (GND also is needed)
* Baudrate is 38400bps


