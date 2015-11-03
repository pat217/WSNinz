////////////////////////////////////////////////////////////////////////////////////////
//
//Based on the examples wsn-ping6.cc, ping6.cc, example-ping-lr-wpan.cc and udp-echo.cc
#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/applications-module.h"
#include "ns3/packet.h"
#include "ns3/socket.h"
#include "ns3/packet-socket.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/internet-stack-helper.h"
#include <ns3/ipv6-address-helper.h>
#include <ns3/sixlowpan-helper.h>
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include <ns3/spectrum-value.h>
#include "ns3/trace-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
#include <ns3/udp-header.h>
#include "ns3/seq-ts-header.h"
#include "ns3/realtime-simulator-impl.h"
#include "wsn_fun.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("WSN_REG");


	Ptr<LrWpanNetDevice> dev1 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev2 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev3 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev4 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev5 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev6 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev7 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev8 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev9 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev10 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev11 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev12 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev13 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev14 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev15 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev16 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev17 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev18 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev19 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev20 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev21 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev22 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev23 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev24 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev25 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev26 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev27 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev28 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev29 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev30 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev31 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev32 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev33 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev34 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev35 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev36 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev37 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev38 = CreateObject<LrWpanNetDevice>();
	Ptr<LrWpanNetDevice> dev39 = CreateObject<LrWpanNetDevice>();

