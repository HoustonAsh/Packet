# Packet

A simple packet management library for Arduino and other embedded platforms designed for various communication protocols.

## Usage

- In general:
  ```cpp
  const uint8_t buffSize = 16
  uint8_t buff[buffSize] = {...}  
  Packet p(buff, buffSize);

  //if you need custom head and tail:
  uint8_t h[2] = {0x14, 0xBB};
  uint8_t t[2] = {0x88, 0xA1};
  Packet p(buff, buffSize, false, h, 2, t, 2);

  //if you need to create Packet instance from buffer:
  p.insertPacket(buff, buffSize, false, h, 2, t, 2)
   .fixCRC()
   .print();

  if (p.isValid()) {
    ...
  }
  ```

- By default, memory for data will be allocated on the stack. If you want to use heap memory, add this line to your platformio.ini file:

  ```ts
  build_flags = -DPACKET_BUFFER_SIZE=128
  ```

  Or if you use ArduinoIDE define PACKET_BUFFER_SIZE <u>**before**</u> including Packet.h:

  ```cpp
  #define PACKET_BUFFER_SIZE 128
  #include <Packet.h>
  ```


- Note: operator '=' creates a copy