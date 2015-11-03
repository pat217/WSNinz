#include "wsn_fun.h"
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
#include <ns3/udp-header.h>
#include "ns3/seq-ts-header.h"
#include "string.h"
#include <sstream>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("WSN_FUN");

uint32_t NumOfPacketsRecvBySink = 0;
uint16_t port = 4000;
uint32_t packetSize = 100; //bytes
Time interPacketInterval = Seconds (0.1);
uint32_t numPackets = 1; //100 packets	
double delay = 1;//[s]0.9
double simulationStart = 0.0;
double simulationEnd = 20.0;
uint8_t Seed = 5;
uint8_t allReceived=2;//flag that controls if all of the packets were received by sink
uint8_t retransmit = 0;
uint32_t SeqReceivedBySink[39];
std::vector<int> SeqSent;
std::vector<Ptr<Socket> >sendSockets;	
std::vector<Ptr<Socket> >recvSockets;

void ReceivePacket (Ptr<Socket>dest, Ptr<Socket> socket)
{ 	
	Ptr<Packet> pRecv = socket->Recv ();
	SeqTsHeader sh;
	pRecv->RemoveHeader(sh);
	 
	 if (pRecv)
    {	
		NS_LOG_UNCOND (Now().As(Time::S)<<"  Node " << socket->GetNode()->GetId()+1 << " received one packet with id: "<<sh.GetSeq());
        pRecv->AddHeader(sh);
        dest->Send (pRecv);
    }
    else NS_LOG_UNCOND("Error receiving packet");
}


void ReceiveDestiny (Ptr<Socket> socket)
{
	Ptr<Packet> pFinal = socket->Recv ();
	SeqTsHeader sh;
	pFinal->RemoveHeader(sh);
	
  if (pFinal)
    {
      NS_LOG_UNCOND (Now().As(Time::S)<<"  Packet with id: "<<sh.GetSeq()<<" arrived at sink node.");
      NumOfPacketsRecvBySink++;
      SeqReceivedBySink[sh.GetSeq()]=sh.GetSeq();
    }
    else NS_LOG_UNCOND("Error receiving packet");
    
}

void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,uint32_t pktCount, Time pktInterval )
{
  if (pktCount > 0)
    {
	 Ptr <Packet> p = Create<Packet>(pktSize);
	 SeqTsHeader sh = SeqTsHeader ();
	 sh.SetSeq(socket->GetNode()->GetId()+1);
	 p->AddHeader (sh);
	 
      socket->Send (p);
      NS_LOG_UNCOND(Now().As(Time::S)<<" Node "<<socket->GetNode()->GetId()+1 <<" sends packet number "<< pktCount<<"with id "<<sh.GetSeq());
      Simulator::Schedule (pktInterval, &GenerateTraffic, 
                           socket, pktSize,pktCount-1, pktInterval);
      SeqSent.push_back(sh.GetSeq());
    }
  else
    {
    }
}

void sendFrom( Ptr<Socket> from,  uint32_t packetSize, Time when, uint32_t numPackets, Time interPacketInterval)
{
	 Simulator::ScheduleWithContext (from->GetNode()->GetId (),
                                  when, &GenerateTraffic, 
                                  from, packetSize, numPackets, interPacketInterval);	
                         
    //Simulator::ScheduleNow(&GenerateTraffic, from, packetSize, numPackets, interPacketInterval);	
	} 
	
	
void retransmitFun( Ptr<Socket> from,  uint32_t packetSize, Time when, uint32_t numPackets, Time interPacketInterval)
{
	 Simulator::ScheduleWithContext (from->GetNode()->GetId (),
                                  when, &GenerateTraffic, 
                                  from, packetSize, numPackets, interPacketInterval);	
    //Simulator::ScheduleNow(&GenerateTraffic, from, packetSize, numPackets, interPacketInterval);	
	}                            


void checkForRetransmission(Time when){
	//NS_LOG_UNCOND("checkforretranssmission");
		
	NS_LOG_UNCOND("Num of packets sent "<<SeqSent.size());
	NS_LOG_UNCOND ("Num of packets that reached sink:"<<NumOfPacketsRecvBySink);
	
	double min =0;
	double max =delay;
	SeedManager::SetSeed(Seed);
		
	Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
	x->SetAttribute ("Min", DoubleValue (min));
	x->SetAttribute ("Max", DoubleValue (max));
	
      
    if (SeqSent.size()>=38){//packets with all the sequences were sent
    for (int i=1; i<38; i++){
		if(SeqReceivedBySink[i]==0){//the i. packet didn't arrive 
							
			NS_LOG_UNCOND(Now().As(Time::S));
			NS_LOG_UNCOND("Packet "<<i<<" didn't arrive");
			if (i<=23){
			retransmitFun(sendSockets[i-1], packetSize, Seconds((when.GetDouble())/10e18+x->GetValue()), numPackets, interPacketInterval);}
			else{
							retransmitFun(sendSockets[i-2], packetSize, Seconds((when.GetDouble())/10e18+x->GetValue()), numPackets, interPacketInterval);}
			}
	}
}
}

void checkIfAllArrived(Time when){
	//NS_LOG_UNCOND("checkifallarrived");
	uint8_t numOfNulls=0;
	for (int i=1; i<39; i++){
		
		if(SeqReceivedBySink[i]==0){
			numOfNulls++;
			//NS_LOG_INFO("false");
			allReceived=0;//not all received
			retransmit=1;
		}
		else{ retransmit=0;
			allReceived=1;}
			}
	
	for(int i=1; i<39; i++){
		std::cout<<SeqReceivedBySink[i]<<" ";
		}
		
	if(numOfNulls==0){ NS_LOG_UNCOND("true");//all received by sink

	}
	else{
	
	NS_LOG_UNCOND("Next retransmission");
		//NS_LOG_INFO("When retranssmission"<<(when.GetDouble())/10e18+delay);
		Simulator::Schedule(Seconds((when.GetDouble())/10e18+delay),&checkForRetransmission, Seconds((when.GetDouble())/10e18+delay));
		Simulator::Schedule(Seconds((when.GetDouble())/10e18+2*delay),&checkIfAllArrived, Seconds((when.GetDouble())/10e18+2*delay));

		//Simulator::ScheduleNow(&checkForRetransmission, Seconds(when+delay));
		}
		}



