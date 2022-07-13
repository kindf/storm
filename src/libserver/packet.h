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
// Ĭ�ϴ�С 10KB
#define DEFAULT_PACKET_BUFFER_SIZE	1024 * 10
#endif

class Packet : public Buffer {
public:
    Packet(SOCKET socket, uint32 dest = 0);
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

    char* GetBuffer() const;
    unsigned short GetDataLength() const;
    int GetMsgId() const;
    void FillData(unsigned int size);
    void ReAllocBuffer();
    SOCKET GetSocket() const;

    uint32 GetDest() const {
        return _dest;
    }

private:
    /* Proto::MsgId _msgId; */
    SOCKET _socket;
    uint32 _dest;
};
