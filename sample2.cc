#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/stats-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/config-store-module.h"
#include "ns3/energy-module.h"
#include "ns3/internet-module.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Main");

class Experiment
{
public:
  Experiment ();
  Experiment (std::string name);
  Gnuplot2dDataset Run (const WifiHelper &wifi, const YansWifiPhyHelper &wifiPhy,
                        const WifiMacHelper &wifiMac, const YansWifiChannelHelper &wifiChannel);
private:
  void ReceivePacket (Ptr<Socket> socket);
  void SetPosition (Ptr<Node> node, Vector position);
  Vector GetPosition (Ptr<Node> node);
  void AdvancePosition (Ptr<Node> node);
  Ptr<Socket> SetupPacketReceive (Ptr<Node> node);

  uint32_t m_bytesTotal;
  Gnuplot2dDataset m_output;
};

Experiment::Experiment ()
{
}

Experiment::Experiment (std::string name)
  : m_output (name)
{
  m_output.SetStyle (Gnuplot2dDataset::LINES);
}

void
Experiment::SetPosition (Ptr<Node> node, Vector position)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  mobility->SetPosition (position);
}

Vector
Experiment::GetPosition (Ptr<Node> node)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel> ();
  return mobility->GetPosition ();
}

void 
Experiment::AdvancePosition (Ptr<Node> node) 
{
  Vector pos = GetPosition (node);
  double mbs = ((m_bytesTotal * 8.0) / 1000000);
  m_bytesTotal = 0;
  m_output.Add (pos.x, mbs);
  m_output.Add (pos.y, mbs);
  pos.x += 1.0;
  pos.y += 2.0;
  if (pos.x >= 600.0) 
    {
      return;
    }
  if (pos.y >= 900.0) 
    {
      return;
    }
  SetPosition (node, pos);
  //std::cout << "x="<<pos.x << std::endl;
  Simulator::Schedule (Seconds (1.5), &Experiment::AdvancePosition, this, node);
}

void
Experiment::ReceivePacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  while ((packet = socket->Recv ()))
    {
      m_bytesTotal += packet->GetSize ();
    }
}

Ptr<Socket>
Experiment::SetupPacketReceive (Ptr<Node> node)
{
  TypeId tid = TypeId::LookupByName ("ns3::PacketSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  sink->Bind ();
  sink->SetRecvCallback (MakeCallback (&Experiment::ReceivePacket, this));
  return sink;
}
/// Trace function for remaining energy at node.
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Current remaining energy = " << remainingEnergy << "J");
}

/// Trace function for total energy consumption at node.
void
TotalEnergy (double oldValue, double totalEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy consumed by node = " << totalEnergy << "J");
}

//****

Gnuplot2dDataset
Experiment::Run (const WifiHelper &wifi, const YansWifiPhyHelper &wifiPhy,
                 const WifiMacHelper &wifiMac, const YansWifiChannelHelper &wifiChannel)
{
  m_bytesTotal = 0;

  NodeContainer c;
  c.Create (16);

  PacketSocketHelper packetSocket;
  packetSocket.Install (c);

  YansWifiPhyHelper phy = wifiPhy;
  phy.SetChannel (wifiChannel.Create ());

  WifiMacHelper mac = wifiMac;
  NetDeviceContainer devices = wifi.Install (phy, mac, c);

  NS_LOG_INFO ("Enabling OLSR routing on all backbone nodes");
  OlsrHelper olsr;
  

  MobilityHelper mobility;
 /* mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (20.0),
                                 "MinY", DoubleValue (20.0),
                                 "DeltaX", DoubleValue (20.0),
                                 "DeltaY", DoubleValue (20.0),
                                 "GridWidth", UintegerValue (5),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::RandomDirection2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-500, 500, -500, 500)),
                             "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=2]"),
                             "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.2]"));
  mobility.Install (c);
*/
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (100.0, 0.0, 0.0));//node 0(mobile)
  positionAlloc->Add (Vector (90.0, 10.0, 0.0));//node1 e1
  positionAlloc->Add (Vector (110.0, 10.0, 0.0));//node2 e2
  positionAlloc->Add (Vector (115.0, -10.0, 0.0));//node3 e3
  positionAlloc->Add (Vector (100.0, -10.0, 0.0));//node4 e4
  positionAlloc->Add (Vector (90.0, -10.0, 0.0));//node5 e5

  positionAlloc->Add (Vector (75.0, -35.0, 0.0));//node6
  positionAlloc->Add (Vector (95.0, -30.0, 0.0));//node7
  positionAlloc->Add (Vector (105.0, -25.0, 0.0));//node8
  positionAlloc->Add (Vector (85.0, -10.0, 0.0));//node9
