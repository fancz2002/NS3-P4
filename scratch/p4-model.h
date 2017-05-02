#ifndef NS4_NS4_MODEL_H_H
#define NS4_NS4_MODEL_H_H

#include <bm/bm_sim/switch.h>
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include <chrono>
#include <string>
using namespace bm;

struct ns3PacketAndPort{
	int port_num;
	ns3::Ptr<ns3::Packet> packet;
};

struct bm2PacketAndPort{
	int port_num;
	ns3::Ptr<bm::Packet> packet;
};



class P4Model : public bm::Switch{
public:


	struct ns3PacketAndPort * receivePacket(struct ns3PacketAndPort *ns3packet);


	int init(int argc, char *argv[]);
	P4Model(std::string jsonfilelocation);
private:

	struct bm2PacketAndPort * ns3tobmv2(struct ns3PacketAndPort *);
	struct ns3PacketAndPort * bmv2tons3(struct bm2PacketAndPort *);


    using clock = std::chrono::high_resolution_clock;
    bm::TargetParserBasic * argParser;

};




#endif
