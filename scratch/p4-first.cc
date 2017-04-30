/*
 * p4-first.cc
 *
 *  Created on: Apr 29, 2017
 *      Author: mark
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/openflow-module.h"
#include "ns3/log.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("First P4 Script");

int main(int argc, char *argv[])
{
	CommandLine cmd;
	cmd.Parse(argc,argv);
	Time::SetResolution(Time::NS);

	NS_LOG_INFO("Now let's get started!");
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

	// Init NodeContainer
	NodeContainer csmaSwitch;
	csmaSwitch.Create(1);
	NodeContainer terminals;
	terminals.Create(2);

	// Init Channel
	NS_LOG_INFO("Building Topology.");
	CsmaHelper csma;
	csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
	csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
	NS_LOG_INFO("Create csma links from each terminal to switch.");

	// Init NetDeviceContainers
	NetDeviceContainer terminalDevices;
	NetDeviceContainer switchDevices;
	for (int i = 0; i<2;i++){
	      NetDeviceContainer link = csma.Install (NodeContainer (terminals.Get (i), csmaSwitch));
	      terminalDevices.Add(link.Get(0));
	      switchDevices.Add(link.Get(1));
	}

	// Create the switch net device
	Ptr<Node> switchNode = csmaSwitch.Get(0);
	OpenFlowSwitchHelper swtch;
    Ptr<ns3::ofi::LearningController> controller = CreateObject<ns3::ofi::LearningController> ();
    swtch.Install (switchNode, switchDevices, controller);//Install swtichDevice onto switchNode


    // Add internet stack to the terminals node
    InternetStackHelper internet;
    internet.Install(terminals);
    // Add internet ip to the "NIC" of the node
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(terminalDevices);


    // Init Application
    UdpEchoServerHelper echoServer(99);
    ApplicationContainer serverApps = echoServer.Install(terminals.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));
    UdpEchoClientHelper echoClient(interfaces.GetAddress(0),99);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = echoClient.Install (terminals.Get (1));
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (10.0));

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