void ConfNodesRoutes(NodeContainer nodes, TypeId tid, Ipv6InterfaceContainer deviceInterfaces){
	
	//node 1
    Ptr<Socket> inter1Recv = Socket::CreateSocket (nodes.Get(0), tid); 
    Ptr<Socket> inter1_3Sender = Socket::CreateSocket (nodes.Get(0), tid);
    Inet6SocketAddress local1 = Inet6SocketAddress (Ipv6Address::GetAny (), port+1);//listens to 4001;
    Inet6SocketAddress remote1_3= Inet6SocketAddress (deviceInterfaces.GetAddress (2,1), port+3);//sends on 4003 to n3
    inter1Recv->Bind (local1);
    inter1_3Sender->SetAllowBroadcast (false);
    inter1_3Sender->Connect (remote1_3);
    inter1Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter1_3Sender));
    recvSockets.push_back(inter1Recv);
    sendSockets.push_back(inter1_3Sender);
    

    //node 2
  
    Ptr<Socket> inter2Recv = Socket::CreateSocket (nodes.Get(1), tid);
    Ptr<Socket> inter2_10Sender = Socket::CreateSocket (nodes.Get(1), tid);
    Inet6SocketAddress local2 = Inet6SocketAddress (Ipv6Address::GetAny (), port+2); //listens 4002
    Inet6SocketAddress remote2_10 = Inet6SocketAddress (deviceInterfaces.GetAddress (9,1), port+10);//sends on 4010 to n10
    inter2Recv->Bind (local2);
    inter2_10Sender->Connect(remote2_10);
    inter2Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter2_10Sender));
	recvSockets.push_back(inter2Recv);
    sendSockets.push_back(inter2_10Sender);
    
    //node 3
    Ptr<Socket> inter3Recv = Socket::CreateSocket (nodes.Get(2), tid);
    Ptr<Socket> inter3_9Sender = Socket::CreateSocket (nodes.Get(2), tid);
    Inet6SocketAddress local3 = Inet6SocketAddress (Ipv6Address::GetAny (), port+3);//listens to port 4003
    Inet6SocketAddress remote3_9 = Inet6SocketAddress (deviceInterfaces.GetAddress (8,1), port+9);//sends on port 4009 to n9
    inter3Recv->Bind (local3);
    inter3_9Sender->Connect(remote3_9);
    inter3Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter3_9Sender));
    recvSockets.push_back(inter3Recv);
    sendSockets.push_back(inter3_9Sender);
     
    //node 4
    Ptr<Socket> inter4Recv = Socket::CreateSocket (nodes.Get(3), tid);
    Ptr<Socket> inter4_9Sender = Socket::CreateSocket (nodes.Get(3), tid);
    Inet6SocketAddress local4 = Inet6SocketAddress (Ipv6Address::GetAny (), port+4); //listens to port 4004
    Inet6SocketAddress remote4_9 = Inet6SocketAddress (deviceInterfaces.GetAddress (8,1), port+9);//sends on port 4009 to n9
    inter4Recv->Bind (local4);
    inter4_9Sender->Connect(remote4_9);
    inter4Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter4_9Sender));
    recvSockets.push_back(inter4Recv);
    sendSockets.push_back(inter4_9Sender);

	//node 5
	Ptr<Socket> inter5Recv = Socket::CreateSocket (nodes.Get(4), tid);
    Ptr<Socket> inter5_8Sender = Socket::CreateSocket (nodes.Get(4), tid);
    Inet6SocketAddress local5 = Inet6SocketAddress (Ipv6Address::GetAny (), port+5); //listens to port 4005
    Inet6SocketAddress remote5_8 = Inet6SocketAddress (deviceInterfaces.GetAddress (7,1), port+8);//sends on port 4008 to n8
    inter5Recv->Bind (local5);
    inter5_8Sender->Connect(remote5_8);
    inter5Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter5_8Sender));
    recvSockets.push_back(inter5Recv);
    sendSockets.push_back(inter5_8Sender);
    
    //node 6
    Ptr<Socket> inter6Recv = Socket::CreateSocket (nodes.Get(5), tid);
    Ptr<Socket> inter6_8Sender = Socket::CreateSocket (nodes.Get(5), tid);
    Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), port+6); //listens to port 4006
    Inet6SocketAddress remote6_8 = Inet6SocketAddress (deviceInterfaces.GetAddress (7,1), port+8);//sends on port 4008 to n8
    inter6Recv->Bind (local6);
    inter6_8Sender->Connect(remote6_8);
    inter6Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter6_8Sender));
 	recvSockets.push_back(inter6Recv);
    sendSockets.push_back(inter6_8Sender); 
    
    //node 7
    Ptr<Socket> inter7Recv = Socket::CreateSocket (nodes.Get(6), tid);
    Ptr<Socket> inter7_8Sender = Socket::CreateSocket (nodes.Get(6), tid);
    Inet6SocketAddress local7 = Inet6SocketAddress (Ipv6Address::GetAny (), port+7); //listens to port 4007
    Inet6SocketAddress remote7_8 = Inet6SocketAddress (deviceInterfaces.GetAddress (7,1), port+8);//sends on port 4008 to n8
    inter7Recv->Bind (local7);
    inter7_8Sender->Connect(remote7_8);
    inter7Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter7_8Sender));
	recvSockets.push_back(inter7Recv);
    sendSockets.push_back(inter7_8Sender);
	
	//node 8
	Ptr<Socket> inter8Recv = Socket::CreateSocket (nodes.Get(7), tid);
    Ptr<Socket> inter8_sinkSender = Socket::CreateSocket (nodes.Get(7), tid);
    Inet6SocketAddress local8 = Inet6SocketAddress (Ipv6Address::GetAny (), port+8); //listens to port 4008
    Inet6SocketAddress remote8_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter8Recv->Bind (local8);
    inter8_sinkSender->Connect(remote8_sink);
    inter8Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter8_sinkSender));
	recvSockets.push_back(inter8Recv);
    sendSockets.push_back(inter8_sinkSender);  
    
    //node 9
    Ptr<Socket> inter9Recv = Socket::CreateSocket (nodes.Get(8), tid);
    Ptr<Socket> inter9_sinkSender = Socket::CreateSocket (nodes.Get(8), tid);
    Inet6SocketAddress local9 = Inet6SocketAddress (Ipv6Address::GetAny (), port+9); //listens to port 4009
    Inet6SocketAddress remote9_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter9Recv->Bind (local9);
    inter9_sinkSender->Connect(remote9_sink);
    inter9Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter9_sinkSender));
	recvSockets.push_back(inter9Recv);
    sendSockets.push_back(inter9_sinkSender); 
    
     //node 10
    Ptr<Socket> inter10Recv = Socket::CreateSocket (nodes.Get(9), tid);
    Ptr<Socket> inter10_sinkSender = Socket::CreateSocket (nodes.Get(9), tid);
    Inet6SocketAddress local10 = Inet6SocketAddress (Ipv6Address::GetAny (), port+10); //listens to port 4010
    Inet6SocketAddress remote10_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter10Recv->Bind (local10);
    inter10_sinkSender->Connect(remote10_sink);
    inter10Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter10_sinkSender));
	recvSockets.push_back(inter10Recv);
    sendSockets.push_back(inter10_sinkSender);   
    
    //node 11
    Ptr<Socket> inter11Recv = Socket::CreateSocket (nodes.Get(10), tid);
    Ptr<Socket> inter11_sinkSender = Socket::CreateSocket (nodes.Get(10), tid);
    Inet6SocketAddress local11 = Inet6SocketAddress (Ipv6Address::GetAny (), port+11); //listens to port 4011
    Inet6SocketAddress remote11_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter11Recv->Bind (local11);
    inter11_sinkSender->Connect(remote11_sink);
    inter11Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter11_sinkSender));
	recvSockets.push_back(inter11Recv);
    sendSockets.push_back(inter11_sinkSender);   
    
    //node 12
    Ptr<Socket> inter12Recv = Socket::CreateSocket (nodes.Get(11), tid);
    Ptr<Socket> inter12_sinkSender = Socket::CreateSocket (nodes.Get(11), tid);
    Inet6SocketAddress local12 = Inet6SocketAddress (Ipv6Address::GetAny (), port+12); //listens to port 4012
    Inet6SocketAddress remote12_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter12Recv->Bind (local12);
    inter12_sinkSender->Connect(remote12_sink);
    inter12Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter12_sinkSender));
 	recvSockets.push_back(inter12Recv);
    sendSockets.push_back(inter12_sinkSender);
    
    //node 13
    Ptr<Socket> inter13Recv = Socket::CreateSocket (nodes.Get(12), tid);
    Ptr<Socket> inter13_14Sender = Socket::CreateSocket (nodes.Get(12), tid);
    Inet6SocketAddress local13 = Inet6SocketAddress (Ipv6Address::GetAny (), port+13); //listens to port 4013
    Inet6SocketAddress remote13_14 = Inet6SocketAddress (deviceInterfaces.GetAddress (13,1), port+14);//sends on port 4014 to n14
    inter13Recv->Bind (local13);
    inter13_14Sender->Connect(remote13_14);
    inter13Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter13_14Sender));
	recvSockets.push_back(inter13Recv);
    sendSockets.push_back(inter13_14Sender);
    
     //node 14
    Ptr<Socket> inter14Recv = Socket::CreateSocket (nodes.Get(13), tid);
    Ptr<Socket> inter14_sinkSender = Socket::CreateSocket (nodes.Get(13), tid);
    Inet6SocketAddress local14 = Inet6SocketAddress (Ipv6Address::GetAny (), port+14); //listens to port 4014
    Inet6SocketAddress remote14_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter14Recv->Bind (local14);
    inter14_sinkSender->Connect(remote14_sink);
    inter14Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter14_sinkSender));
 	recvSockets.push_back(inter14Recv);
    sendSockets.push_back(inter14_sinkSender);  
    
    
     //node 15
    Ptr<Socket> inter15Recv = Socket::CreateSocket (nodes.Get(14), tid);
    Ptr<Socket> inter15_sinkSender = Socket::CreateSocket (nodes.Get(14), tid);
    Inet6SocketAddress local15 = Inet6SocketAddress (Ipv6Address::GetAny (), port+15); //listens to port 4014
    Inet6SocketAddress remote15_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter15Recv->Bind (local15);
    inter15_sinkSender->Connect(remote15_sink);
    inter15Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter15_sinkSender));
 	recvSockets.push_back(inter15Recv);
    sendSockets.push_back(inter15_sinkSender);
       
    //node 16
    Ptr<Socket> inter16Recv = Socket::CreateSocket (nodes.Get(15), tid);
    Ptr<Socket> inter16_14Sender = Socket::CreateSocket (nodes.Get(15), tid);
    Inet6SocketAddress local16 = Inet6SocketAddress (Ipv6Address::GetAny (), port+16); //listens to port 4016
    Inet6SocketAddress remote16_14 = Inet6SocketAddress (deviceInterfaces.GetAddress (13,1), port+14);//sends on port 4014 to n14
    inter16Recv->Bind (local16);
    inter16_14Sender->Connect(remote16_14);
    inter16Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter16_14Sender));
	recvSockets.push_back(inter16Recv);
    sendSockets.push_back(inter16_14Sender);
    
    //node 17
   
    Ptr<Socket> inter17Recv = Socket::CreateSocket (nodes.Get(16), tid);
    Ptr<Socket> inter17_sinkSender = Socket::CreateSocket (nodes.Get(16), tid);
    Inet6SocketAddress local17 = Inet6SocketAddress (Ipv6Address::GetAny (), port+17); //listens to port 4017
    Inet6SocketAddress remote17_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter17Recv->Bind (local17);
    inter17_sinkSender->Connect(remote17_sink);
    inter17Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter17_sinkSender));
 	recvSockets.push_back(inter17Recv);
    sendSockets.push_back(inter17_sinkSender);
    
       
    //node 18
    Ptr<Socket> inter18Recv = Socket::CreateSocket (nodes.Get(17), tid);
    Ptr<Socket> inter18_16Sender = Socket::CreateSocket (nodes.Get(17), tid);
    Inet6SocketAddress local18 = Inet6SocketAddress (Ipv6Address::GetAny (), port+18); //listens to port 4018
    Inet6SocketAddress remote18_16 = Inet6SocketAddress (deviceInterfaces.GetAddress (15,1), port+16);//sends on port 4016 to n16
    inter18Recv->Bind (local18);
    inter18_16Sender->Connect(remote18_16);
    inter18Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter18_16Sender));
	recvSockets.push_back(inter18Recv);
    sendSockets.push_back(inter18_16Sender);  
    
    //node 19
    Ptr<Socket> inter19Recv = Socket::CreateSocket (nodes.Get(18), tid);
    Ptr<Socket> inter19_13Sender = Socket::CreateSocket (nodes.Get(18), tid);
    Inet6SocketAddress local19 = Inet6SocketAddress (Ipv6Address::GetAny (), port+19); //listens to port 4019
    Inet6SocketAddress remote19_13 = Inet6SocketAddress (deviceInterfaces.GetAddress (12,1), port+13);//sends on port 4013 to n13
    inter19Recv->Bind (local19);
    inter19_13Sender->Connect(remote19_13);
    inter19Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter19_13Sender));
 	recvSockets.push_back(inter19Recv);
    sendSockets.push_back(inter19_13Sender);
 	
    //node 20
    Ptr<Socket> inter20Recv = Socket::CreateSocket (nodes.Get(19), tid);
    Ptr<Socket> inter20_13Sender = Socket::CreateSocket (nodes.Get(19), tid);
    Inet6SocketAddress local20 = Inet6SocketAddress (Ipv6Address::GetAny (), port+20); //listens to port 4020
    Inet6SocketAddress remote20_13 = Inet6SocketAddress (deviceInterfaces.GetAddress (12,1), port+13);//sends on port 4013 to n13
    inter20Recv->Bind (local20);
    inter20_13Sender->Connect(remote20_13);
    inter20Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter20_13Sender));
	recvSockets.push_back(inter20Recv);
    sendSockets.push_back(inter20_13Sender);    
        
    //node 21
    Ptr<Socket> inter21Recv = Socket::CreateSocket (nodes.Get(20), tid);
    Ptr<Socket> inter21_22Sender = Socket::CreateSocket (nodes.Get(20), tid);
    Inet6SocketAddress local21 = Inet6SocketAddress (Ipv6Address::GetAny (), port+21); //listens to port 4021
    Inet6SocketAddress remote21_22 = Inet6SocketAddress (deviceInterfaces.GetAddress (21,1), port+22);//sends on port 4022 to n22
    inter21Recv->Bind (local21);
    inter21_22Sender->Connect(remote21_22);
    inter21Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter21_22Sender));
	recvSockets.push_back(inter21Recv);
    sendSockets.push_back(inter21_22Sender);
    
    //node 22
    Ptr<Socket> inter22Recv = Socket::CreateSocket (nodes.Get(21), tid);
    Ptr<Socket> inter22_17Sender = Socket::CreateSocket (nodes.Get(21), tid);
    Inet6SocketAddress local22 = Inet6SocketAddress (Ipv6Address::GetAny (), port+22); //listens to port 4022
    Inet6SocketAddress remote22_17 = Inet6SocketAddress (deviceInterfaces.GetAddress (16,1), port+17);//sends on port 4017 to n17
    inter22Recv->Bind (local22);
    inter22_17Sender->Connect(remote22_17);
    inter22Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter22_17Sender));
 	recvSockets.push_back(inter22Recv);
    sendSockets.push_back(inter22_17Sender);   
    
    //SINK/////////////////
    Ptr<Socket> sinkRecv = Socket::CreateSocket (nodes.Get(22), tid);
    Inet6SocketAddress localsink = Inet6SocketAddress (Ipv6Address::GetAny (), port+23); //listens to port 4023
    sinkRecv->Bind (localsink);
    sinkRecv->SetRecvCallback (MakeCallback (&ReceiveDestiny));//notifies that sink got the packet
    
    //node 24
    Ptr<Socket> inter24Recv = Socket::CreateSocket (nodes.Get(23), tid); 
    Ptr<Socket> inter24_25Sender = Socket::CreateSocket (nodes.Get(23), tid);
    Inet6SocketAddress local24 = Inet6SocketAddress (Ipv6Address::GetAny (), port+24);//listens to 4024;
    Inet6SocketAddress remote24_25= Inet6SocketAddress (deviceInterfaces.GetAddress (24,1), port+25);//sends on 4025 to n25
    inter1Recv->Bind (local24);
    inter24_25Sender->SetAllowBroadcast (false);
    inter24_25Sender->Connect (remote24_25);
    inter24Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter24_25Sender));
    recvSockets.push_back(inter24Recv);
    sendSockets.push_back(inter24_25Sender);
    

    //node 25
  
    Ptr<Socket> inter25Recv = Socket::CreateSocket (nodes.Get(24), tid);
    Ptr<Socket> inter25_27Sender = Socket::CreateSocket (nodes.Get(24), tid);
    Inet6SocketAddress local25 = Inet6SocketAddress (Ipv6Address::GetAny (), port+25); //listens 4025
    Inet6SocketAddress remote25_27 = Inet6SocketAddress (deviceInterfaces.GetAddress (26,1), port+27);//sends on 4027 to n27
    inter25Recv->Bind (local25);
    inter25_27Sender->Connect(remote25_27);
    inter25Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter25_27Sender));
	recvSockets.push_back(inter25Recv);
    sendSockets.push_back(inter25_27Sender);
    
    //node 26
    Ptr<Socket> inter26Recv = Socket::CreateSocket (nodes.Get(25), tid);
    Ptr<Socket> inter26_28Sender = Socket::CreateSocket (nodes.Get(25), tid);
    Inet6SocketAddress local26 = Inet6SocketAddress (Ipv6Address::GetAny (), port+26);//listens to port 4026
    Inet6SocketAddress remote26_28 = Inet6SocketAddress (deviceInterfaces.GetAddress (27,1), port+28);//sends on port 4028 to n28
    inter26Recv->Bind (local26);
    inter26_28Sender->Connect(remote26_28);
    inter26Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter26_28Sender));
    recvSockets.push_back(inter26Recv);
    sendSockets.push_back(inter26_28Sender);
     
    //node 27
    Ptr<Socket> inter27Recv = Socket::CreateSocket (nodes.Get(26), tid);
    Ptr<Socket> inter27_28Sender = Socket::CreateSocket (nodes.Get(26), tid);
    Inet6SocketAddress local27 = Inet6SocketAddress (Ipv6Address::GetAny (), port+27); //listens to port 4027
    Inet6SocketAddress remote27_28 = Inet6SocketAddress (deviceInterfaces.GetAddress (27,1), port+28);//sends on port 4028 to n28
    inter27Recv->Bind (local27);
    inter27_28Sender->Connect(remote27_28);
    inter27Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter27_28Sender));
    recvSockets.push_back(inter27Recv);
    sendSockets.push_back(inter27_28Sender);

	//node 28
	Ptr<Socket> inter28Recv = Socket::CreateSocket (nodes.Get(27), tid);
    Ptr<Socket> inter28_sinkSender = Socket::CreateSocket (nodes.Get(27), tid);
    Inet6SocketAddress local28 = Inet6SocketAddress (Ipv6Address::GetAny (), port+28); //listens to port 4028
    Inet6SocketAddress remote28_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter28Recv->Bind (local28);
    inter28_sinkSender->Connect(remote28_sink);
    inter28Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter28_sinkSender));
    recvSockets.push_back(inter28Recv);
    sendSockets.push_back(inter28_sinkSender);
    
    //node 29
    Ptr<Socket> inter29Recv = Socket::CreateSocket (nodes.Get(28), tid);
    Ptr<Socket> inter29_31Sender = Socket::CreateSocket (nodes.Get(28), tid);
    Inet6SocketAddress local29 = Inet6SocketAddress (Ipv6Address::GetAny (), port+29); //listens to port 4029
    Inet6SocketAddress remote29_31 = Inet6SocketAddress (deviceInterfaces.GetAddress (30,1), port+31);//sends on port 4031 to n31
    inter29Recv->Bind (local29);
    inter29_31Sender->Connect(remote29_31);
    inter29Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter29_31Sender));
 	recvSockets.push_back(inter29Recv);
    sendSockets.push_back(inter29_31Sender); 
    
    //node 30
    Ptr<Socket> inter30Recv = Socket::CreateSocket (nodes.Get(29), tid);
    Ptr<Socket> inter30_36Sender = Socket::CreateSocket (nodes.Get(29), tid);
    Inet6SocketAddress local30 = Inet6SocketAddress (Ipv6Address::GetAny (), port+30); //listens to port 4030
    Inet6SocketAddress remote30_36 = Inet6SocketAddress (deviceInterfaces.GetAddress (35,1), port+36);//sends on port 4036 to n36
    inter30Recv->Bind (local30);
    inter30_36Sender->Connect(remote30_36);
    inter30Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket,inter30_36Sender));
	recvSockets.push_back(inter30Recv);
    sendSockets.push_back(inter30_36Sender);
	
	//node 31
	Ptr<Socket> inter31Recv = Socket::CreateSocket (nodes.Get(30), tid);
    Ptr<Socket> inter31_30Sender = Socket::CreateSocket (nodes.Get(30), tid);
    Inet6SocketAddress local31 = Inet6SocketAddress (Ipv6Address::GetAny (), port+31); //listens to port 4031
    Inet6SocketAddress remote31_30 = Inet6SocketAddress (deviceInterfaces.GetAddress (29,1), port+30);//sends on port 4030 to n30
    inter31Recv->Bind (local31);
    inter31_30Sender->Connect(remote31_30);
    inter31Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter31_30Sender));
	recvSockets.push_back(inter31Recv);
    sendSockets.push_back(inter31_30Sender);  
    
    //node 32
    Ptr<Socket> inter32Recv = Socket::CreateSocket (nodes.Get(31), tid);
    Ptr<Socket> inter32_36Sender = Socket::CreateSocket (nodes.Get(31), tid);
    Inet6SocketAddress local32 = Inet6SocketAddress (Ipv6Address::GetAny (), port+32); //listens to port 4032
    Inet6SocketAddress remote32_36 = Inet6SocketAddress (deviceInterfaces.GetAddress (35,1), port+36);//sends on port 4036 to n36
    inter32Recv->Bind (local32);
    inter32_36Sender->Connect(remote32_36);
    inter32Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter32_36Sender));
	recvSockets.push_back(inter32Recv);
    sendSockets.push_back(inter32_36Sender); 
    
     //node 33
    Ptr<Socket> inter33Recv = Socket::CreateSocket (nodes.Get(32), tid);
    Ptr<Socket> inter33_sinkSender = Socket::CreateSocket (nodes.Get(32), tid);
    Inet6SocketAddress local33 = Inet6SocketAddress (Ipv6Address::GetAny (), port+33); //listens to port 4033
    Inet6SocketAddress remote33_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter33Recv->Bind (local33);
    inter33_sinkSender->Connect(remote33_sink);
    inter33Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter33_sinkSender));
	recvSockets.push_back(inter33Recv);
    sendSockets.push_back(inter33_sinkSender);   
    
    //node 34
    Ptr<Socket> inter34Recv = Socket::CreateSocket (nodes.Get(33), tid);
    Ptr<Socket> inter34_39Sender = Socket::CreateSocket (nodes.Get(33), tid);
    Inet6SocketAddress local34 = Inet6SocketAddress (Ipv6Address::GetAny (), port+34); //listens to port 4034
    Inet6SocketAddress remote34_39 = Inet6SocketAddress (deviceInterfaces.GetAddress (38,1), port+39);//sends on port 4039 to n39
    inter34Recv->Bind (local34);
    inter34_39Sender->Connect(remote34_39);
    inter34Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter34_39Sender));
	recvSockets.push_back(inter34Recv);
    sendSockets.push_back(inter34_39Sender);   
    
    //node 35
    Ptr<Socket> inter35Recv = Socket::CreateSocket (nodes.Get(34), tid);
    Ptr<Socket> inter35_33Sender = Socket::CreateSocket (nodes.Get(34), tid);
    Inet6SocketAddress local35 = Inet6SocketAddress (Ipv6Address::GetAny (), port+35); //listens to port 4035
    Inet6SocketAddress remote35_33 = Inet6SocketAddress (deviceInterfaces.GetAddress (32,1), port+33);//sends on port 4033 to n33
    inter35Recv->Bind (local35);
    inter35_33Sender->Connect(remote35_33);
    inter35Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter35_33Sender));
 	recvSockets.push_back(inter35Recv);
    sendSockets.push_back(inter35_33Sender);
    
    //node 36
    Ptr<Socket> inter36Recv = Socket::CreateSocket (nodes.Get(35), tid);
    Ptr<Socket> inter36_sinkSender = Socket::CreateSocket (nodes.Get(35), tid);
    Inet6SocketAddress local36 = Inet6SocketAddress (Ipv6Address::GetAny (), port+36); //listens to port 4036
    Inet6SocketAddress remote36_sink = Inet6SocketAddress (deviceInterfaces.GetAddress (22,1), port+23);//sends on port 4023 to sink
    inter36Recv->Bind (local36);
    inter36_sinkSender->Connect(remote36_sink);
    inter36Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter36_sinkSender));
	recvSockets.push_back(inter36Recv);
    sendSockets.push_back(inter36_sinkSender);
    
     //node 37
    Ptr<Socket> inter37Recv = Socket::CreateSocket (nodes.Get(36), tid);
    Ptr<Socket> inter37_33Sender = Socket::CreateSocket (nodes.Get(36), tid);
    Inet6SocketAddress local37 = Inet6SocketAddress (Ipv6Address::GetAny (), port+37); //listens to port 4037
    Inet6SocketAddress remote37_33 = Inet6SocketAddress (deviceInterfaces.GetAddress (32,1), port+33);//sends on port 4033 to n33
    inter37Recv->Bind (local37);
    inter37_33Sender->Connect(remote37_33);
    inter37Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter37_33Sender));
 	recvSockets.push_back(inter37Recv);
    sendSockets.push_back(inter37_33Sender);  
    
    
     //node 38
    Ptr<Socket> inter38Recv = Socket::CreateSocket (nodes.Get(37), tid);
    Ptr<Socket> inter38_37Sender = Socket::CreateSocket (nodes.Get(37), tid);
    Inet6SocketAddress local38 = Inet6SocketAddress (Ipv6Address::GetAny (), port+38); //listens to port 4038
    Inet6SocketAddress remote38_37 = Inet6SocketAddress (deviceInterfaces.GetAddress (36,1), port+37);//sends on port 4037 to n37
    inter38Recv->Bind (local38);
    inter38_37Sender->Connect(remote38_37);
    inter38Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter38_37Sender));
 	recvSockets.push_back(inter38Recv);
    sendSockets.push_back(inter38_37Sender);
    
    //node 39
    Ptr<Socket> inter39Recv = Socket::CreateSocket (nodes.Get(38), tid);
    Ptr<Socket> inter39_33Sender = Socket::CreateSocket (nodes.Get(38), tid);
    Inet6SocketAddress local39 = Inet6SocketAddress (Ipv6Address::GetAny (), port+39); //listens to port 4039
    Inet6SocketAddress remote39_33 = Inet6SocketAddress (deviceInterfaces.GetAddress (32,1), port+33);//sends on port 4033 to n33
    inter39Recv->Bind (local39);
    inter39_33Sender->Connect(remote39_33);
    inter39Recv->SetRecvCallback (MakeBoundCallback(&ReceivePacket, inter39_33Sender));
 	recvSockets.push_back(inter39Recv);
    sendSockets.push_back(inter39_33Sender);
    
	 NS_LOG_UNCOND("ConfNodesRoutes"); 
	}



