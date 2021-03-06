/* -*-	Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Orange Labs Network
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	 02111-1307	 USA
 *
 * Author: William DIEGO <william.diego@orange.com>
 */

#ifndef YOUTUBE_CONTROLLER
#define YOUTUBE_CONTROLLER

#include "ns3/event-id.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/address.h"
#include "ns3/socket.h"

//#include "-server.h"
//#include "-client.h"

#include <vector>
#include <deque>

using namespace std;

namespace ns3 {

/**
 * \brief HttpApplication Data Unit
 *
 * Each ADU represents data to be sent over the network by either the client or the
 * server of the TCP connection.  ADUs also specify when data should be
 * sent relative to either the reception of an ADU or the completed
 * sending of a previous ADU.
 */

struct YoutubeMessage
{
  typedef enum
  {
    REQUEST, RESPONSE
  } MessageType;

  MessageType messageType;              ///< REQUEST or RESPONSE
  uint32_t windowSizeClient;            ///< Initiator's window size.
  uint32_t windowSizeServer;            ///< Acceptor's window size.
  double requestPageGapTime;            ///< The amount of time the client has to wait to send the ADU of the next web page
  double requestObjectGapTime;          ///< The amount of time the client has to wait to send the ADU of the next object within one web page
  double serverDelayTime;               ///< The amount of time the client/server has to wait before sending this ADU after receiving an ADU from the other node.
  //deque<double> objects;        ///< Sequential list of web object
  //deque<double> requestObjectGapTime;        ///< Sequential list of web object
  double totalCurrentSize;            ///< This is the current total page size, it will not change with receiving ADUs
  uint32_t size;                        ///< Number of bytes to send.
  uint32_t page;
  //bool firstOjbect;                     ///< if the first object in a page or not
  bool YoutubeSent;                         ///< if the ADU is sent out or not
 bool
  operator == (const YoutubeMessage & rhs) const;
  bool
  operator != (const YoutubeMessage & rhs) const;
};

/**
 * An ADU container is a container of all the ADU generated based on distribution functions.
 * Each connection vector has several fields which specify the type of
 * connection (sequential or concurrent), connection start time, loss
 * rate, window size, etc.  Each connection vector also contains an
 * arbitrary number of application data units (ADUs).
 *
 * \see ADU
 */


struct YoutubeSite
{
  double startTime;                     ///< The absolute time at which each page is scheduled to begin.
  double serverReceiveTime;             ///< The previous time we sent server ADU
  double clientSentTime;                ///< The previous time we sent client ADU
  double requestPageGaps;        ///< The deque to save all the page gaps
  deque<YoutubeMessage> objects;                   ///< Sequential list of web object
  deque<YoutubeMessage> request;                   ///< Sequential list of web object

  //deque< deque<YoutubeMessaje> > pages;            ///< Sequential list of pages.
  double remainingChunks;           ///< This is the current page size
  //deque< double > totalPageSize;      ///< The data structure contains all the page sizes
  //double totalCurrentPage;            ///< This is the current total page size, it will not change with receiving ADUs
  double totalChunks;
  double totalRecived;            ///< This is the current total page size, it will not change with receiving ADUs
  double chunkRecived;            ///< This is the current total page size, it will not change with receiving ADUs
  double timeSendChunk;
  double timeReceptionChunk;
  bool	firstObject;
  double  remainingPageSize;
  double  totalPageSize;
  uint32_t pageNumber;
  uint32_t windowSize;            ///< Initiator's window size.
  void
  Clear ();

};

/**
 * \addtogroup Youtube controller
 *
 * Class ns3::YoutubeController can be used as a base class for ns3 Youtube
 * client and server, it provides the basic functionalities for them
 */

/**
* \brief The base class for all ns3 applications
*
*/
class YoutubeController : public Object
{
public:
  static TypeId GetTypeId (void);
  /**
   * \brief Constructor.
   */
  YoutubeController ();
  /**
   * \brief Destructor.
   */
  ~YoutubeController ();
  /**
   *\brief Set both the server and client contaienrs
   *
   * Set the server and client container in the controller, the server and client application
   * use the container saved in controller only
   */
  void SetServerSite (const YoutubeSite & site);
  void SetClientSite (const YoutubeSite & site);
  /**
   * \brief Get the server or client container for use in web server or web client
   */
  YoutubeSite & GetClientSite ();
  YoutubeSite & GetServerSite ();

