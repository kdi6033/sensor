BLDC 모터
======================
![001](https://user-images.githubusercontent.com/37902752/134831269-0cdb8d24-544b-4778-9f2d-be28e6a3677a.png)

## 1.1 사용제품
![image](https://user-images.githubusercontent.com/37902752/134830443-6e32b122-aa9e-4bbb-8690-4a585405e173.png)   
E-TECH에서 제작한 BLDC 모터 24V 250W x 2

![image](https://user-images.githubusercontent.com/37902752/134830450-aa7533a0-4c3b-47c7-a61e-e39f475ee09d.png)   
Keya Electron에서 제작한 bldc 모터 컨트롤러 KYDBL4830-2E


## 1.2 수동제어
![001](https://user-images.githubusercontent.com/37902752/134831373-808332f4-8413-4f2d-9356-c23a89431118.png)

### 수동제어 배선도
![컨트롤러](https://user-images.githubusercontent.com/37902752/134830890-33462f8b-1d66-4cc3-905d-0c5f54bec8fa.jpg)
![모터](https://user-images.githubusercontent.com/37902752/134830875-d01f7ba6-35f5-47fc-9566-f7f06398532b.jpg)
![보드](https://user-images.githubusercontent.com/37902752/134830857-3f9549b1-d85f-4637-9ee5-43fde834a21c.jpg)
## 1.3 RS232제어
![022](https://user-images.githubusercontent.com/37902752/134831429-5cf94fee-6669-4d21-b9a0-56896782ddda.png)

![image](https://user-images.githubusercontent.com/37902752/134831669-dbf2d038-4408-4900-9e6b-1e306d234b91.png)   
컨피그로 들어가 줍니다.   
![image](https://user-images.githubusercontent.com/37902752/134831740-9161e3c6-b03d-4857-8182-33004521f67d.png)   
디지털 인풋에 3번과 10번을 low로 바꿔줍니다.
### KEYA 프로그램 설명
![223](https://user-images.githubusercontent.com/37902752/134830921-093b6612-a310-40c0-89e5-438df8b847d9.PNG)
### RS232 제어 배선도
![RS 223](https://user-images.githubusercontent.com/37902752/134830378-71aec27b-a991-408c-a01a-20f82deb8618.png)

## 1.4 Node-Red 제어
![030](https://user-images.githubusercontent.com/37902752/134831451-eb674c91-c003-45f5-a38c-2a507f9c4956.png)
### Node-Red 배선도
![node_red](https://user-images.githubusercontent.com/37902752/134830768-dfd20459-1507-41e2-8ac7-74f9393bfd40.png)
### Node-Red 소스 코드

```
[{"id":"1c7ca8a0.3613e7","type":"tab","label":"플로우 7","disabled":false,"info":""},{"id":"87a531bc.b676a","type":"serial out","z":"1c7ca8a0.3613e7","name":"","serial":"adc9d8bd.2b3c18","x":800,"y":180,"wires":[]},{"id":"d01ee55c.ea19e8","type":"looptimer-advanced","z":"1c7ca8a0.3613e7","duration":"1","units":"Second","maxloops":"3600","maxtimeout":"1","maxtimeoutunits":"Hour","name":"반복","x":670,"y":340,"wires":[["87a531bc.b676a"],[]]},{"id":"8cc38345.f9b15","type":"function","z":"1c7ca8a0.3613e7","name":"","func":"msg.payload =\"!M  \" +100 + \" \"+msg.payload;\nreturn msg;","outputs":1,"noerr":0,"initialize":"","finalize":"","x":460,"y":360,"wires":[["d01ee55c.ea19e8"]]},{"id":"859e417d.4b62e","type":"ui_slider","z":"1c7ca8a0.3613e7","name":"","label":"좌","tooltip":"","group":"199b0928.389a87","order":10,"width":0,"height":0,"passthru":true,"outs":"all","topic":"topic","topicType":"str","min":"0","max":"500","step":"100","x":270,"y":360,"wires":[["8cc38345.f9b15"]]},{"id":"b8f7f022.1d82d","type":"ui_slider","z":"1c7ca8a0.3613e7","name":"","label":"우","tooltip":"","group":"199b0928.389a87","order":10,"width":0,"height":0,"passthru":true,"outs":"all","topic":"topic","topicType":"str","min":"0","max":"500","step":"100","x":270,"y":420,"wires":[["6edd12c.e83dfec"]]},{"id":"6edd12c.e83dfec","type":"function","z":"1c7ca8a0.3613e7","name":"","func":"msg.payload =\"!M  \" +msg.payload + \" \"+100;\nreturn msg;","outputs":1,"noerr":0,"initialize":"","finalize":"","x":460,"y":420,"wires":[["d01ee55c.ea19e8"]]},{"id":"293cfdf4.a4e092","type":"ui_slider","z":"1c7ca8a0.3613e7","name":"","label":"전진","tooltip":"","group":"199b0928.389a87","order":10,"width":0,"height":0,"passthru":true,"outs":"all","topic":"topic","topicType":"str","min":"-600","max":"600","step":"100","x":270,"y":260,"wires":[["b40b783e.de06a8","94da69e4.2e38e8"]]},{"id":"b40b783e.de06a8","type":"function","z":"1c7ca8a0.3613e7","name":"","func":"if (msg.value === 1 ){\n    msg.payload =\"!M  \" +msg.payload + \" \"+msg.payload;\n}\nelse { msg.payload =\"!M -\" +msg.payload + \" -\"+msg.payload;\n}\nreturn msg;","outputs":1,"noerr":0,"initialize":"","finalize":"","x":460,"y":260,"wires":[["d01ee55c.ea19e8"]]},{"id":"c5b2e8fc.5e0868","type":"ui_template","z":"1c7ca8a0.3613e7","group":"199b0928.389a87","name":"긴급 정지","order":1,"width":"3","height":"3","format":"\n<md-button class=\"vibrate filled touched bigfont rounded\" style=\"background-color:#c0392b\" ng-click=\"send({payload: '!EX'})\"> \n    긴급<br/>정지\n</md-button> \n\n","storeOutMessages":true,"fwdInMessages":true,"resendOnRefresh":false,"templateScope":"local","x":280,"y":120,"wires":[["87a531bc.b676a"]]},{"id":"e905fac8.b8c898","type":"ui_template","z":"1c7ca8a0.3613e7","group":"199b0928.389a87","name":"해제","order":2,"width":"3","height":"3","format":"\n<md-button class=\"filled touched bigfont rounded vibrate\" style=\"background-color:#27ae60\" ng-click=\"send({payload: '!MG'})\"> \n    해제<br/>\n</md-button> \n\n","storeOutMessages":true,"fwdInMessages":true,"resendOnRefresh":false,"templateScope":"local","x":272.0000305175781,"y":184,"wires":[["87a531bc.b676a"]]},{"id":"94da69e4.2e38e8","type":"ui_gauge","z":"1c7ca8a0.3613e7","name":"","group":"199b0928.389a87","order":15,"width":0,"height":0,"gtype":"gage","title":"gauge","label":"units","format":"{{value}}","min":"-600","max":"600","colors":["#eeff00","#04e600","#ffff00"],"seg1":"-300","seg2":"300","x":450,"y":300,"wires":[]},{"id":"adc9d8bd.2b3c18","type":"serial-port","serialport":"COM15","serialbaud":"115200","databits":"8","parity":"none","stopbits":"1","waitfor":"","dtr":"none","rts":"none","cts":"none","dsr":"none","newline":"","bin":"false","out":"char","addchar":"\\r","responsetimeout":"10000"},{"id":"199b0928.389a87","type":"ui_group","name":"키우미","tab":"5eec05a4.959bac","order":1,"disp":true,"width":"6","collapse":false},{"id":"5eec05a4.959bac","type":"ui_tab","name":"키우미","icon":"dashboard","order":3,"disabled":false,"hidden":false}]
```
