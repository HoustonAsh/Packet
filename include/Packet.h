#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>
#include <FastCRC.h>

#define bytesToInt64(buf, startIndex) \
(((int64_t)buf[startIndex] << 56LL) | \
 ((int64_t)buf[startIndex + 1] << 48LL) | \
 ((int64_t)buf[startIndex + 2] << 40LL) | \
 ((int64_t)buf[startIndex + 3] << 32LL) | \
 ((int64_t)buf[startIndex + 4] << 24LL) | \
 ((int64_t)buf[startIndex + 5] << 16LL) | \
 ((int64_t)buf[startIndex + 6] << 8LL)  | \
  (int64_t)buf[startIndex + 7])

#define bytesToInt32(buf, startIndex) \
(((int32_t)buf[startIndex] << 24) | \
 ((int32_t)buf[startIndex + 1] << 16) | \
 ((int32_t)buf[startIndex + 2] << 8)  | \
  (int32_t)buf[startIndex + 3])


#define bytesToInt16(buf, startIndex) (((int16_t)buf[startIndex] << 8)  | (int16_t)buf[startIndex + 1])
#define intToByte(v, i) (uint8_t)(v >> 8*i & 0xff)


#if PACKET_STACK_SIZE == 0
#define BUFFER_INIT_SIZE 32
#endif
#define HEAD1 0xAB
#define HEAD2 0xCD

#define FOOT1 0xDC
#define FOOT2 0xBA

class Packet {
private:
#if PACKET_STACK_SIZE == 0
  uint32_t cap = BUFFER_INIT_SIZE;
#else
  uint32_t cap;
#endif
protected:
  FastCRC16 CRC16;
  uint16_t len;

#if PACKET_STACK_SIZE == 0
  uint8_t* data;
#else
  uint8_t data[PACKET_STACK_SIZE];
#endif

public:
  Packet();
  Packet(uint8_t* payload, uint16_t buffSize);
  ~Packet();
  uint16_t size();

  uint8_t operator[] (int i);
  Packet operator= (Packet a);
  void fixCRC();
  void insertPacket(uint8_t* buf, uint16_t bufferSize);
  bool checkCRC();
  bool isValid();
  void clear();

  void print();

  uint8_t* getData();
};

#endif