  void StartVideo (Ptr<Socket> socket);//, Ptr<YoutubeServer>& server);


  //======================

//  void SetServerSocket (Ptr<Socket> socket, Address local);

  void StartNewClientSocket ();
  void StartNewServerSocket ();

  void SetClientApplication (Ptr<Node> node, uint32_t id);
  void SetServerApplication (Ptr<Node> node, uint32_t id);

  //void StartFirstWebSession ();

  void CallClientSend ();

  void SetClientSocket (Ptr<Socket> socket);
  void SetServerSocket (Ptr<Socket> socket);
  /**
   * \brief Call the timeout function for each web page.
   *
   * When the transmission time for one web page has passed
   * that value, time out that transmission, record the results, and start next page transmission.  Note that
   * we do keep the same transport connection open
   */
  void CallTimeOut (Ptr<Socket> socket);
  /**
   * \brief Call the first object send from client
   */
  void FirstObjectClientSend (Ptr<Socket> socket, double delay);
  /**
   * \brief Performs the actual send of data through the socket.
   *
   * May be scheduled or called directly from ScheduleNextSend.
   */
  void ClientSend (Ptr<Socket> socket);
  /**
   * \brief Performs the actual send of data through the socket.
   *
   * May be scheduled or called directly from ScheduleNextSend.
   */
  void ServerSend (Ptr<Socket> socket);
  /**
   * \brief Schedule the next client send through the socket.
   *
   * This function calls the client send or server send depending on the conditions.
   */
  void ScheduleNextClientSend (Ptr<Socket> socket);
  /**
   * \brief Schedule the next server send through the socket.
   *
   * This function calls the server send depending on the conditions.
   * Server would only care about the response size, the request size in the ADU container is only used
   * to test if the received request ADU is the right size or not.
   */
  void ScheduleNextServerSend (Ptr<Socket> socket);
  /**
   * \brief Find and remove the next web request.
   * \param container The ADU container to operate on
   *
   * Find the next request and mark it as sent and then remove that request
   */
  /**
   * \brief Check the next request.
   * \param container The ADU container to operate on
   *
   * Check the next request and mark it as sent
   */
  bool CheckNextRequest (YoutubeSite & site);
  /**
   * \brief Set if we are working on pipelining or not.
   * \param pipelining The boolean value for pipelining
   * \param persistent The boolean value for persistent connection
   */
  void SetYoutubeVersion (bool pipelining, bool persistent);
  /**
   * \brief Cleanup the Adu containers.
   */
  void Cleanup ();

private:

  void CallControllerUser (void);
  /**
   * \brief Send more data as soon as some has been transmitted.
   */
  void DataSendUser (Ptr<Socket>, uint32_t); // for socket's SetSendCallback

  void CallControllerServer (void);
  /**
   * \brief Send more data as soon as some has been transmitted.
   */
  void DataSendServer (Ptr<Socket>, uint32_t); // for socket's SetSendCallback


  Ptr<Socket>                   m_clientSocket;                     ///< The client socket to work on
  Ptr<Socket>                   m_serverSocket;                     ///< The client socket to work on
//  Address m_local;

  uint32_t m_serverId;    ///< server application id
  Ptr<Node> m_serverNode; ///< the server node

  uint32_t m_clientId;    ///< client application id
  Ptr<Node> m_clientNode; ///< the server node

  bool m_persistent;            ///< notify if the connection is persistent connection or not

  /**
   * \brief Maximum size of packets to send.
   *
   * Larger ADUs will be broken up into chunks of no more than this many of bytes.
   *
   * Default: 1460 bytes.
   */
  uint32_t                      m_segmentSize;
  /**
   * The ADU container for both server and client
   */
  YoutubeSite                 		m_serverSite;
  YoutubeSite                  	m_clientSite;
  double                        m_delayTime;                        ///< Used to record the delay time to send
  bool                          m_pipelining;                       ///< Whether this transaction is pipelining or not
};

} // namespace ns3

#endif /* YOUTUBE_CONTROLLER */
