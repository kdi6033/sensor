/** Handle root or redirect to captive portal */
void handleRoot() {
  //Serial.println("Server Html");
  //Serial.println ( WiFi.localIP() );
  String s; 
  String s1= String(ssid);
  s="<meta name='viewport' content='width=device-width, initial-scale=1.0'/>";
  //s=s+"<meta http-equiv='refresh' content='5'/>";
  s=s+"<meta http-equiv='Content-Type' content='text/html;charset=utf-8' />";
  s=s+"<h1>LG 에어컨 웹서버 i2r</h1>";
   s=s+"<br>김동일교수 idea to real<br>";
  s=s+"<br>에어컨   ";
  if(ac_power_on==1)
    s=s+"<a href=\"onoff\"><button style='background-color:Lime; color:blue;'>ON 상태</button></a>";
  else 
    s=s+"<a href=\"onoff\"><button style='background-color:DarkGreen; color:white;'>OFF 상태</button></a>";
  s=s+"<br><br>";

  s=s+"설정온도   "+ac_temperature+"      ";
  s=s+"<a href=\"temperatureup\"><button style='background-color:Green; color:white;'>증가   +   </button></a>";
  s=s+"        ";
  s=s+"<a href=\"temperaturedown\"><button style='background-color:Green; color:white;'>감소  -   </button></a>";
  s=s+"<br><br>";

  if(ac_flow==0)
    s=s+"바람세기   약     ";
  else if(ac_flow==1)
    s=s+"바람세기   중     ";
  else if(ac_flow==2)
    s=s+"바람세기   강     ";
  s=s+"<a href=\"airflowup\"><button style='background-color:Green; color:white;'>증가   +   </button></a>";
  s=s+"        ";
  s=s+"<a href=\"airflowdown\"><button style='background-color:Green; color:white;'>감소  -   </button></a>";
  s=s+"<br><br>";

  s=s+"바람 스윙   ";
  if(air_swing==1)
    s=s+"<a href=\"airswing\"><button style='background-color:Green; color:white;'>ON 상태</button></a>";
  else 
    s=s+"<a href=\"airswing\"><button style='background-color:Green; color:white;'>OFF 상태</button></a>";
  s=s+"<br><br>";
  
  s=s+"Air Clean   ";
  if(ac_air_clean_state==1)
    s=s+"<a href=\"airclean\"><button style='background-color:Green; color:white;'>ON 상태</button></a>";
  else 
    s=s+"<a href=\"airclean\"><button style='background-color:Green; color:white;'>OFF 상태</button></a>";
  s=s+"<br><br>";

  if(ac_heat==1)
    s=s+"<a href=\"coolheat\"><button style='background-color:Red; color:white;'>난방</button></a>";
  else 
    s=s+"<a href=\"coolheat\"><button style='background-color:Blue; color:white;'>냉방</button></a>";
  s=s+"     ";
  
  if(kAc_Type==0)
    s=s+"<a href=\"actype\"><button style='background-color:Green; color:white;'>타워형</button></a>";
  else 
    s=s+"<a href=\"actype\"><button style='background-color:Green; color:white;'>벽걸이형</button></a>";
  s=s+"<br><br>";
       
  s=s+"아래 공유기 이름과 주소가 연결되었으면 주소를 선택한 후에 설정에서 사용 하시는 인터넷 공유기를 선택하세요. <br>";
  s=s+"연결된 AP 이름과 IP : "+sAP_ssid+"  "+String(ssid)+"  "+"<p><a href='http://"+WiFi.localIP().toString()+"'/>"+WiFi.localIP().toString()+"</a></p>";
  s=s+"<p><a href='/wifi'>공유기를 바꾸려면 누르세요.</a></p>";
  server.send(200, "text/html", s); // Empty content inhibits Content-length header so we have to close the socket ourselves.
}

void GoHome() {
  IPAddress ip = WiFi.localIP();
  String ipS;
  ipS=String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3]);
  //String addy = server.client().remoteIP().toString();
  //Serial.println(addy);
  IPAddress ipClient = server.client().remoteIP();
  if(ipClient[2]==4) 
    s="<meta http-equiv='refresh' content=\"0;url='http://192.168.4.1/'\">";
  else
    s="<meta http-equiv='refresh' content=\"0;url='http://"+ipS+"/'\">";
  server.send(200, "text/html", s);
}

void handleScan() {
  String s="";
  s="{\"chipid\":\""+sChipID+"\","+"\"ip\":\""+WiFi.localIP().toString()+","+"\"class\":\"aircon\""+","
      +"\"power\":"+ac_power_on+"\"temper\":"+ac_temperature+"\"flow\":"+ac_flow+"\"swing\":"+air_swing
  +"\"clean\":"+ac_air_clean_state+"\"heat\":"+ac_heat +"}";
  server.send(200, "text/plain", s);
}

