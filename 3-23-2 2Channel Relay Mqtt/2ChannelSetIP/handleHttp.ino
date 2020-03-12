/** Handle root or redirect to captive portal */

void handleRoot() {
  String s; 
  s="<meta name='viewport' content='width=device-width, initial-scale=1.0'/>";
  //s=s+"<meta http-equiv='refresh' content='5'/>";
  s=s+"<meta http-equiv='Content-Type' content='text/html;charset=utf-8' />";
  s=s+"<h1>김동일 웹서버 i2r.link</h1>";
  
  if(relay[0]==1)
    s=s+"<a href=\"relay0\"><button style='background-color:Lime; color:blue;'>릴레이0 on</button></a>";
  else 
    s=s+"<a href=\"relay0\"><button style='background-color:DarkGreen; color:white;'>릴레이0 off</button></a>";
  s=s+"&nbsp&nbsp";

  if(relay[1]==1)
    s=s+"<a href=\"relay1\"><button style='background-color:Lime; color:blue;'>릴레이1 on</button></a>";
  else 
    s=s+"<a href=\"relay1\"><button style='background-color:DarkGreen; color:white;'>릴레이1 off</button></a>";
  s=s+"&nbsp&nbsp";
  server.send(200, "text/html", s);
}

void handleRelay0() {
  if(relay[0]==1) 
    relay[0]=0;
  else
    relay[0]=1;

  relayDo();
  GoHome();
}

void handleRelay1() {
  if(relay[1]==1) 
    relay[1]=0;
  else
    relay[1]=1;

  relayDo();
  GoHome();
}


void handleOn() {
  digitalWrite(led, 0);
  GoHome();
}

void handleOff() {
  digitalWrite(led, 1);
  GoHome();
}

void GoHome() {
  String s,ipS;
  IPAddress ip;
  ip = WiFi.localIP();
  
  ipS=String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3]);
    s="<meta http-equiv='refresh' content=\"0;url='http://"+ipS+"/'\">";
  server.send(200, "text/html", s);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleScan() {
  String s;
  s="{\"chip\":\""+sChipID+"\",\"ip\":\""+WiFi.localIP().toString()+"\"}";
  server.send(200, "text/html", s);
}