positionAlloc->Add (Vector (70.0, 0.0, 0.0));//node10
positionAlloc->Add (Vector (55.0, -10.0, 0.0));//node11
positionAlloc->Add (Vector (60.0, -20.0, 0.0));//node12
positionAlloc->Add (Vector (110.0, -30.0, 0.0));//node13
positionAlloc->Add (Vector (95.0, -40.0, 0.0));//node14
positionAlloc->Add (Vector (85.0,-30.0, 0.0));//node15
/*positionAlloc->Add (Vector (17.0, 0.0, 0.0));//node16
positionAlloc->Add (Vector (0.0, 2.0, 0.0));//node17
positionAlloc->Add (Vector (5.0, 0.0, 0.0));//node18
positionAlloc->Add (Vector (0.0, 16.0, 0.0));//node19*/

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (c);

 /** Energy Model **/
  /***************************************************************************/
  /* energy source */
  BasicEnergySourceHelper basicSourceHelper;
  // configure energy source
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (100.0));
  // install source
  EnergySourceContainer sources = basicSourceHelper.Install (c);
  /* device energy model */
  WifiRadioEnergyModelHelper radioEnergyHelper;
  // configure radio energy model
  radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.000001));
  // install device model
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (devices, sources);
  /***************************************************************************/

 
//****
  PacketSocketAddress socket;
  socket.SetSingleDevice (devices.Get (1)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (2)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (3)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (4)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (5)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (6)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (7)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (8)->GetIfIndex ());
  socket.SetSingleDevice (devices.Get (9)->GetIfIndex ());

socket.SetSingleDevice (devices.Get (10)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (11)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (12)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (13)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (14)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (15)->GetIfIndex ());
/*socket.SetSingleDevice (devices.Get (1)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (17)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (18)->GetIfIndex ());
socket.SetSingleDevice (devices.Get (19)->GetIfIndex ());*/

  socket.SetPhysicalAddress (devices.Get (1)->GetAddress ());
  socket.SetProtocol (1);

  socket.SetPhysicalAddress (devices.Get (2)->GetAddress ());
  socket.SetProtocol (2);

  socket.SetPhysicalAddress (devices.Get (3)->GetAddress ());
  socket.SetProtocol (3);

  socket.SetPhysicalAddress (devices.Get (4)->GetAddress ());
  socket.SetProtocol (4);

  socket.SetPhysicalAddress (devices.Get (0)->GetAddress ());
  socket.SetProtocol (0);



  OnOffHelper onoff ("ns3::PacketSocketFactory", Address (socket));
  onoff.SetConstantRate (DataRate (6000));
  onoff.SetAttribute ("PacketSize", UintegerValue (200));
//data transfer start
  ApplicationContainer apps = onoff.Install (c.Get (1));

  apps.Start (Seconds (0.5));
  apps.Stop (Seconds (250.0));

ApplicationContainer apps1 = onoff.Install (c.Get (2));

  apps1.Start (Seconds (0.10));
  apps1.Stop (Seconds (250.0));

ApplicationContainer apps2 = onoff.Install (c.Get (3));

  apps2.Start (Seconds (0.15));
  apps2.Stop (Seconds (250.0));

ApplicationContainer apps3 = onoff.Install (c.Get (4));

  apps3.Start (Seconds (0.10));
  apps3.Stop (Seconds (250.0));

ApplicationContainer apps4 = onoff.Install (c.Get (5));

  apps4.Start (Seconds (0.5));
  apps4.Stop (Seconds (250.0));

ApplicationContainer apps5 = onoff.Install (c.Get (6));

  apps5.Start (Seconds (0.10));
  apps5.Stop (Seconds (250.0));

ApplicationContainer apps6 = onoff.Install (c.Get (7));

  apps6.Start (Seconds (0.15));
  apps6.Stop (Seconds (250.0));

ApplicationContainer apps7 = onoff.Install (c.Get (8));

  apps7.Start (Seconds (0.5));
  apps7.Stop (Seconds (250.0));

