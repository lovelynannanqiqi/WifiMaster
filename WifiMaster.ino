
#define SSID       "netcore"
#define PASSWORD   "987656789"
#define UDPSERVER_PORT 8899
#define INIT_BUTTON_PIN 4
#define RELAY_PIN 6

#include "uartWIFI.h"
#include <SoftwareSerial.h>
WIFI wifi;
extern int chlID;	//client id(0-4)

String app_ssid;
String app_password;

void setup()
{
   pinMode(INIT_BUTTON_PIN, INPUT);
   pinMode(RELAY_PIN,OUTPUT);
   
   int aps;
   wifi.begin();
 
   //aps=splitAPList(showAP());
   //DebugSerial.println(String(aps));
   
   
   
   //linkRoute(SSID,PASSWORD);
   
   
   //isHaveConnectedRoute();    
   
   //DebugSerial.println(ping("www.baidu.com"));
   
   delay(5000);  
   //createUDPServer(UDPSERVER_PORT);
   createUDPServer("192.168.1.4",UDPSERVER_PORT,1);
   
   //TCP Server----------------------------------------//
   
   //wifi.confMux(1);
   //if(wifi.confServer(1,8088))
   //{
   //	DebugSerial.println("TCP Server is runing!");
   //}
   
   //-------------------------------------------------//
   
   
   //String ipstring  = wifi.showIP();
   //DebugSerial.println(ipstring);		//show the ip address of module
 
  

}
void loop()
{
 
  String recive;
  char buf[100];
  
  int iLen = wifi.ReceiveMessage(buf);
  int lineCodeIndex=-1;
  
  if(iLen > 0)
  {
    lineCodeIndex=String(buf).indexOf(char(13));
    recive=String(buf).substring(0,lineCodeIndex);
    wifi.Send(chlID,recive);
    DebugSerial.println(recive);
    
    if(getRouteId_Password(recive))
    {
       linkRoute(app_ssid,app_password);
    }


    if (strcmp(buf, "OPEN") == 0)
    {
        digitalWrite(RELAY_PIN,HIGH);
    }
    
    if (strcmp(buf, "CLOSE") == 0)
    {
        digitalWrite(RELAY_PIN,LOW);
    }
    
    
    if (strcmp(buf, "RESTORE") == 0)
    {
        wifi.Send(chlID,"RESTORE OK!");
        restore();
    }

    
    
  }
  
}



void createUDPServer(int port,int mode)
{
   wifi.closeMux(1);
   if(wifi.confMux(1)&&setUDPServer(1,getLocalIP(),port,port,mode))
   {
       DebugSerial.println("UDPServer is runing!");
       DebugSerial.println(getLocalIP());
   }
   else
   {
       DebugSerial.println("UDPServer create error!");
   }  
}

void createUDPServer(String ip,int port,int mode)
{
   wifi.closeMux(1);
   if(wifi.confMux(1)&&setUDPServer(1,ip,port,port,mode))
   {
       DebugSerial.println("UDPServer is runing!");
       DebugSerial.println(getLocalIP());
   }
   else
   {
       DebugSerial.println("UDPServer create error!");
   }  
}
String ping(String host)
{
  String result;
  _cell.flush();
  _cell.print("AT+PING=");
  _cell.print("\"");
  _cell.print(host);
  _cell.println("\"");
  _cell.flush(); 
  unsigned long start;
  start = millis();
  while (millis() - start<3000) {
    if (_cell.available()>0)
    {
        char a=_cell.read();
        result+=a;
    }
    if (result.indexOf("OK") != -1)
    {
        char head[4] = { 0x0D, 0x0A };
        
	result.replace("AT+PING", "");
        result.replace("OK", "");
        result.replace(head,"");
        return result.substring(result.indexOf("+"),result.indexOf("\n\a\n\a"));;
    }    
  }
  return "";
}
boolean restore()
{
  boolean result = false;
  _cell.flush();
  _cell.println("AT+RESTORE");	
  result = _cell.find("OK");
  if(result)
    return true;
  return false;
}
//password at least 8 char
//type 0=OPEN   2=WAP_PSK   3=WPA2_PSK  4=WPA_WPA2_PSK
boolean setAP(String ssid,String password,byte chl,byte type)
{
  boolean result = false;  
  _cell.print("AT+CWSAP=");
  _cell.print("\"");
  _cell.print(ssid);	
  _cell.print("\"");
  _cell.print(",");
  _cell.print("\"");
  _cell.print(password);	
  _cell.print("\"");
  _cell.print(",");
  _cell.print(chl);	
  _cell.print(",");
  _cell.println(type);	  
  
  result = _cell.find("OK");
  if(result)
    return true;
  return false;
}
//Config UDP Server
boolean setUDPServer(byte id, String addr, int port, int localPort,int mode)

