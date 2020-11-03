// html->string 변환사이트
// http://davidjwatts.com/youtube/esp8266/esp-convertHTM.html#
// https://tomeko.net/online_tools/cpp_text_escape.php?lang=en
String webHead="<html> <head> <!-- <meta http-equiv='refresh' content='2'/>--> <meta name='viewport' content='width=device-width, initial-scale=1.0'/> <meta http-equiv='Content-Type' content='text/html;charset=utf-8' /> <style> body { background: #eab0dc; font-family: \\\"Lato\\\", sans-serif; } .button { border: none; color: white; padding: 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; } .button-on {border-radius: 100%; background-color: #4CAF50;} .button-off {border-radius: 100%;background-color: #707070;} .button-ledon {border-radius: 100%; padding: 10px; font-size: 8px; margin: 0px 0px; background-color: #ff4500;} .button-ledoff {border-radius: 100%; padding: 10px; font-size: 8px; background-color: #707070;} .slidecontainer { width: 100%; } .slider { -webkit-appearance: none; width: 80%; height: 15px; border-radius: 5px; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; } .slider:hover { opacity: 1; } .slider::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; border-radius: 50%; background: #4CAF50; cursor: pointer; } .slider::-moz-range-thumb { width: 25px; height: 25px; border-radius: 50%; background: #4CAF50; cursor: pointer; } </style> </head> <body>";
String webTail="\n<p><a href='/wifi'>네트웍공유기변경-누른후 와이파설정부터 다시하세요</a></p></body>\n</html>";

void handleRoot() {
  String s=""; 
  s+="<form action='/on'>";
  if(onSw==1)
    s+="<a href='on'> <button type='submit' name='act' value='1' class='button button-on' >on</button></a>";
  else
    s+="<a href='on'> <button type='submit' name='act' value='1' class='button button-off' >off</button></a>";
  s+="</form>";
  
  s+="<br>속도조절<br><br>";
  s+="<form action='/on' id='nameform'>";
  s+="<input type='hidden' name='act' value='2'>";
  s+="<input type='range' name='variable' min='100' max='10000' class='slider' value='"+(String)delayMsec+"' onclick='submit();'>"+(String)delayMsec+"</form>";
  s+="</form>";
  s+="&emsp;<button type='submit' form='nameform' value='Submit'>보내기</button>";
  s+="<br><br>AP & IP :&emsp;"+sChipId+"&emsp;"+WiFi.localIP().toString();
  server.send(200, "text/html", webHead+s+webTail);
}

void handleWifi() {
  WiFiManager wm;
  wm.resetSettings();
  wm.resetSettings();
  ESP.reset();
}

void handleOn() {
  int act=server.arg("act").toInt();
  Serial.println(act);
  if(act==1) {
    if(onSw==0) {
      onSw=1;
      digitalWrite(EN, 1);
    }
    else {
      onSw=0;
      digitalWrite(EN, 0);
    }
  }
  if(act==2) {
    delayMsec=server.arg("variable").toInt();
    Serial.println(delayMsec);
  }
  GoHome();
}

void GoHome() {
  String s,ipS;
  //IPAddress ip;
  ipS=toStringIp(WiFi.localIP());
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
  s="{\"chipId\":\""+sChipId+"\",\"ip\":\""+WiFi.localIP().toString()+"\"}";
  server.send(200, "text/html", s);
}
