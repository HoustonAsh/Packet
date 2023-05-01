# Packet

A simple packet management library for Arduino and other embedded platforms designed for various communication protocols like Serial, Radio Signal, and LoRa.

## Usage

- CRC validation and fix:

  ```cpp
  Packet p;
  ...
  if (p.isValid())
    p.fixCRC();
  ```

</br>

- Choose where allocate memory:
  
  By default, memory for data will be allocated on the heap. If you want to use stack memory (for any reason), add this line to your platformio.ini file:

  ```ts
  build_flags = -DPACKET_STACK_SIZE=128
  ```

  Or if you use ArduinoIDE define PACKET_STACK_SIZE <u>**before**</u> including Packet.h:

  ```cpp
  #define PACKET_STACK_SIZE 128
  #include <Packet.h>
  ```

</br>

- Note that operator '=' creates a copy of right operand