{
        _cell.print("AT+CIPSTART=");
	_cell.print(String(id));
	_cell.print(",");
	_cell.print("\"UDP\"");
	_cell.print(",");
	_cell.print("\"");
	_cell.print(addr);
	_cell.print("\"");
	_cell.print(",");
	_cell.print(String(port));
	_cell.print(",");
	_cell.print(String(localPort));
        _cell.print(",");
        _cell.println(String(mode));  

        
	String data;
	unsigned long start;
	start = millis();
	while (millis() - start<3000) {
		if (_cell.available()>0)
		{
			char a = _cell.read();
			data = data + a;
		}
		if (data.indexOf("OK") != -1 || data.indexOf("ALREAY CONNECT") != -1)
		{
			return true;
		}
	}
	return false;
}


//Config UDP Server
boolean setUDPServer(String addr, int port, int localPort,int mode)

{
        _cell.print("AT+CIPSTART=");
	_cell.print("\"UDP\"");
	_cell.print(",");
	_cell.print("\"");
	_cell.print(addr);
	_cell.print("\"");
	_cell.print(",");
	_cell.print(String(port));
	_cell.print(",");
	_cell.print(String(localPort));
        _cell.print(",");
        _cell.println(String(mode));  

        
	String data;
	unsigned long start;
	start = millis();
	while (millis() - start<3000) {
		if (_cell.available()>0)
		{
			char a = _cell.read();
			data = data + a;
		}
		if (data.indexOf("OK") != -1 || data.indexOf("ALREAY CONNECT") != -1)
		{
			return true;
		}
	}
	return false;
}


String getLocalIP()
{
    String data;   
    _cell.flush();
    _cell.println("AT+CIPSTA?");  
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
       if(_cell.available()>0)
       {
           char a =_cell.read();
           data=data+a;
       }
       
       if (data.indexOf("OK")!=-1)
       {
           break;
       }
       
       if (data.indexOf("ERROR")!=-1)
       {
           return data;
       }
    }
    data=data.substring(data.indexOf("+CIPSTA:\"")+9);
    data=data.substring(0,data.indexOf("\""));
    //data.replace("\"","");
    return data;
}
//use to split the showAP result to single item
int splitAPList(String aplist)
{
   int indexNum=0;
   int startIndex=0;    
   while((startIndex=aplist.indexOf("+"))>-1)
   {
     DebugSerial.println(String(indexNum)+aplist.substring(startIndex,aplist.indexOf("\n")));
     aplist.setCharAt(aplist.indexOf("+"),'-');
     aplist.setCharAt(aplist.indexOf("\n"),'\a');
     indexNum++;
   }
   return indexNum;
}



String showAP()
{
   String aplist="+CWLAP:";
   _cell.flush();   
   _cell.println("AT+CWLAP");
   delay(1000);
   while(char(_cell.read())!=':');
   while(1)
   {
      if(_cell.available()>0)
      {
          char temp=_cell.read();
      
          if(aplist.indexOf("OK")>-1)
              return aplist; 
          else
            aplist+=temp;

      }              
   }
   return aplist;
}


void isHaveConnectedRoute()
{
  while(wifi.showJAP().indexOf("No")!=-1)
   {
    delay(300);
   }
}
void linkRoute(String ssid,String password)
{
  bool b = wifi.Initialize(AP_STA, ssid, password);
  if(!b)
  {
     DebugSerial.println("Initialize:ERROR");
  }
  else
  {
     DebugSerial.println("Initialize:OK");
  }

}


//info fromat "#<SSIDidPASSpassword>";
boolean getRouteId_Password(String info)
{
  // substring(index) looks for the substring from the index position to the end:
  if (info.substring(0,1) == "#") {
    String ssid=info.substring(info.indexOf("SSID")+4,info.indexOf("PASS"));
    String pass=info.substring(info.indexOf("PASS")+4,info.indexOf(">"));
    app_ssid=ssid;
    app_password=pass;
    return true;
  } 
  return false;
}
