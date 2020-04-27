#include <SPI.h>         
#include <Ethernet.h>

#if ARDUINO >= 100
#include <EthernetUdp.h>  // New from IDE 1.0
#else
#include <Udp.h>  
#endif  

// libraries for realtime clock
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 RTC;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x00, 0x11, 0x22, 0x33, 0xFB, 0x11 }; // Use your MAC address
byte ip[] = { 192, 168, 0, 10 };                      // no DHCP so we set our own IP address
byte subnet[] = { 255, 255, 255, 0 };                // subnet mask
byte gateway[] = { 192, 168, 0, 2 };                 // internet access via router

unsigned int localPort = 8888;             // local port to listen for UDP packets

// find your local ntp server https://www.pool.ntp.org/zone/europe or 
// https://support.ntp.org/bin/view/Servers/StratumTwoTimeServers
// byte timeServer[] = {192, 43, 244, 18}; // time.nist.gov NTP server
byte timeServer[] = {145, 238, 203, 10};    // ntp1.nl.net NTP server  

const int NTP_PACKET_SIZE= 48;             // NTP time stamp is in the first 48 bytes of the message

byte pb[NTP_PACKET_SIZE];                  // buffer to hold incoming and outgoing packets 

#if ARDUINO >= 100
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;    // New from IDE 1.0
#endif  


///////////////////////////////////////////
//
// SETUP
// 
void setup() 
{
  Serial.begin(9600);
  Serial.println("NTP2RTC 0.5");

  // start Ethernet and UDP

  Ethernet.begin(mac, ip);     // For when you are directly connected to the Internet.
  Udp.begin(localPort);
  Serial.println("network ...");

  // init RTC
  Wire.begin();
  RTC.begin();
  Serial.println("rtc ...");
  Serial.println();
}

///////////////////////////////////////////
//
// LOOP
// 
void loop()
{
  Serial.print("RTC before: ");
  PrintDateTime(RTC.now());
  Serial.println();

  // send an NTP packet to a time server
  sendNTPpacket(timeServer);

  // wait to see if a reply is available
  delay(1000);

  if ( Udp.available() ) {
    // read the packet into the buffer
#if ARDUINO >= 100
    Udp.read(pb, NTP_PACKET_SIZE);      // New from IDE 1.0,
#else
    Udp.readPacket(pb, NTP_PACKET_SIZE);
#endif  

    // NTP contains four timestamps with an integer part and a fraction part
    // we only use the integer part here
    unsigned long t1, t2, t3, t4;
    t1 = t2 = t3 = t4 = 0;
    for (int i=0; i< 4; i++)
    {
      t1 = t1 << 8 | pb[16+i];      
      t2 = t2 << 8 | pb[24+i];      
      t3 = t3 << 8 | pb[32+i];      
      t4 = t4 << 8 | pb[40+i];
    }

    // part of the fractional part
    // could be 4 bytes but this is more precise than the 1307 RTC 
    // which has a precision of ONE second
    // in fact one byte is sufficient for 1307 
    float f1,f2,f3,f4;
    f1 = ((long)pb[20] * 256 + pb[21]) / 65536.0;      
    f2 = ((long)pb[28] * 256 + pb[29]) / 65536.0;      
    f3 = ((long)pb[36] * 256 + pb[37]) / 65536.0;      
    f4 = ((long)pb[44] * 256 + pb[45]) / 65536.0;

    // NOTE:
    // one could use the fractional part to set the RTC more precise
    // 1) at the right (calculated) moment to the NEXT second! 
    //    t4++;
    //    delay(1000 - f4*1000);
    //    RTC.adjust(DateTime(t4));
    //    keep in mind that the time in the packet was the time at
    //    the NTP server at sending time so one should take into account
    //    the network latency (try ping!) and the processing of the data
    //    ==> delay (850 - f4*1000);
    // 2) simply use it to round up the second
    //    f > 0.5 => add 1 to the second before adjusting the RTC
    //   (or lower threshold eg 0.4 if one keeps network latency etc in mind)
    // 3) a SW RTC might be more precise, => ardomic clock :)


    // convert NTP to UNIX time, differs seventy years = 2208988800 seconds
    // NTP starts Jan 1, 1900
    // Unix time starts on Jan 1 1970.
    const unsigned long seventyYears = 2208988800UL;
    t1 -= seventyYears;
    t2 -= seventyYears;
    t3 -= seventyYears;
    t4 -= seventyYears;

    /*
    Serial.println("T1 .. T4 && fractional parts");
    PrintDateTime(DateTime(t1)); Serial.println(f1,4);
    PrintDateTime(DateTime(t2)); Serial.println(f2,4);
    PrintDateTime(DateTime(t3)); Serial.println(f3,4);
    */
    PrintDateTime(DateTime(t4)); Serial.println(f4,4);
    Serial.println();

    // Adjust timezone and DST... in my case substract 4 hours for Chile Time
    // or work in UTC?
    t4 -= (3 * 3600L);     // Notice the L for long calculations!!
    t4 += 1;               // adjust the delay(1000) at begin of loop!
    if (f4 > 0.4) t4++;    // adjust fractional part, see above
    RTC.adjust(DateTime(t4));

    Serial.print("RTC after : ");
    PrintDateTime(RTC.now());
    Serial.println();

    Serial.println("done ...");
    // endless loop 
    while(1);
  }
  else
  {
    Serial.println("No UDP available ...");
  }
  // wait 1 minute before asking for the time again
  // you don't want to annoy NTP server admin's
  delay(60000L); 
}

///////////////////////////////////////////
//
// MISC
// 
void PrintDateTime(DateTime t)
{
    char datestr[24];
    sprintf(datestr, "%04d-%02d-%02d  %02d:%02d:%02d  ", t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());
    Serial.print(datestr);  
}


// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(byte *address)
{
  // set all bytes in the buffer to 0
  memset(pb, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  pb[0] = 0b11100011;   // LI, Version, Mode
  pb[1] = 0;     // Stratum, or type of clock
  pb[2] = 6;     // Polling Interval
  pb[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  pb[12]  = 49; 
  pb[13]  = 0x4E;
  pb[14]  = 49;
  pb[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp: 
#if ARDUINO >= 100
  // IDE 1.0 compatible:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(pb,NTP_PACKET_SIZE);
  Udp.endPacket(); 
#else
  Udp.sendPacket( pb,NTP_PACKET_SIZE,  address, 123); //NTP requests are to port 123
#endif    

}
///////////////////////////////////////////
//
// End of program
//