void SetDistances(std::vector<Ptr<LrWpanNetDevice> > lrwpanDevices){
	
	
	Ptr<ConstantPositionMobilityModel> sender1 = CreateObject<ConstantPositionMobilityModel>();
	sender1->SetPosition(Vector(0,23,3));
	lrwpanDevices.at(0)->GetPhy()->SetMobility(sender1);
	
	Ptr<ConstantPositionMobilityModel> sender2 = CreateObject<ConstantPositionMobilityModel>();
	sender2->SetPosition(Vector(7,23,3));
	lrwpanDevices.at(1)->GetPhy()->SetMobility(sender2);
	
	Ptr<ConstantPositionMobilityModel> sender3 = CreateObject<ConstantPositionMobilityModel>();
	sender3->SetPosition(Vector(10,18,3));
	lrwpanDevices.at(2)->GetPhy()->SetMobility(sender3);
	
	Ptr<ConstantPositionMobilityModel> sender4 = CreateObject<ConstantPositionMobilityModel>();
	sender4->SetPosition(Vector(9,14,3));
	lrwpanDevices.at(3)->GetPhy()->SetMobility(sender4);
	
	Ptr<ConstantPositionMobilityModel> sender5 = CreateObject<ConstantPositionMobilityModel>();
	sender5->SetPosition(Vector(0,0,3));
	lrwpanDevices.at(4)->GetPhy()->SetMobility(sender5);
	
	Ptr<ConstantPositionMobilityModel> sender6 = CreateObject<ConstantPositionMobilityModel>();
	sender6->SetPosition(Vector(10,9,3));
	lrwpanDevices.at(5)->GetPhy()->SetMobility(sender6);
	
	Ptr<ConstantPositionMobilityModel> sender7 = CreateObject<ConstantPositionMobilityModel>();
	sender7->SetPosition(Vector(14,1,3));
	lrwpanDevices.at(6)->GetPhy()->SetMobility(sender7);
	
	Ptr<ConstantPositionMobilityModel> sender8 = CreateObject<ConstantPositionMobilityModel>();
	sender8->SetPosition(Vector(9,14,3));
	lrwpanDevices.at(7)->GetPhy()->SetMobility(sender8);
	
	Ptr<ConstantPositionMobilityModel> sender9 = CreateObject<ConstantPositionMobilityModel>();
	sender9->SetPosition(Vector(20,14,3));
	lrwpanDevices.at(8)->GetPhy()->SetMobility(sender9);
	
	Ptr<ConstantPositionMobilityModel> sender10 = CreateObject<ConstantPositionMobilityModel>();
	sender10->SetPosition(Vector(25,20,3));
	lrwpanDevices.at(9)->GetPhy()->SetMobility(sender10);
	
	Ptr<ConstantPositionMobilityModel> sender11 = CreateObject<ConstantPositionMobilityModel>();
	sender11->SetPosition(Vector(23,13,3));
	lrwpanDevices.at(10)->GetPhy()->SetMobility(sender11);
	
	Ptr<ConstantPositionMobilityModel> sender12 = CreateObject<ConstantPositionMobilityModel>();
	sender12->SetPosition(Vector(27,2,3));
	lrwpanDevices.at(11)->GetPhy()->SetMobility(sender12);
	
	Ptr<ConstantPositionMobilityModel> sender13 = CreateObject<ConstantPositionMobilityModel>();
	sender13->SetPosition(Vector(39,22,3));
	lrwpanDevices.at(12)->GetPhy()->SetMobility(sender13);
	
	Ptr<ConstantPositionMobilityModel> sender14 = CreateObject<ConstantPositionMobilityModel>();
	sender14->SetPosition(Vector(28,16,3));
	lrwpanDevices.at(13)->GetPhy()->SetMobility(sender14);
	
	Ptr<ConstantPositionMobilityModel> sender15 = CreateObject<ConstantPositionMobilityModel>();
	sender15->SetPosition(Vector(33,12,3));
	lrwpanDevices.at(14)->GetPhy()->SetMobility(sender15);
	
	Ptr<ConstantPositionMobilityModel> sender16 = CreateObject<ConstantPositionMobilityModel>();
	sender16->SetPosition(Vector(37,18,3));
	lrwpanDevices.at(15)->GetPhy()->SetMobility(sender16);
	
	Ptr<ConstantPositionMobilityModel> sender17 = CreateObject<ConstantPositionMobilityModel>();
	sender17->SetPosition(Vector(40,9,3));
	lrwpanDevices.at(16)->GetPhy()->SetMobility(sender17);
	
	Ptr<ConstantPositionMobilityModel> sender18 = CreateObject<ConstantPositionMobilityModel>();
	sender18->SetPosition(Vector(42,17,3));
	lrwpanDevices.at(17)->GetPhy()->SetMobility(sender18);
	
	Ptr<ConstantPositionMobilityModel> sender19 = CreateObject<ConstantPositionMobilityModel>();
	sender19->SetPosition(Vector(44,20,3));
	lrwpanDevices.at(18)->GetPhy()->SetMobility(sender19);
	
	Ptr<ConstantPositionMobilityModel> sender20 = CreateObject<ConstantPositionMobilityModel>();
	sender20->SetPosition(Vector(48,26,3));
	lrwpanDevices.at(19)->GetPhy()->SetMobility(sender20);
	
	Ptr<ConstantPositionMobilityModel> sender21 = CreateObject<ConstantPositionMobilityModel>();
	sender21->SetPosition(Vector(45,17,3));
	lrwpanDevices.at(20)->GetPhy()->SetMobility(sender21);
	
	Ptr<ConstantPositionMobilityModel> sender22 = CreateObject<ConstantPositionMobilityModel>();
	sender22->SetPosition(Vector(43,2,3));
	lrwpanDevices.at(21)->GetPhy()->SetMobility(sender22);
	
	Ptr<ConstantPositionMobilityModel> sender23 = CreateObject<ConstantPositionMobilityModel>();
	sender23->SetPosition(Vector(28,10,3));
	lrwpanDevices.at(22)->GetPhy()->SetMobility(sender23);
	
	Ptr<ConstantPositionMobilityModel> sender24 = CreateObject<ConstantPositionMobilityModel>();
	sender24->SetPosition(Vector(0,23,0));
	lrwpanDevices.at(23)->GetPhy()->SetMobility(sender24);
	
	Ptr<ConstantPositionMobilityModel> sender25 = CreateObject<ConstantPositionMobilityModel>();
	sender25->SetPosition(Vector(11,18,0));
	lrwpanDevices.at(24)->GetPhy()->SetMobility(sender25);
	
	Ptr<ConstantPositionMobilityModel> sender26 = CreateObject<ConstantPositionMobilityModel>();
	sender26->SetPosition(Vector(25,20,0));
	lrwpanDevices.at(25)->GetPhy()->SetMobility(sender26);
	
	Ptr<ConstantPositionMobilityModel> sender27 = CreateObject<ConstantPositionMobilityModel>();
	sender27->SetPosition(Vector(20,14,0));
	lrwpanDevices.at(26)->GetPhy()->SetMobility(sender27);
	
	Ptr<ConstantPositionMobilityModel> sender28 = CreateObject<ConstantPositionMobilityModel>();
	sender28->SetPosition(Vector(23,14,0));
	lrwpanDevices.at(27)->GetPhy()->SetMobility(sender28);
	
	Ptr<ConstantPositionMobilityModel> sender29 = CreateObject<ConstantPositionMobilityModel>();
	sender29->SetPosition(Vector(0,0,0));
	lrwpanDevices.at(28)->GetPhy()->SetMobility(sender29);
	
	Ptr<ConstantPositionMobilityModel> sender30 = CreateObject<ConstantPositionMobilityModel>();
	sender30->SetPosition(Vector(22,1,0));
	lrwpanDevices.at(29)->GetPhy()->SetMobility(sender30);
	
	Ptr<ConstantPositionMobilityModel> sender31 = CreateObject<ConstantPositionMobilityModel>();
	sender31->SetPosition(Vector(18,9,0));
	lrwpanDevices.at(30)->GetPhy()->SetMobility(sender31);
	
	Ptr<ConstantPositionMobilityModel> sender32 = CreateObject<ConstantPositionMobilityModel>();
	sender32->SetPosition(Vector(44,2,0));
	lrwpanDevices.at(31)->GetPhy()->SetMobility(sender32);
	
	Ptr<ConstantPositionMobilityModel> sender33 = CreateObject<ConstantPositionMobilityModel>();
	sender33->SetPosition(Vector(29,16,0));
	lrwpanDevices.at(32)->GetPhy()->SetMobility(sender33);
	
	Ptr<ConstantPositionMobilityModel> sender34 = CreateObject<ConstantPositionMobilityModel>();
	sender34->SetPosition(Vector(48,25,0));
	lrwpanDevices.at(33)->GetPhy()->SetMobility(sender34);
	
	Ptr<ConstantPositionMobilityModel> sender35 = CreateObject<ConstantPositionMobilityModel>();
	sender35->SetPosition(Vector(34,12,0));
	lrwpanDevices.at(34)->GetPhy()->SetMobility(sender35);
	
	Ptr<ConstantPositionMobilityModel> sender36 = CreateObject<ConstantPositionMobilityModel>();
	sender36->SetPosition(Vector(27,2,0));
	lrwpanDevices.at(35)->GetPhy()->SetMobility(sender36);
	
	Ptr<ConstantPositionMobilityModel> sender37 = CreateObject<ConstantPositionMobilityModel>();
	sender37->SetPosition(Vector(42,14,0));
	lrwpanDevices.at(36)->GetPhy()->SetMobility(sender37);
	
	Ptr<ConstantPositionMobilityModel> sender38 = CreateObject<ConstantPositionMobilityModel>();
	sender38->SetPosition(Vector(48,11,0));
	lrwpanDevices.at(37)->GetPhy()->SetMobility(sender38);
	
	Ptr<ConstantPositionMobilityModel> sender39 = CreateObject<ConstantPositionMobilityModel>();
	sender39->SetPosition(Vector(40,22,0));
	lrwpanDevices.at(38)->GetPhy()->SetMobility(sender39);
	
	NS_LOG_UNCOND("SetDistances");	
}

