#pragma once

#include "base_buffer.h"
#include "common.h"

#pragma pack(push)
#pragma pack(4)

struct PacketHead {
    unsigned short MsgId;
};

#pragma pack(pop)

#if TestNetwork
#define DEFAULT_PACKET_BUFFER_SIZE	10
#else
// Ä¬ÈÏ´óÐ¡ 10KB
#define DEFAULT_PACKET_BUFFER_SIZE	1024 * 10
#endif

class Packet : public Buffer {
public:
    Packet(SOCKET socket, unsigned short msgid = 0);
    ~Packet();

    /* template<class ProtoClass> */
    /* ProtoClass ParseToProto() */
    /* { */
    /*     ProtoClass proto; */
    /*     proto.ParsePartialFromArray(GetBuffer(), GetDataLength()); */
    /*     return proto; */
    /* } */

    /* template<class ProtoClass> */
    /* void SerializeToBuffer(ProtoClass& protoClase) */
    /* { */
    /*     auto total = (unsigned int)protoClase.ByteSizeLong(); */
    /*     while (GetEmptySize() < total) */
    /*     { */
    /*         ReAllocBuffer(); */
    /*     } */

    /*     protoClase.SerializePartialToArray(GetBuffer(), total); */
    /*     FillData(total); */
    /* } */

    void BackToPool();
    void CleanBuffer();

    unsigned short GetDataLength() const;
    void FillData(unsigned int size);
    void ReAllocBuffer();

    char* GetBuffer() const {
        return _buffer;
    }

    SOCKET GetSocket() const {
        return _socket;
    }

    unsigned short GetMsgId() const {
        return _msgId;
    }

private:
    unsigned short _msgId;
    SOCKET _socket;
};
