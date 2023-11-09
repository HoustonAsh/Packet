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


#define intToBCD(x) x + (x/10)*6
#define BCDtoInt(x) x - (x/16)*6

#define bytesToInt16(buf, startIndex) (((int16_t)buf[startIndex] << 8)  | (int16_t)buf[startIndex + 1])
#define intToByte(v, i) (uint8_t)(v >> 8*i & 0xff)

#ifndef PACKET_BUFFER_SIZE
#define PACKET_STACK_SIZE 128
#endif

#define HEAD_SIZE 8
#define TAIL_SIZE 8

extern FastCRC16 fastCRC;

class Packet {
private:
#ifdef PACKET_BUFFER_SIZE
  uint32_t cap = PACKET_BUFFER_SIZE;
#else
  uint32_t cap;
#endif
protected:
  typedef uint16_t(*CrcFunc)(const uint8_t* data, uint16_t datalen);
  CrcFunc crcFunc;
  uint16_t len;
  bool isBigEndianCRC;

#ifdef PACKET_BUFFER_SIZE
  uint8_t* data;
#else
  uint8_t data[PACKET_STACK_SIZE];
#endif

  uint8_t head[HEAD_SIZE] = { 0xAB, 0xCD };
  int headLen = 2;
  uint8_t tail[TAIL_SIZE] = { 0xDC, 0xBA };
  int tailLen = 2;

public:
  Packet();
  Packet(
    uint8_t* payload, uint16_t buffSize,
    bool isBigEndianCRC = false,
    uint8_t* head = nullptr, uint8_t headLen = 0,
    uint8_t* tail = nullptr, uint8_t tailLen = 0,
    CrcFunc crcF = [](const uint8_t* data, uint16_t datalen) { return fastCRC.kermit(data, datalen); }
  );

  Packet(
    uint8_t* payload, uint16_t buffSize,
    CrcFunc crcF,
    bool isBigEndianCRC = false,
    uint8_t* head = nullptr, uint8_t headLen = 0,
    uint8_t* tail = nullptr, uint8_t tailLen = 0
  ) : Packet(payload, buffSize, isBigEndianCRC, head, headLen, tail, tailLen, crcF) {}

  ~Packet();
  uint16_t size();

  uint8_t operator[] (int i);
  Packet& operator= (Packet& a);
  Packet& fixCRC();
  Packet& insertPacket(
    uint8_t* buf, uint16_t bufferSize,
    bool isBigEndianCRC = false,
    uint8_t* head = nullptr, int headLen = 0,
    uint8_t* tail = nullptr, int tailLen = 0
  );

  bool checkCRC();
  bool isValid();
  Packet& clear();

  Packet& print();

  uint8_t* getData();
};

#endif