template <typename T>
std::string to_string(T value)
{
	std::ostringstream os ;
	os << value ;
	return os.str() ;
}

void AssignMac16Address( NetDeviceContainer lrwpanDevices){
	
		std::string addr1;
				
	for (uint8_t i=0; i<lrwpanDevices.GetN(); i++){
		if(i<10){
			addr1="00:0"+to_string(i);
			
		}
		else{
		addr1= "00:"+to_string(i);
	
	}
	const char* res = addr1.c_str();
		lrwpanDevices.Get(i)->SetAddress(Mac16Address(res));
	}
	NS_LOG_UNCOND("AssignMac16Address");
}


void SetChannelForDevices (std::vector<Ptr<LrWpanNetDevice> > lrwpanDevices, Ptr<SingleModelSpectrumChannel> channel){
	NS_LOG_INFO("SetChannelForDevices");
	
	for (uint8_t j=0; j<lrwpanDevices.size(); j++){
	
	lrwpanDevices.at(j)-> SetChannel(channel);
	
	}
	NS_LOG_UNCOND("SetChannelForDevices");

}

void AddDeviceToNode(NetDeviceContainer lrwpanDevices, NodeContainer nodes){
	
	
	if (lrwpanDevices.GetN()==nodes.GetN()){
	for (uint8_t k=0; k<lrwpanDevices.GetN(); k++){
	nodes.Get(k)->AddDevice(lrwpanDevices.Get(k));
	}
}else NS_LOG_INFO("Size error!");
NS_LOG_UNCOND("AddDeviceToNode");
}

