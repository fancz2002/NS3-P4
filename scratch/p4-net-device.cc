#include "p4-net-device.h"


void P4NetDevice::ReceiveFromDevice(Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
        Address const &source, Address const &destination, PacketType packetType){
	int port_num = GetPortNumber(device);
	struct ns3PacketAndPort *ns3packet = new struct ns3PacketAndPort;
	ns3packet->port_num = port_num;
	ns3packet->packet = packet;
	struct ns3PacketAndPort * egress_packetandport = p4model.receivePacket(ns3packet);
	Ptr<Packet> egress_packet = egress_packetandport->packet;
	int egress_port_num = egress_packetandport->port_num;
	Ptr<NetDevice>outNetDevice = &m_ports[egress_port_num];
	int result = outNetDevice->Send(packet,*CreateObject<Address>(),0);
}


int P4NetDevice::GetPortNumber(Ptr<NetDevice> port){
	auto it = std::find(m_ports.begin(), m_ports.end(), port);
	auto index = std::distance(m_ports.begin(), it);
}


P4NetDevice::P4NetDevice(){
		p4model = new P4Model("/home/mark/1.json");
		p4model.init();
	}
