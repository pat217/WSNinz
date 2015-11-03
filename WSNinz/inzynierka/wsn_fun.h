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


using namespace ns3;

extern uint8_t allReceived;
extern uint8_t retransmit;
extern uint8_t Seed;
extern double simulationEnd;
extern double simulationStart;
extern double delay;
extern uint32_t numPackets;
extern Time interPacketInterval;
extern uint32_t packetSize;
extern uint16_t port;
extern uint32_t NumOfPacketsRecvBySink;
extern uint32_t SeqReceivedBySink[39];
extern std::vector<int> SeqSent;
extern std::vector<Ptr<Socket> >sendSockets;	
extern std::vector<Ptr<Socket> >recvSockets;


extern void ReceivePacket (Ptr<Socket>dest, Ptr<Socket> socket);
extern void ReceiveDestiny (Ptr<Socket> socket);
extern void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval );
extern void sendFrom( Ptr<Socket> from,  uint32_t packetSize, Time when, uint32_t numPackets, Time interPacketInterval);
extern void retransmitFun( Ptr<Socket> from,  uint32_t packetSize, Time when, uint32_t numPackets, Time interPacketInterval);
extern void checkForRetransmission(Time when);
extern void checkIfAllArrived(Time when);
extern NodeContainer CreateNodeContainer();
extern NetDeviceContainer CreateLrwpanDevices();
extern void AssignMac16Address(NetDeviceContainer lrwpanDevices);
extern void SetChannelForDevices(std::vector<Ptr<LrWpanNetDevice> > lrwpanDevices, Ptr<SingleModelSpectrumChannel> channel);
extern void AddDeviceToNode(NetDeviceContainer lrwpanDevices, NodeContainer nodes);
extern void SetDistances(std::vector<Ptr<LrWpanNetDevice> > lrwpanDevices);
extern void ConfNodesRoutes(NodeContainer nodes, TypeId tid, Ipv6InterfaceContainer deviceInterfaces);
extern std::vector<Ptr<LrWpanNetDevice> > CreateLrwpanDeviceVector();