NodeContainer CreateNodeContainer(){
	NS_LOG_UNCOND("CreatenodeContainer");
	
	//1st floor
	Ptr<Node> n1 = CreateObject<Node>();
	Ptr<Node> n2 = CreateObject<Node>();
	Ptr<Node> n3 = CreateObject<Node>();
	Ptr<Node> n4 = CreateObject<Node>();
	Ptr<Node> n5 = CreateObject<Node>();
	Ptr<Node> n6 = CreateObject<Node>();
	Ptr<Node> n7 = CreateObject<Node>();
	Ptr<Node> n8 = CreateObject<Node>();
	Ptr<Node> n9 = CreateObject<Node>();
	Ptr<Node> n10 = CreateObject<Node>();
	Ptr<Node> n11= CreateObject<Node>();
	Ptr<Node> n12 = CreateObject<Node>();
	Ptr<Node> n13 = CreateObject<Node>();
	Ptr<Node> n14 = CreateObject<Node>();
	Ptr<Node> n15 = CreateObject<Node>();
	Ptr<Node> n16 = CreateObject<Node>();
	Ptr<Node> n17 = CreateObject<Node>();
	Ptr<Node> n18 = CreateObject<Node>();
	Ptr<Node> n19 = CreateObject<Node>();
	Ptr<Node> n20 = CreateObject<Node>();
	Ptr<Node> n21 = CreateObject<Node>();
	Ptr<Node> n22 = CreateObject<Node>();
	Ptr<Node> sink = CreateObject<Node>();
	//ground floor
	Ptr<Node> n24 = CreateObject<Node>();
	Ptr<Node> n25 = CreateObject<Node>();
	Ptr<Node> n26 = CreateObject<Node>();
	Ptr<Node> n27 = CreateObject<Node>();
	Ptr<Node> n28 = CreateObject<Node>();
	Ptr<Node> n29 = CreateObject<Node>();
	Ptr<Node> n30 = CreateObject<Node>();
	Ptr<Node> n31 = CreateObject<Node>();
	Ptr<Node> n32 = CreateObject<Node>();
	Ptr<Node> n33 = CreateObject<Node>();
	Ptr<Node> n34= CreateObject<Node>();
	Ptr<Node> n35 = CreateObject<Node>();
	Ptr<Node> n36 = CreateObject<Node>();
	Ptr<Node> n37 = CreateObject<Node>();
	Ptr<Node> n38 = CreateObject<Node>();
	Ptr<Node> n39 = CreateObject<Node>();
	
	NodeContainer nodes;
	Node::ChecksumEnabled();
	
	nodes.Add(n1);
	nodes.Add(n2);
	nodes.Add(n3);
	nodes.Add(n4);
	nodes.Add(n5);
	nodes.Add(n6);
	nodes.Add(n7);
	nodes.Add(n8);
	nodes.Add(n9);
	nodes.Add(n10);
	nodes.Add(n11);
	nodes.Add(n12);
	nodes.Add(n13);
	nodes.Add(n14);
	nodes.Add(n15);
	nodes.Add(n16);
	nodes.Add(n17);
	nodes.Add(n18);
	nodes.Add(n19);
	nodes.Add(n20);
	nodes.Add(n21);
	nodes.Add(n22);
	nodes.Add(sink);
	nodes.Add(n24);
	nodes.Add(n25);
	nodes.Add(n26);
	nodes.Add(n27);
	nodes.Add(n28);
	nodes.Add(n29);
	nodes.Add(n30);
	nodes.Add(n31);
	nodes.Add(n32);
	nodes.Add(n33);
	nodes.Add(n34);
	nodes.Add(n35);
	nodes.Add(n36);
	nodes.Add(n37);
	nodes.Add(n38);
	nodes.Add(n39);
	NS_LOG_INFO("CreatenodeContainer");
	return nodes;
	
}


