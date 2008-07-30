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
 * \file Settings.cpp
 * \author Juan-Pablo Caceres
 * \date July 2008
 */

#include "Settings.h"
#include "globals.h"

#include <iostream>
#include <getopt.h> // for command line parsing

using std::cout; using std::endl;

int gVerboseFlag = 0;


//*******************************************************************************
Settings::Settings() :
  mNumInChans(gDefaultNumInChannels),
  mNumOutChans(gDefaultNumOutChannels),
  mBitResolutionMode(gDefaultBitResolutionMode),
  mQueueLength(gDefaultQueueLength),
  mLoopBack(false)
{}


//*******************************************************************************
void Settings::parseInput(int argc, char** argv)
{
  // If no command arguments are given, print instructions
  if(argc == 1) {
    printUsage();
    std::exit(0);
  }

  // Usage example at:
  // http://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example
  // options descriptor
  //----------------------------------------------------------------------------
  static struct option longopts[] = {
    // These options set a flag, has to be sepcified as a long option --verbose
    { "verbose", no_argument, &gVerboseFlag, 1 },
    // These options don't set a flag.
    { "numchannels", required_argument, NULL, 'n' }, // Number of input and output channels
    { "server", no_argument, NULL, 's' }, // Run in server mode
    { "client", required_argument, NULL, 'c' }, // Run in client mode, set server IP address
    { "queue", required_argument, NULL, 'q' }, // Queue Length
    { "bitres", required_argument, NULL, 'b' }, // Audio Bit Resolution
    { "loopback", required_argument, NULL, 'l' }, // Run in loopback mode
    { "help", no_argument, NULL, 'h' }, // Print Help
    { NULL, 0, NULL, 0 }
  };

  // Parse Command Line Arguments
  //----------------------------------------------------------------------------
  /// \todo Specify mandatory arguments
  int ch;
  while ( (ch = getopt_long(argc, argv, "n:sc:q:b:lh", longopts, NULL)) != -1 )
    switch (ch) {
      
    case 'n':
      //-------------------------------------------------------
      mNumInChans = atoi(optarg);
      mNumOutChans = atoi(optarg);
      break;
    case 's':
      //-------------------------------------------------------
      /// \todo Implement this
      break;
    case 'c':
      //-------------------------------------------------------
      /// \todo Implement this
      mPeerHostOrIP = optarg;
      break;
    case 'b':
      //-------------------------------------------------------
      if      ( atoi(optarg) == 8 ) {
	mBitResolutionMode = JackAudioInterface::BIT8; }
      else if ( atoi(optarg) == 16 ) {
	mBitResolutionMode = JackAudioInterface::BIT16; }
      else if ( atoi(optarg) == 24 ) {
	mBitResolutionMode = JackAudioInterface::BIT16; }
      else if ( atoi(optarg) == 32 ) {
	mBitResolutionMode = JackAudioInterface::BIT32; }
      else {
	std::cerr << "--bitres ERROR: Wrong bit resolutions: " 
		  << atoi(optarg) << " is not supported." << endl;
	printUsage();
	std::exit(1); }
      break;
    case 'q':
      //-------------------------------------------------------
      if ( atoi(optarg) <= 0 ) {
	std::cerr << "--queue ERROR: The queue has to be a positive integer" << endl;
	printUsage();
	std::exit(1); }
      else {
	mQueueLength = atoi(optarg);
      }
      break;
    case 'l': //loopback
      //-------------------------------------------------------
      mLoopBack = true;
      break;
    case 'h':
      //-------------------------------------------------------
      printUsage();
      std::exit(0);
      break;
    default:
      //-------------------------------------------------------
      printUsage();
      std::exit(0);
      break;
    }

  // Warn user if undefined options where entered
  //----------------------------------------------------------------------------
  if (optind < argc) {
    cout << gPrintSeparator << endl;
    cout << "WARINING: The following entered options have no effect" << endl;
    cout << "          They will be ignored!" << endl;
    cout << "          Type jacktrip to see options." << endl;
    for( ; optind < argc; optind++) {
      printf("argument: %s\n", argv[optind]);
    }
    cout << gPrintSeparator << endl;
  }
}


//*******************************************************************************
void Settings::printUsage()
{
  cout << "" << endl;
  cout << "JackTrip: A System for High-Quality Audio Network Performance" << endl;
  cout << "over the Internet" << endl;
  cout << "Copyright (c) 2008 Juan-Pablo Caceres, Chris Chafe." << endl;
  cout << "SoundWIRE group at CCRMA, Stanford University" << endl;
  cout << "-----------------------------------------------------------------------------" << endl;
  cout << "" << endl;
  cout << "Usage: jacktrip [-s|-c host] [options]" << endl;
  cout << "" << endl;
  cout << "Options: " << endl;
  cout << " -n, --numchannels #                      Number of Input and Output Channels (default 2)" << endl;
  cout << " -s, --server                             Run in Server Mode" << endl;
  cout << " -c, --client      <peer_host_IP_or_name> Run in Client Mode" << endl;
  cout << " -q, --queue       # (1 or more)          Queue Buffer Length, in Packet Size (default 4)" << endl;
  cout << " -b, --bitres      # (8, 16 (Default), 24 or 32)    Audio Bit Rate Resolutions (default 16)" << endl;
  cout << " -l, --loopback                           Run in Loop-Back Mode" << endl;
  cout << " -h, --help                               Prints this help" << endl;
  cout << "" << endl;
}
