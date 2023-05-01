#include <Packet.h>

Packet::Packet() {
#if PACKET_STACK_SIZE == 0
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

#if PACKET_STACK_SIZE == 0
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

#if PACKET_STACK_SIZE == 0
  for (size_t i = 0; i < buffSize; ++i)
#else
  for (size_t i = 0; i < min(buffSize, (uint16_t)PACKET_STACK_SIZE); ++i)
#endif
    data[i + 2] = payload[i];

  data[buffSize + 2] = highByte(crc);
  data[buffSize + 3] = lowByte(crc);

  data[buffSize + 4] = FOOT1;
  data[buffSize + 5] = FOOT2;

  len = buffSize + 8;
}

Packet::~Packet() {
#if PACKET_STACK_SIZE == 0
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
#endif
}

uint16_t Packet::size() { return len; }

uint8_t* Packet::operator[] (int i) {
  if (i >= len || i < -len)
    return nullptr;

  if (i < 0)
    return &data[len + i];

  return &data[i];
}

bool Packet::checkCRC() {
  uint8_t* temp = (uint8_t*)malloc(len * sizeof(uint8_t));

  for (uint8_t i = 2; i < len - 3; ++i)
    temp[i - 2] = data[i];

  uint16_t CRCchecked = CRC16.kermit(temp, len - 6);
  uint16_t CRCreceived = makeWord(data[len - 3], data[len - 4]);
  free(temp);
  return CRCchecked == CRCreceived;
}

void Packet::insertPacket(uint8_t* buf, uint16_t buffSize) {
#if PACKET_STACK_SIZE == 0
  while (cap < buffSize + 8)
    cap <<= 1;
  data = (uint8_t*)realloc(data, cap * sizeof(uint8_t));
  for (uint16_t i = 0; i < buffSize; ++i)
#else
  for (uint16_t i = 0; i < min(buffSize, (uint16_t)PACKET_STACK_SIZE); ++i)
#endif
    data[i] = buf[i];
  len = buffSize;
}

void Packet::fixCRC() {
  uint8_t* tmp = (uint8_t*)malloc(len * sizeof(uint8_t));
  for (uint16_t i = 2; i < len; ++i)
    tmp[i - 2] = data[i];

  FastCRC16 CRC16;
  uint16_t crc = CRC16.kermit(tmp, len - 6);

  data[len - 3] = highByte(crc);
  data[len - 4] = lowByte(crc);
  free(tmp);
}

bool Packet::isValid() {
  return len > 8 &&
    data[0] == HEAD1 && data[1] == HEAD2 &&
    data[len - 2] == FOOT1 && data[len - 2] == FOOT2 &&
    checkCRC();
}

void Packet::clear() {
  len = 0;
#if PACKET_STACK_SIZE == 0
  cap = BUFFER_INIT_SIZE;
  if (data != nullptr) {
    free(data);
    data = nullptr;
  }
  data = (uint8_t*)malloc(cap * sizeof(uint8_t));
#endif
}

Packet Packet::operator= (Packet a) {
  clear();
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