/*std::vector<Ptr<LrWpanNetDevice> > CreateLrwpanDeviceVector(){
	NS_LOG_UNCOND("CreateLrwpanDeviceVector");
	
		std::vector<Ptr<LrWpanNetDevice> >lrwpanDevices;
	
//1st floor
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
	//ground floor
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
	//first floor
	lrwpanDevices.push_back(dev1);
	lrwpanDevices.push_back(dev2);
	lrwpanDevices.push_back(dev3);
	lrwpanDevices.push_back(dev4);
	lrwpanDevices.push_back(dev5);
	lrwpanDevices.push_back(dev6);
	lrwpanDevices.push_back(dev7);
	lrwpanDevices.push_back(dev8);
	lrwpanDevices.push_back(dev9);
	lrwpanDevices.push_back(dev10);
	lrwpanDevices.push_back(dev11);
	lrwpanDevices.push_back(dev12);
	lrwpanDevices.push_back(dev13);
	lrwpanDevices.push_back(dev14);
	lrwpanDevices.push_back(dev15);
	lrwpanDevices.push_back(dev16);
	lrwpanDevices.push_back(dev17);
	lrwpanDevices.push_back(dev18);
	lrwpanDevices.push_back(dev19);
	lrwpanDevices.push_back(dev20);
	lrwpanDevices.push_back(dev21);
	lrwpanDevices.push_back(dev22);
	lrwpanDevices.push_back(dev23);
	//gruond floor
	lrwpanDevices.push_back(dev24);
	lrwpanDevices.push_back(dev25);
	lrwpanDevices.push_back(dev26);
	lrwpanDevices.push_back(dev27);
	lrwpanDevices.push_back(dev28);
	lrwpanDevices.push_back(dev29);
	lrwpanDevices.push_back(dev30);
	lrwpanDevices.push_back(dev31);
	lrwpanDevices.push_back(dev32);
	lrwpanDevices.push_back(dev33);
	lrwpanDevices.push_back(dev34);
	lrwpanDevices.push_back(dev35);
	lrwpanDevices.push_back(dev36);
	lrwpanDevices.push_back(dev37);
	lrwpanDevices.push_back(dev38);
	

return 	lrwpanDevices;
	
	
	
	
	
	
}*/

/*NetDeviceContainer CreateLrwpanDevices(){
	NS_LOG_UNCOND("CreateLrwpanDevice");

	NetDeviceContainer lrwpanDevices;
	
//1st floor
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
	//ground floor
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
	//first floor
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
	//gruond floor
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
	

return 	lrwpanDevices;
}*/


