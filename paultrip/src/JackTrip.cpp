//*****************************************************************
/*
  JackTrip: A System for High-Quality Audio Network Performance
  over the Internet

  Copyright (c) 2008 Juan-Pablo Caceres, Chris Chafe.
  SoundWIRE group at CCRMA, Stanford University.
  
  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/
//*****************************************************************

/**
 * \file JackTrip.cpp
 * \author Juan-Pablo Caceres
 * \date July 2008
 */

#include "JackTrip.h"
#include "UdpDataProtocol.h"
#include "globals.h"

#include <iostream>

using std::cout; using std::endl;

//*******************************************************************************
JackTrip::JackTrip(char* PeerHostOrIP, dataProtocolT DataProtocolType, int NumChans,
		   JackAudioInterface::audioBitResolutionT AudioBitResolution) :
  mNumChans(NumChans),
  mSampleRate(0),
  mAudioBufferSize(0),
  mDataProtocolSender(NULL),
  mDataProtocolReceiver(NULL),
  mJackAudio(NULL)
{
  // Create JackAudioInterface Client Object
  mJackAudio = new JackAudioInterface(mNumChans, mNumChans, AudioBitResolution);
  mSampleRate = mJackAudio->getSampleRate();
  std::cout << "The Sampling Rate is: " << mSampleRate << std::endl;
  std::cout << gPrintSeparator << std::endl;
  mAudioBufferSize = mJackAudio->getBufferSize();
  int AudioBufferSizeInBytes = mAudioBufferSize*sizeof(sample_t);
  std::cout << "The Audio Buffer Size is: " << mAudioBufferSize << " samples" << std::endl;
  std::cout << "                      or: " << AudioBufferSizeInBytes 
	    << " bytes" << std::endl;
  std::cout << gPrintSeparator << std::endl;

  // Create DataProtocol Objects
  switch (DataProtocolType) {
  case UDP:
    std::cout << "Using UDP Protocol" << std::endl;
    std::cout << gPrintSeparator << std::endl;
    //mDataProtocolSender = new UdpDataProtocol(DataProtocol::SENDER, PeerHostOrIP);
    //mDataProtocolReceiver =  new UdpDataProtocol(DataProtocol::RECEIVER, PeerHostOrIP);
    mDataProtocolSender = new UdpDataProtocol(DataProtocol::SENDER);
    mDataProtocolReceiver =  new UdpDataProtocol(DataProtocol::RECEIVER);
    mDataProtocolSender->setPeerAddress(PeerHostOrIP);
    mDataProtocolReceiver->setPeerAddress(PeerHostOrIP);
    break;
  case TCP:
    std::cerr << "ERROR: TCP Protocol is not unimplemented" << std::endl;
    exit(1);
    break;
  case SCTP:
    std::cerr << "ERROR: SCTP Protocol is not unimplemented" << std::endl;
    exit(1);
    break;
  default: 
    std::cerr << "ERROR: Protocol not defined or unimplemented" << std::endl;
    exit(1);
    break;
  }
  mDataProtocolSender->setAudioPacketSize(mJackAudio->getSizeInBytesPerChannel() * NumChans);
  mDataProtocolReceiver->setAudioPacketSize(mJackAudio->getSizeInBytesPerChannel() * NumChans);

  // Create RingBuffers with the apprioprate size
  /// \todo Make all this operations cleaner
  mSendRingBuffer.reset( new RingBuffer(mJackAudio->getSizeInBytesPerChannel() * NumChans, 4) );
  std::cout << "NEWED mSendRingBuffer" << std::endl;
  std::cout << gPrintSeparator << std::endl;
  mReceiveRingBuffer.reset( new RingBuffer(mJackAudio->getSizeInBytesPerChannel() * NumChans, 8) );
  std::cout << "NEWED mReceiveRingBuffer" << std::endl;
  std::cout << gPrintSeparator << std::endl;

  // Set RingBuffers pointers in protocols
  mDataProtocolSender->setRingBuffer(mSendRingBuffer);
  mDataProtocolReceiver->setRingBuffer(mReceiveRingBuffer);
  mJackAudio->setRingBuffers(mSendRingBuffer, mReceiveRingBuffer);
  //mJackAudio->setRingBuffers(mReceiveRingBuffer, mSendRingBuffer);


  /// \todo this is new
  mDataProtocolSender->fillHeaderCommonFromJack(*mJackAudio);

}


//*******************************************************************************
JackTrip::~JackTrip()
{
  delete mDataProtocolSender;
  delete mDataProtocolReceiver;
  delete mJackAudio;
}


//*******************************************************************************
void JackTrip::startThreads()
{
  mJackAudio->startProcess();
  mDataProtocolSender->start(QThread::TimeCriticalPriority);
  mDataProtocolReceiver->start(QThread::TimeCriticalPriority);
}


//*******************************************************************************
void JackTrip::appendProcessPlugin(const std::tr1::shared_ptr<ProcessPlugin> plugin)
{
  mJackAudio->appendProcessPlugin(plugin);
}