void handleAirSwing() {
  if(air_swing==0)
    air_swing=1;
  else
    air_swing=0;
  Ac_Change_Air_Swing(air_swing);
  GoHome();
  delay(100);
}

void handleAirflowUp() {
  ac_flow +=1;
  if(ac_flow>2)
    ac_flow = 2;
  if(ac_power_on==1);
    Ac_Activate(ac_temperature, ac_flow, ac_heat);
  GoHome();
  delay(100);
}

void handleAirflowDown() {
  if(ac_flow>0)
      ac_flow -=1;
  if(ac_flow<=0)
    ac_flow = 0;
  if(ac_power_on==1);
    Ac_Activate(ac_temperature, ac_flow, ac_heat);
  GoHome();
  delay(100);
}

void handleTemperatureUp() {
  ac_temperature +=1;
  if(ac_temperature>30)
    ac_temperature = 30;
  if(ac_power_on==1);
    Ac_Activate(ac_temperature, ac_flow, ac_heat);
  GoHome();
  delay(100);
}

void handleTemperatureDown() {
  ac_temperature -=1;
  if(ac_temperature<18)
    ac_temperature = 18;
  if(ac_power_on==1);
    Ac_Activate(ac_temperature, ac_flow, ac_heat);
  GoHome();
  delay(100);
}

void handleAirClean() {
  if(ac_air_clean_state==0)
    ac_air_clean_state=1;
  else
    ac_air_clean_state=0;
  Ac_Air_Clean(ac_air_clean_state);
  GoHome();
  delay(100);
}

void handleCoolHeat() {
  if(ac_heat==0)
    ac_heat=1;
  else
    ac_heat=0;
  if(ac_power_on==1);
    Ac_Activate(ac_temperature, ac_flow, ac_heat);
  GoHome();
  delay(100);
}

void handleAcType() {
  if(kAc_Type==0)
    kAc_Type=1;
  else
    kAc_Type=0;
  GoHome();
  delay(100);
}

void handleOnOff() {
  if(ac_power_on==1) {
    ac_power_on=0;
    handleOff();
  }
  else {
    ac_power_on=1;
    handleOn();
  }
}

void handleOn() {
  Ac_Activate(ac_temperature, ac_flow, ac_heat);
  GoHome();
  delay(100);
}

void handleOff() {
  Ac_Power_Down();
  GoHome();
  delay(100);
}

void handleWifi() {
  String s; 
  String s1= String(ssid);
  //s="<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\", meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\", meta http-equiv='refresh' content='5'/>";
  s="<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\", meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />";
  //s=s+"<meta http-equiv='refresh' content='5'/>";
  s=s+"<h1>Wifi 사양</h1>";
  if (server.client().localIP() == apIP) {
    Serial.println(String(softAP_ssid));
    s=s+String("<p>연결된 AP: ") + sAP_ssid + "</p>";
  } else {
    s=s+"<p>연결된 와이파이 " + String(ssid) + "</p>";
  }
  s=s+"<table><tr><th align='left'>SoftAP 사양</th></tr>";
  s=s+"<tr><td>SSID " + String(softAP_ssid) + "</td></tr>";
  s=s+"<tr><td>IP   " + toStringIp(WiFi.softAPIP()) + "</td></tr>"+"</table>";
  s=s+"<br /><table><tr><th align='left'>WLAN 사양</th></tr>";
  s=s+"<tr><td>SSID " + String(ssid) + "</td></tr>";
  s=s+"<tr><td>IP   " + toStringIp(WiFi.localIP()) + "</td></tr>"+"</table>";
  
  s=s+"<br /><table><tr><th align='left'>검색된 와이파이 </th></tr>";
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      s=s+"\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *") + " (" + WiFi.RSSI(i) + ")</td></tr>";
    }
  } else {
    s=s+"<tr><td>No WLAN found</td></tr>";
  }
  s=s+"</table>";
  s=s+"<p><a href='/wifi'>와이파이가 없으면 다시 검색하세요.</a></p>";
  
  s=s+"<form method='POST' action='wifisave'><h4>연결하려는 와이파이 입력</h4>"
    +"<input type='text' placeholder='와이파이 이름' name='n'/>"
    +"<br /><input type='password' placeholder='비밀번호' name='p'/>"
    +"<br /><input type='submit' value='      저    장      '/></form>"
    +"<p><a href='/'>메인 홈페이지로 가기</a>.</p>";
  server.send(200, "text/html", s);
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  Serial.println("wifi save");
  server.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  server.arg("p").toCharArray(password, sizeof(password) - 1);
  server.sendHeader("Location", "wifi", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  saveCredentials();
  connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname)+".local")) {
    Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}
