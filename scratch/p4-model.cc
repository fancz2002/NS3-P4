#include "p4-model.h"
#define MAXSIZE 100000
using namespace bm;

P4Model::P4Model(){
	argParser = new TargetParserBasic();
}


int P4Model::init(int argc, char *argv[]){
    this->init_from_command_line_options(argc, argv, argParser);
    return 0;
}

struct ns3PacketAndPort * P4Model::receivePacket(struct ns3PacketAndPort *ns3packet){
	struct bm2PacketAndPort * bm2packet= ns3tobmv2(ns3packet);
	bm::Packet *packet = bm2packet->packet;
	int port_num = bm2packet->port_num;
	int len = packet->get_data_size();
	packet->set_ingress_port(port_num);
    PHV *phv = packet->get_phv();
    phv->reset_metadata();
    phv->get_field("standard_metadata.ingress_port").set(port_num);
    phv->get_field("standard_metadata.packet_length").set(len);
    Field &f_instance_type = phv->get_field("standard_metadata.instance_type");

    if (phv->has_field("intrinsic_metadata.ingress_global_timestamp")) {
        phv->get_field("intrinsic_metadata.ingress_global_timestamp")
                .set(0);
    }

    // Ingress
    Parser *parser = this->get_parser("parser");
    Pipeline *ingress_mau = this->get_pipeline("ingress");
    phv = packet->get_phv();

    parser->parse(packet);

    ingress_mau->apply(packet);

    packet->reset_exit();


    Field &f_egress_spec = phv->get_field("standard_metadata.egress_spec");
    int egress_port = f_egress_spec.get_int();

    // Egress
    Deparser *deparser = this->get_deparser("deparser");
    Pipeline *egress_mau = this->get_pipeline("egress");
    f_egress_spec = phv->get_field("standard_metadata.egress_spec");
    f_egress_spec.set(0);
    egress_mau->apply(packet);
    deparser->deparse(packet);

    // Build return value
    struct bm2PacketAndPort * outPacket = new struct bm2PacketAndPort;
    outPacket->packet = packet;
    outPacket->port_num = egress_port;
    return bmv2tons3(outPacket);

}



struct ns3PacketAndPort * P4Model::bmv2tons3(struct bm2PacketAndPort *bm2packet){
	struct ns3PacketAndPort * ret = new struct ns3PacketAndPort;
	// Extract and set buffer
	void *buffer_start = bm2packet->packet->data();
	size_t buffer_length = bm2packet->packet->get_data_size();
	ret->packet = new ns3::Packet((unsigned char *)buffer_start,buffer_length);
	// Extract and set port number
	ret->port_num = bm2packet->port_num;
	// Set packet size
	return ret;
}

struct bm2PacketAndPort * P4Model::ns3tobmv2(struct ns3PacketAndPort *ns3packet){
	struct bm2PacketAndPort * ret = new struct bm2PacketAndPort;
	int len = ns3packet->packet->GetSize();
	int port_num = ns3packet->port_num;
	uint8_t * buffer = new uint8_t *[sizeof(uint8_t)*MAXSIZE];
	if (ns3packet->packet->Serialize(buffer,MAXSIZE)){
		ret->packet = new_packet_ptr(port_num, pktID++, len,
				bm::PacketBuffer(len + 512, buffer, len));	}
	ret-> port_num = port_num;
	return ret;
}

