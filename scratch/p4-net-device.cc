#include "p4-net-device.h"
#include "p4-model.h"
#include <bm/bm_sim/switch.h>

void P4NetDevice::ReceiveFromDevice(Ptr<NetDevice> device, Ptr<const ns3::Packet> packet, uint16_t protocol,
        Address const &source, Address const &destination, PacketType packetType){
	int port_num = GetPortNumber(device);
	struct ns3PacketAndPort *ns3packet = new (struct ns3PacketAndPort);
	ns3packet->port_num = port_num;
	ns3packet->packet = packet;
	struct ns3PacketAndPort * egress_packetandport = p4Model->receivePacket(ns3packet);
	Ptr<ns3::Packet> egress_packet = egress_packetandport->packet;
	int egress_port_num = egress_packetandport->port_num;
	Ptr<NetDevice>outNetDevice = &m_ports[egress_port_num];
	int result = outNetDevice->Send(packet,*CreateObject<Address>(),0);
}


int P4NetDevice::GetPortNumber(Ptr<NetDevice> port){
	auto it = std::find(m_ports.begin(), m_ports.end(), port);
	auto index = std::distance(m_ports.begin(), it);
}


P4NetDevice::P4NetDevice(){
	p4Model = new P4Model;
	p4Model->init();
	}