ApplicationContainer apps8 = onoff.Install (c.Get (9));

  apps8.Start (Seconds (0.10));
  apps8.Stop (Seconds (250.0));

ApplicationContainer apps9 = onoff.Install (c.Get (10));

  apps9.Start (Seconds (0.15));
  apps9.Stop (Seconds (250.0));

ApplicationContainer apps10 = onoff.Install (c.Get (11));

  apps10.Start (Seconds (0.5));
  apps10.Stop (Seconds (250.0));

ApplicationContainer apps11 = onoff.Install (c.Get (12));

  apps11.Start (Seconds (0.10));
  apps11.Stop (Seconds (250.0));

ApplicationContainer apps12 = onoff.Install (c.Get (13));

  apps12.Start (Seconds (0.15));
  apps12.Stop (Seconds (250.0));

ApplicationContainer apps13 = onoff.Install (c.Get (14));

  apps13.Start (Seconds (0.5));
  apps13.Stop (Seconds (250.0));

ApplicationContainer apps14 = onoff.Install (c.Get (15));

  apps14.Start (Seconds (0.9));
  apps14.Stop (Seconds (250.0));

/*ApplicationContainer apps15 = onoff.Install (c.Get (16));

  apps15.Start (Seconds (0.13));
  apps15.Stop (Seconds (250.0));

ApplicationContainer apps16 = onoff.Install (c.Get (17));

  apps16.Start (Seconds (0.5));
  apps16.Stop (Seconds (250.0));

ApplicationContainer apps17 = onoff.Install (c.Get (18));

  apps17.Start (Seconds (0.10));
  apps17.Stop (Seconds (250.0));

ApplicationContainer apps18 = onoff.Install (c.Get (19));

  apps18.Start (Seconds (0.15));
  apps18.Stop (Seconds (250.0));*/


// (mobility)
  /*Simulator::Schedule (Seconds (1.5), &Experiment::AdvancePosition, this, c.Get (1));
  Ptr<Socket> recvSink = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (2.5), &Experiment::AdvancePosition, this, c.Get (2));
  Ptr<Socket> recvSink1 = SetupPacketReceive (c.Get (2));


Simulator::Schedule (Seconds (0.5), &Experiment::AdvancePosition, this, c.Get (3));
  Ptr<Socket> recvSink2 = SetupPacketReceive (c.Get (3));

Simulator::Schedule (Seconds (1.5), &Experiment::AdvancePosition, this, c.Get (4));
  Ptr<Socket> recvSink3 = SetupPacketReceive (c.Get (4));

Simulator::Schedule (Seconds (2.5), &Experiment::AdvancePosition, this, c.Get (5));
  Ptr<Socket> recvSink4 = SetupPacketReceive (c.Get (5));*/


Simulator::Schedule (Seconds (1.5), &Experiment::AdvancePosition, this, c.Get (6));
  Ptr<Socket> recvSink5 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (0.6), &Experiment::AdvancePosition, this, c.Get (7));
  Ptr<Socket> recvSink6 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (0.8), &Experiment::AdvancePosition, this, c.Get (8));
  Ptr<Socket> recvSink7 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (0.9), &Experiment::AdvancePosition, this, c.Get (9));
  Ptr<Socket> recvSink8 = SetupPacketReceive (c.Get (1));


Simulator::Schedule (Seconds (1.0), &Experiment::AdvancePosition, this, c.Get (10));
  Ptr<Socket> recvSink9 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (2.0), &Experiment::AdvancePosition, this, c.Get (11));
  Ptr<Socket> recvSink10 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (1.2), &Experiment::AdvancePosition, this, c.Get (12));
  Ptr<Socket> recvSink11 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (1.4), &Experiment::AdvancePosition, this, c.Get (13));
  Ptr<Socket> recvSink12 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (1.5), &Experiment::AdvancePosition, this, c.Get (14));
  Ptr<Socket> recvSink13 = SetupPacketReceive (c.Get (1));

Simulator::Schedule (Seconds (1.0), &Experiment::AdvancePosition, this, c.Get (15));
  Ptr<Socket> recvSink14 = SetupPacketReceive (c.Get (1));

