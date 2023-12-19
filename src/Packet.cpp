#include <Packet.h>

FastCRC16 fastCRC;

Packet::Packet() {
#ifdef PACKET_BUFFER_SIZE
  cap = PACKET_BUFFER_SIZE;
  len = 0;
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif
}

Packet::Packet(
  const uint8_t* payload, uint16_t buffSize,
  bool isBigEndianCRC,
  const uint8_t* head, uint8_t headLen,
  const uint8_t* tail, uint8_t tailLen,
  CrcFunc crcF
) {
  this->crcFunc = crcF;
  this->isBigEndianCRC = isBigEndianCRC;
  if (head != nullptr) memcpy(this->head, head, headLen);
  if (tail != nullptr) memcpy(this->tail, tail, tailLen);

#ifdef PACKET_BUFFER_SIZE
  cap = PACKET_BUFFER_SIZE;
  len = 0;
  while (cap < buffSize + this->headLen + this->tailLen + 4) cap <<= 1;
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif

  len = buffSize + this->headLen + this->tailLen + 2;
  memcpy(data, this->head, this->headLen);
  memcpy(data + this->headLen, payload, buffSize);

  uint16_t crc = crcFunc(payload, buffSize);
  data[buffSize + this->headLen] = isBigEndianCRC ? highByte(crc) : lowByte(crc);
  data[buffSize + this->headLen + 1] = isBigEndianCRC ? lowByte(crc) : highByte(crc);
  memcpy(data + buffSize + this->headLen + 2, this->tail, this->tailLen);
}

Packet::~Packet() {
#ifdef PACKET_BUFFER_SIZE
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
#endif
}

uint16_t Packet::size() { return len; }

uint8_t Packet::operator[] (int i) {
  if (i < 0) return data[len + i];
  return data[i];
}

bool Packet::checkCRC() {
  int crcIndexEnd = len - tailLen;

  uint16_t crc = isBigEndianCRC
    ? makeWord(data[crcIndexEnd - 2], data[crcIndexEnd - 1])
    : makeWord(data[crcIndexEnd - 1], data[crcIndexEnd - 2]);

  return crcFunc(data + headLen, crcIndexEnd - headLen - 2) == crc;
}

Packet& Packet::insertPacket(
  const uint8_t* buf, uint16_t buffSize,
  bool isBigEndianCRC,
  const uint8_t* head, int headLen,
  const uint8_t* tail, int tailLen
) {
  this->isBigEndianCRC = isBigEndianCRC;
  if (head != nullptr) memcpy(this->head, head, headLen);
  if (tail != nullptr) memcpy(this->tail, tail, tailLen);
#ifdef PACKET_BUFFER_SIZE
  while (cap < buffSize + headLen + tailLen + 2)
    cap <<= 1;
  data = (uint8_t*)realloc(data, cap * sizeof(uint8_t));
#endif

  memcpy(data, buf, buffSize);
  len = buffSize;
  return *this;
}

Packet& Packet::fixCRC() {
  int crcIndexEnd = len - tailLen;
  uint16_t crc = crcFunc(data + headLen, crcIndexEnd - headLen - 2);

  data[crcIndexEnd - 1] = isBigEndianCRC ? lowByte(crc) : highByte(crc);
  data[crcIndexEnd - 2] = isBigEndianCRC ? highByte(crc) : lowByte(crc);
  return *this;
}

bool Packet::isValid() {
  return len > headLen + tailLen + 2 &&
    !memcmp(data, head, headLen) &&
    !memcmp(data + len - tailLen, tail, tailLen) &&
    checkCRC();
}

Packet& Packet::clear() {
  len = 0;
#ifdef PACKET_BUFFER_SIZE
  cap = PACKET_BUFFER_SIZE;
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif
  return *this;
}

Packet& Packet::operator= (Packet& a) {
  if (this == &a) return *this;
  insertPacket(a.data, a.size());
  return *this;
}

Packet& Packet::print() {
  for (uint16_t i = 0; i < len; ++i) {
    Serial.print(data[i] < 0x10 ? " 0" : " ");
    Serial.print(data[i], HEX);
  }
  Serial.print("\n");

  return *this;
}

uint8_t* Packet::getData() {
  return data;
}