int main()
{
	SeqReceivedBySink[23]=23;
	LogComponentEnable ("WSN_REG", LOG_LEVEL_INFO);
	LogComponentEnable ("Socket", LOG_LEVEL_INFO);
  
	//Create nodes//
	NodeContainer nodes = CreateNodeContainer();
	//********// 
	
	//Create lrwpan devices//
	NetDeviceContainer lrwpanDevices;// = CreateLrwpanDevices();
	lrwpanDevices.Add(dev1);
	lrwpanDevices.Add(dev2);
	lrwpanDevices.Add(dev3);
	lrwpanDevices.Add(dev4);
	lrwpanDevices.Add(dev5);
	lrwpanDevices.Add(dev6);
	lrwpanDevices.Add(dev7);
	lrwpanDevices.Add(dev8);
	lrwpanDevices.Add(dev9);
	lrwpanDevices.Add(dev10);
	lrwpanDevices.Add(dev11);
	lrwpanDevices.Add(dev12);
	lrwpanDevices.Add(dev13);
	lrwpanDevices.Add(dev14);
	lrwpanDevices.Add(dev15);
	lrwpanDevices.Add(dev16);
	lrwpanDevices.Add(dev17);
	lrwpanDevices.Add(dev18);
	lrwpanDevices.Add(dev19);
	lrwpanDevices.Add(dev20);
	lrwpanDevices.Add(dev21);
	lrwpanDevices.Add(dev22);
	lrwpanDevices.Add(dev23);
	lrwpanDevices.Add(dev24);
	lrwpanDevices.Add(dev25);
	lrwpanDevices.Add(dev26);
	lrwpanDevices.Add(dev27);
	lrwpanDevices.Add(dev28);
	lrwpanDevices.Add(dev29);
	lrwpanDevices.Add(dev30);
	lrwpanDevices.Add(dev31);
	lrwpanDevices.Add(dev32);
	lrwpanDevices.Add(dev33);
	lrwpanDevices.Add(dev34);
	lrwpanDevices.Add(dev35);
	lrwpanDevices.Add(dev36);
	lrwpanDevices.Add(dev37);
	lrwpanDevices.Add(dev38);
	lrwpanDevices.Add(dev39);
	
	
	std::vector<Ptr<LrWpanNetDevice> > lrWpanDevicesVector;// = CreateLrwpanDeviceVector();
	lrWpanDevicesVector.push_back(dev1);
	lrWpanDevicesVector.push_back(dev2);
	lrWpanDevicesVector.push_back(dev3);
	lrWpanDevicesVector.push_back(dev4);
	lrWpanDevicesVector.push_back(dev5);
	lrWpanDevicesVector.push_back(dev6);
	lrWpanDevicesVector.push_back(dev7);
	lrWpanDevicesVector.push_back(dev8);
	lrWpanDevicesVector.push_back(dev9);
	lrWpanDevicesVector.push_back(dev10);
	lrWpanDevicesVector.push_back(dev11);
	lrWpanDevicesVector.push_back(dev12);
	lrWpanDevicesVector.push_back(dev13);
	lrWpanDevicesVector.push_back(dev14);
	lrWpanDevicesVector.push_back(dev15);
	lrWpanDevicesVector.push_back(dev16);
	lrWpanDevicesVector.push_back(dev17);
	lrWpanDevicesVector.push_back(dev18);
	lrWpanDevicesVector.push_back(dev19);
	lrWpanDevicesVector.push_back(dev20);
	lrWpanDevicesVector.push_back(dev21);
	lrWpanDevicesVector.push_back(dev22);
	lrWpanDevicesVector.push_back(dev23);
	lrWpanDevicesVector.push_back(dev24);
	lrWpanDevicesVector.push_back(dev25);
	lrWpanDevicesVector.push_back(dev26);
	lrWpanDevicesVector.push_back(dev27);
	lrWpanDevicesVector.push_back(dev28);
	lrWpanDevicesVector.push_back(dev29);
	lrWpanDevicesVector.push_back(dev30);
	lrWpanDevicesVector.push_back(dev31);
	lrWpanDevicesVector.push_back(dev32);
	lrWpanDevicesVector.push_back(dev33);
	lrWpanDevicesVector.push_back(dev34);
	lrWpanDevicesVector.push_back(dev35);
	lrWpanDevicesVector.push_back(dev36);
	lrWpanDevicesVector.push_back(dev37);
	lrWpanDevicesVector.push_back(dev38);
	lrWpanDevicesVector.push_back(dev39);
	
	//Generate pseudorandom Seed from the actual time//
	Ptr<RealtimeSimulatorImpl> impl = new RealtimeSimulatorImpl() ;
	Time t = impl->RealtimeNow (); 
	Seed = (int)((t.GetMinutes()-((uint8_t)t.GetYears())*365*24*60)-(uint16_t)((t.GetMinutes()-((uint8_t)t.GetYears())*365*24*60)/60)*60);
	
	if (Seed == 0) Seed=61;//seed cannot be 0
	//**********//
	
	
	
	bool verbose = false;
	LrWpanHelper lrWpanHelper;
	
	if(verbose){
	lrWpanHelper.EnableLogComponents ();
	}
  
		
	//Assign MAC addresses to the lrwpanDevices
	AssignMac16Address(lrwpanDevices);	
	


	//Each device must be attached to same channel
	Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel>();
	Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel>();
	Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel>();
	channel->AddPropagationLossModel(propModel);
	channel->SetPropagationDelayModel(delayModel);
	
	//attaching devices to the above specified channel
	SetChannelForDevices(lrWpanDevicesVector, channel);
		
	//To complete configuration, a LrWpanNetDevice must be added to a node
	AddDeviceToNode(lrwpanDevices, nodes);
	
	
	//Set distance between nodes
	SetDistances(lrWpanDevicesVector);
		
	
	//Install protocol stack at the nodes
	InternetStackHelper internetv6;
	internetv6.Install (nodes);
	NS_LOG_INFO ("nodes size: "<<nodes.GetN());
	SixLowPanHelper sixlowpan;
  	NetDeviceContainer devices = sixlowpan.Install (lrwpanDevices); 
 
  	Ipv6AddressHelper ipv6;
  	ipv6.SetBase (Ipv6Address ("2001:2::"), Ipv6Prefix (64));
  	Ipv6InterfaceContainer deviceInterfaces;
  	deviceInterfaces = ipv6.Assign (devices);
  	deviceInterfaces.SetForwarding (0, true);
    deviceInterfaces.SetDefaultRouteInAllNodes (0);
	
	for(uint8_t i=0; i<nodes.GetN(); i++){
	Ptr<Ipv6> ipv6_1 = nodes.Get(i)->GetObject<Ipv6> ();
	}

    
    //Add layer UDP
    NS_LOG_INFO ("Adding UDP.");
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    
    ///////--------------------NODES ROUTES CONF-----------------------------//////////////
    
    
    ConfNodesRoutes(nodes, tid, deviceInterfaces);
    
       
    AsciiTraceHelper ascii;
  
    //lrWpanHelper.EnableAsciiAll(ascii.CreateFileStream ("wsn.txt"));
    lrWpanHelper.EnablePcapAll(std::string ("/mnt/hgfs/pcapFiles/wsn")/*, nodes*/);//teraz pokazuje pakiety wyslane z kazdego z nodow
    
    
	//running simulation
	NS_LOG_INFO ("Run Simulation.");
 	
	
	double min = simulationStart;
	double max = simulationStart+delay;
	SeedManager::SetSeed (Seed);
	SeedManager::SetRun (7); 
	Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
	x->SetAttribute ("Min", DoubleValue (min));
	x->SetAttribute ("Max", DoubleValue (max));

	double myRandomNo[39];
	for (int i=0; i<39; i++){// loop that generates random delays for sending packets
	 myRandomNo[i] = x->GetValue();
	}
	
	double nextTime=myRandomNo[0];
	for(int i=0; i<39; i++){
		if(myRandomNo[i]>nextTime) nextTime=myRandomNo[i];
	}
	
 NS_LOG_INFO("sendSockets.size"<<sendSockets.size());
 //////////////////Sending packets////////////////////////////////////////////
	for (uint8_t i=0; i<sendSockets.size(); i++){
		if(i<=23){
	sendFrom(sendSockets[i], packetSize,  ns3::Time::FromDouble(myRandomNo[i], ns3::Time::S), numPackets, interPacketInterval);	}
	else {sendFrom(sendSockets[i-1], packetSize,  ns3::Time::FromDouble(myRandomNo[i], ns3::Time::S), numPackets, interPacketInterval);}
	}
		 	
 
	Ptr<FlowMonitor> flowMonitor;
	FlowMonitorHelper flowHelper;
	flowMonitor = flowHelper.InstallAll();
	Simulator::Schedule(Seconds(nextTime+delay), &checkForRetransmission, Seconds(nextTime+delay));
	
	Simulator::Schedule(Seconds(nextTime+2*delay), &checkIfAllArrived, Seconds(nextTime+2*delay)); 
	
				 
    Simulator::Stop(Seconds (simulationEnd));
    Simulator::Run ();
    Simulator::Destroy ();
		
	
	NS_LOG_INFO ("Done.");
    
	//flowMonitor->SerializeToXmlFile("/mnt/hgfs/pcapFiles/wsn.xml", true, true);      
    
   	return 0;
}