/*Simulator::Schedule (Seconds (5.5), &Experiment::AdvancePosition, this, c.Get (16));
  Ptr<Socket> recvSink15 = SetupPacketReceive (c.Get (16));

Simulator::Schedule (Seconds (4.5), &Experiment::AdvancePosition, this, c.Get (17));
  Ptr<Socket> recvSink16 = SetupPacketReceive (c.Get (17));

Simulator::Schedule (Seconds (10.5), &Experiment::AdvancePosition, this, c.Get (18));
  Ptr<Socket> recvSink17 = SetupPacketReceive (c.Get (18));

Simulator::Schedule (Seconds (20.5), &Experiment::AdvancePosition, this, c.Get (19));
  Ptr<Socket> recvSink18 = SetupPacketReceive (c.Get (19));

*/



  /** Internet stack **/
/*  InternetStackHelper internet;
  internet.Install (networkNodes);

  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (networkNodes.Get (1), tid);  // node 1, receiver
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

  Ptr<Socket> source = Socket::CreateSocket (networkNodes.Get (0), tid);    // node 0, sender
  InetSocketAddress remote = InetSocketAddress (Ipv4Address::GetBroadcast (), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);*/


/** connect trace sources **/
  /***************************************************************************/
  // all sources are connected to node 1
  // energy source
  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (1));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

/*Ptr<BasicEnergySource> basicSourcePtr1 = DynamicCast<BasicEnergySource> (sources.Get (1));
  basicSourcePtr1->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));
  
Ptr<BasicEnergySource> basicSourcePtr2 = DynamicCast<BasicEnergySource> (sources.Get (1));
  basicSourcePtr2->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));
  
Ptr<BasicEnergySource> basicSourcePtr3 = DynamicCast<BasicEnergySource> (sources.Get (1));
  basicSourcePtr3->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));*/
  
  // device energy model
  Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr != NULL);
  basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));

/*Ptr<DeviceEnergyModel> basicRadioModelPtr1 = basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr1 != NULL);
  basicRadioModelPtr1->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));
  
Ptr<DeviceEnergyModel> basicRadioModelPtr2 = basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr2 != NULL);
  basicRadioModelPtr2->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));
  
Ptr<DeviceEnergyModel> basicRadioModelPtr3 = basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
  NS_ASSERT (basicRadioModelPtr3 != NULL);
  basicRadioModelPtr3->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));*/
    
/***************************************************************************/



  Simulator::Run ();

  Simulator::Destroy ();

  return m_output;
}

int main (int argc, char *argv[])
{
  // disable fragmentation
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));

  CommandLine cmd;
  cmd.Parse (argc, argv);

  Gnuplot gnuplot = Gnuplot ("reference-rates.png");

  Experiment experiment;
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211a);
  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  Gnuplot2dDataset dataset;

  wifiMac.SetType ("ns3::AdhocWifiMac");

  NS_LOG_DEBUG ("54");
  experiment = Experiment ("54mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate54Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("48");
  experiment = Experiment ("48mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate48Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("36");
  experiment = Experiment ("36mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate36Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("24");
  experiment = Experiment ("24mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate24Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("18");
  experiment = Experiment ("18mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate18Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("12");
  experiment = Experiment ("12mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate12Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("9");
  experiment = Experiment ("9mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate9Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("6");
  experiment = Experiment ("6mb");
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("OfdmRate6Mbps"));
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  gnuplot.GenerateOutput (std::cout);


  gnuplot = Gnuplot ("rate-control.png");
  wifi.SetStandard (WIFI_PHY_STANDARD_holland);


  NS_LOG_DEBUG ("arf");
  experiment = Experiment ("arf");
  wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("aarf");
  experiment = Experiment ("aarf");
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("aarf-cd");
  experiment = Experiment ("aarf-cd");
  wifi.SetRemoteStationManager ("ns3::AarfcdWifiManager");
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("cara");
  experiment = Experiment ("cara");
  wifi.SetRemoteStationManager ("ns3::CaraWifiManager");
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("rraa");
  experiment = Experiment ("rraa");
  wifi.SetRemoteStationManager ("ns3::RraaWifiManager");
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  NS_LOG_DEBUG ("ideal");
  experiment = Experiment ("ideal");
  wifi.SetRemoteStationManager ("ns3::IdealWifiManager");
  dataset = experiment.Run (wifi, wifiPhy, wifiMac, wifiChannel);
  gnuplot.AddDataset (dataset);

  gnuplot.GenerateOutput (std::cout);

  return 0;
}
