#include <Packet.h>

Packet::Packet() {
#ifdef HEAP_PACKET
  cap = BUFFER_INIT_SIZE;
  len = 0;
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif
}

Packet::Packet(uint8_t* payload, uint16_t buffSize) {
  if (buffSize == 0) {
    Packet();
    return;
  }

#ifdef HEAP_PACKET
  cap = BUFFER_INIT_SIZE;
  len = 0;
  while (cap < buffSize + 8)
    cap <<= 1;
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif

  data[0] = HEAD1;
  data[1] = HEAD2;

  FastCRC16 CRC16;
  uint16_t crc = CRC16.kermit(payload, buffSize);

  memcpy(data + 2, payload, buffSize);

  data[buffSize + 2] = highByte(crc);
  data[buffSize + 3] = lowByte(crc);

  data[buffSize + 4] = FOOT1;
  data[buffSize + 5] = FOOT2;

  len = buffSize + 6;
}

Packet::~Packet() {
#ifdef HEAP_PACKET
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
#endif
}

uint16_t Packet::size() { return len; }

uint8_t Packet::operator[] (int i) {
  if (i < 0)
    return data[len + i];
  return data[i];
}

bool Packet::checkCRC() {
  uint16_t CRCchecked = CRC16.kermit(data + 2, len - 6);
  uint16_t CRCreceived = makeWord(data[len - 4], data[len - 3]);
  return CRCchecked == CRCreceived;
}

void Packet::insertPacket(uint8_t* buf, uint16_t buffSize) {
#ifdef HEAP_PACKET
  while (cap < buffSize + 8)
    cap <<= 1;
  data = (uint8_t*)realloc(data, cap * sizeof(uint8_t));
#endif

  memcpy(data, buf, buffSize);
  len = buffSize;
}

void Packet::fixCRC() {
  FastCRC16 CRC16;
  uint16_t crc = CRC16.kermit(data + 2, len - 6);

  data[len - 3] = lowByte(crc);
  data[len - 4] = highByte(crc);
}

bool Packet::isValid() {
  return len > 8 &&
    data[0] == HEAD1 && data[1] == HEAD2 &&
    data[len - 2] == FOOT1 && data[len - 1] == FOOT2 &&
    checkCRC();
}

void Packet::clear() {
  len = 0;
#ifdef HEAP_PACKET
  cap = BUFFER_INIT_SIZE;
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif
}

Packet& Packet::operator= (Packet& a) {
  if (this == &a) return *this;
#ifdef HEAP_PACKET
  clear();
#endif
  insertPacket(a.data, a.size());
  return *this;
}

void Packet::print() {
  for (uint16_t i = 0; i < len; ++i) {
    Serial.print(data[i] < 0x10 ? " 0" : " ");
    Serial.print(data[i], HEX);
  }
  Serial.print("\n");
}

uint8_t* Packet::getData() {
  return data;
}