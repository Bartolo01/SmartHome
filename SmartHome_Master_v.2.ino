#include <esp_now.h>
#include <WiFi.h>

#include <esp_wifi.h>
#include <esp_pm.h>
#include <esp_wifi_types.h>

#include <WebServer.h>
WebServer server(80);

#include <time.h>
int realtime = 43200*100;
uint8_t realday = 0;




/** CHANGABLES **/
const char* ssid       = "UPC4F9E589";
const char* password   = "untdteuFdkz6";

//uint8_t master_address[] = {0xC4,0x4F,0x33,0x39,0x5A,0xFD};
uint8_t slave_address[19][6] = {
  0x94,0xA9,0x90,0x94,0xF5,0x18,   //Test_RTS0
  0x94,0xA9,0x90,0x92,0xF6,0xD0,   //Bartek_R000
  0xDC,0x1E,0xD5,0x36,0x0a,0x38,   //Salon_R?00
  //0x00,0x00,0x00,0x00,0x00,0x00,   //
  //0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
  0x00,0x00,0x00,0x00,0x00,0x00,   //
};




typedef struct struct_data {

  byte id;
  byte code;
  byte mode[4];   //MODES (0-Off : 1-ON/OFF : 2-PWM : 3-Sensor : 4-Sprinkler)

  bool state[4];
  byte duty[4];
  int power[4];
  int runtime[4];

  bool automation[4];
  int starttime[4];
  int duration[4];
  uint8_t days[4];
  
} struct_data;

struct_data incoming;    //Data received
struct_data buffer;      //Data buffer for checking origin

//SLAVE DATA
  struct_data slavedata0;   struct_data slavedata1;   struct_data slavedata2;   struct_data slavedata3;   struct_data slavedata4;
  struct_data slavedata5;   struct_data slavedata6;   struct_data slavedata7;   struct_data slavedata8;   struct_data slavedata9;
  struct_data slavedata10;  struct_data slavedata11;  struct_data slavedata12;  struct_data slavedata13;  struct_data slavedata14;
  struct_data slavedata15;  struct_data slavedata16;  struct_data slavedata17;  struct_data slavedata18;  struct_data slavedata19;
  struct_data slavedata[20] = {slavedata0,  slavedata1,  slavedata2,  slavedata3,  slavedata4,  slavedata5,  slavedata6,  slavedata7,  slavedata8,  slavedata9,
                               slavedata10, slavedata11, slavedata12, slavedata13, slavedata14, slavedata15, slavedata16, slavedata17, slavedata18, slavedata19};
//


//SLAVE INFORMATION
  esp_now_peer_info_t slaveinfo0;   esp_now_peer_info_t slaveinfo1;   esp_now_peer_info_t slaveinfo2;   esp_now_peer_info_t slaveinfo3;   esp_now_peer_info_t slaveinfo4;   
  esp_now_peer_info_t slaveinfo5;   esp_now_peer_info_t slaveinfo6;   esp_now_peer_info_t slaveinfo7;   esp_now_peer_info_t slaveinfo8;   esp_now_peer_info_t slaveinfo9;   
  esp_now_peer_info_t slaveinfo10;  esp_now_peer_info_t slaveinfo11;  esp_now_peer_info_t slaveinfo12;  esp_now_peer_info_t slaveinfo13;  esp_now_peer_info_t slaveinfo14;  
  esp_now_peer_info_t slaveinfo15;  esp_now_peer_info_t slaveinfo16;  esp_now_peer_info_t slaveinfo17;  esp_now_peer_info_t slaveinfo18;  esp_now_peer_info_t slaveinfo19;
  esp_now_peer_info_t slaveinfo[20] = {slaveinfo0,  slaveinfo1,  slaveinfo2,  slaveinfo3,  slaveinfo4,  slaveinfo5,  slaveinfo6,  slaveinfo7,  slaveinfo8,  slaveinfo9,
                                   slaveinfo10, slaveinfo11, slaveinfo12, slaveinfo13, slaveinfo14, slaveinfo15, slaveinfo16, slaveinfo17, slaveinfo18, slaveinfo19};
//




/** CODE OF THE WEBSITE **/
const char html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>
	<head>
		<meta charset="UTF-8" />
		<meta name="viewport" content="width=device-width, initial-scale=1" />
		<title> BawerHome </title>
		<style>
			body {
				font-family: Arial, sans-serif;
				background: #f0f2f5;
				margin: 0;
				padding: 20px;
				text-align: center;
			}

			h1 {
				font-size: 24px;
				margin-bottom: 20px;
			}

			.card {
				background-color: white;
				border-radius: 12px;
				box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
				padding: 20px;
				margin-bottom: 20px;
				max-width: 400px;
				margin-left: auto;
				margin-right: auto;
				text-align: left;
			}

			.title {
				font-size: 18px;
				font-weight: bold;
				margin-bottom: 6px;
			}

			.details {
				font-size: 14px;
				color: #666;
			}

			.switch-wrapper {
				display: flex;
				justify-content: space-between;
				align-items: center;
			}

			.toggle {
				position: relative;
				width: 50px;
				height: 26px;
			}

			.toggle input {
				opacity: 0;
				width: 0;
				height: 0;
			}

			.slider-toggle {
				position: absolute;
				cursor: pointer;
				top: 0;
				left: 0;
				right: 0;
				bottom: 0;
				background-color: grey;
				transition: 0.4s;
				border-radius: 34px;
			}

			.slider-toggle:before {
				position: absolute;
				content: "";
				height: 20px;
				width: 20px;
				left: 3px;
				bottom: 3px;
				background-color: white;
				transition: 0.4s;
				border-radius: 50%;
			}

			input:checked + .slider-toggle {
				background-color: #007BFF;
			}

			input:checked + .slider-toggle:before {
				transform: translateX(24px);
			}

			.slider-input {
				width: 100%;
				margin-top: 12px;
				appearance: none;
				height: 6px;
				background: #ddd;
				border-radius: 5px;
				outline: none;
				transition: background 0.3s ease;
			}

			.slider-input::-webkit-slider-thumb {
				appearance: none;
				width: 18px;
				height: 18px;
				background: #007BFF;
				border-radius: 50%;
				cursor: pointer;
				box-shadow: 0 0 2px rgba(0, 0, 0, 0.5);
			}

			.slider-input::-moz-range-thumb {
				width: 18px;
				height: 18px;
				background: #007BFF;
				border-radius: 50%;
				cursor: pointer;
			}

			.slider-input:disabled {
				opacity: 0.5;
				cursor: not-allowed;
			}
		</style>
	</head>
	<body>


		<div id="espTime" style="font-size: 20px; margin-bottom: 20px;"> --:--:-- </div>
		<h1>BawerHome🏠</h1>



    <!-- Toggle Card -->
    <div class="card">
			<div class="switch-wrapper" style="align-items: flex-start;">
				<div style="flex: 1;">
					<div class="title">💡 Bartek</div>
					<div class="details" style="display: flex; justify-content: flex-start; align-items: center;">
						<div style="flex: 1;">
							Pobór prądu: <span id="current1">--</span>A<br>
							Działa przez: <span id="runtime1">--:--</span>
						</div>
						<button id="arrowBtn1" class="arrow-btn" style="background:none; border:none; font-size: 20px; cursor:pointer;" onclick="toggleAutomationMenu(1)"> V </button>
					</div>
				</div>
				<label class="toggle" style="margin-left: 12px; align-self: flex-start;">
					<input type="checkbox" id="switch1" onclick="toggleSwitch(1)">
					<span class="slider-toggle"></span>
				</label>
			</div>

			<!-- Automation toggle and settings -->
			<div id="automation1" class="automation-menu" style="display: none; margin-top: 10px;">
				<div style="display: flex; justify-content: flex-end; margin-bottom: 10px;">
					<label class="toggle">
						<input type="checkbox" id="autoSwitch1" onclick="toggleAutomation(1)">
						<span class="slider-toggle"></span>
					</label>
				</div>
				Po<input type="checkbox" name="day1" value="pon">
				Wt<input type="checkbox" name="day1" value="wt">
				Śr<input type="checkbox" name="day1" value="sr">
        Cz<input type="checkbox" name="day1" value="czw">
        <label style="margin-left: 20px;"> Start:</label> <input type="time" id="startTime1" />
        <br />
				Pt<input type="checkbox" name="day1" value="pt">
				Sb<input type="checkbox" name="day1" value="sob">
				Nd<input type="checkbox" name="day1" value="nd">
        <label style="margin-left: 19px;"> Czas pracy:</label> <input type="number" id="duration1" min="1" max="999" />

			</div>
		</div>




		<!-- Slider Card -->
		<div class="card">
			<div class="switch-wrapper">
				<div>
					<div class="title">💡 Salon</div>
					<div class="details">
						Pobór prądu: <span id="current2">--</span>A<br>
						Działa przez: <span id="runtime2">--:--</span>
					</div>
				</div>
				<label class="toggle">
					<input type="checkbox" id="switch2" onclick="toggleSwitch(2)">
					<span class="slider-toggle"></span>
				</label>
			</div>
			<input
				type="range"
				class="slider-input"
				min="0"
				max="255"
				value="0"
				id="slider2"
				data-device="2"
				disabled
			/>
		</div>



    <!-- Sensor Display Card -->
    <!--
    <div class="card">
	    <div class="title">
      🌡️ Temperatura gdzieś: : 
      <span id="current3" style="font-weight: normal; color: #333; margin-left: 6px;">--</span>°C
      </div>
    </div>
    -->


		<script>



      function toggleSwitch(id) {
				fetch(`/status${id}`, {
					method: 'POST',
					headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
					body: 'toggle=1'
				})
				.then(response => response.json())
				.then(data => {
					const isOn = (data.state === "ON");
					const switchElem = document.getElementById("switch" + id);
					switchElem.checked = isOn;

					const slider = document.getElementById("slider" + id);
					if (slider) {
						slider.disabled = !isOn;
					}
				});
			}


      function toggleAutomationMenu(id) {
        const panel = document.getElementById("automation" + id);
        panel.style.display = (panel.style.display === "none") ? "block" : "none";
      }


      function toggleAutomation(id) {
        const isEnabled = document.getElementById("autoSwitch" + id).checked;
        const settings = document.getElementById("automation" + id);

        Array.from(settings.querySelectorAll("input[type='checkbox'], input[type='time'], input[type='number']")).forEach(el => {
          if (el.id !== 'autoSwitch' + id) el.disabled = isEnabled;
        });


        //SENDING UPDATE TO ESP
        const auto     = document.getElementById(`autoSwitch${id}`).checked ? 1 : 0;
        const start    = document.getElementById(`startTime${id}`).value;      // “hh:mm”
        const duration = document.getElementById(`duration${id}`).value || 0;  // minutes
        const days     = Array
                    .from(document.querySelectorAll(`#automation${id} input[name='day${id}']:checked`))
                    .map(el => el.value)
                    .join(',');                                           // “pon,wt,sr”

        const body = `auto=${auto}&start=${start}&duration=${duration}&days=${encodeURIComponent(days)}`;

        fetch(`/automation${id}`, {                         // <— hits the handler in your sketch
          method : 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body   : body
        });

      }



			document.querySelectorAll('.slider-input').forEach(slider => {
				slider.addEventListener('input', function () {
					const value = this.value;
					const deviceId = this.dataset.device;

					fetch(`/slider${deviceId}?duty=${value}`);
				});
			});



      const deviceCount = 1;
      document.addEventListener("DOMContentLoaded", () => {
        for (let i = 1; i <= deviceCount; i++) {
          attachAutomationListeners(i);
        }
      });


      function attachAutomationListeners(id) {
        const container = document.getElementById("automation" + id);
        if (!container) return;

        // Time and number inputs
        container.querySelectorAll("input[type='time'], input[type='number']").forEach(el => {
          el.addEventListener("change", () => toggleAutomation(id));
        });

        // Day checkboxes
        container.querySelectorAll(`input[name='day${id}']`).forEach(chk => {
          chk.addEventListener("change", () => toggleAutomation(id));
        });

        // Automation switch
        const autoSwitch = document.getElementById("autoSwitch" + id);
        if (autoSwitch && !autoSwitch.hasListener) {
          autoSwitch.addEventListener("change", () => toggleAutomation(id));
          autoSwitch.hasListener = true;
        }
      }




			function updateStatus(id) {
				fetch("/status" + id)
				.then(response => response.json())
				.then(data => {
					const isOn = (data.state === "ON");

					if(id % 2 == 1){                                          //Toggle tiles
						document.getElementById("switch" + id).checked = isOn;
					}
					else if(id % 2 == 0){                                     //Slider tiles
						const slider = document.getElementById("slider" + id);
						slider.value = data.duty || 0;
						slider.disabled = !isOn;
						document.getElementById("switch" + id).checked = isOn;
					}

          //CURRENT
					document.getElementById("current" + id).textContent = (data.current / 1000).toFixed(2);

          //RUNTIME
					const runtimeSeconds = data.runtime || 0;
					const hours = Math.floor(runtimeSeconds / 3600);
					const mins = Math.floor((runtimeSeconds % 3600) / 60);
					document.getElementById("runtime" + id).textContent = `${hours.toString().padStart(2, '0')}:${mins.toString().padStart(2, '0')}`;

          //AUTOMATION
		      const autoSwitch = document.getElementById("autoSwitch" + id);
		      if (autoSwitch) {
			      autoSwitch.checked = data.auto === 1;

			      const settings = document.getElementById("automation" + id);
			      Array.from(settings.querySelectorAll("input[type='checkbox'], input[type='time'], input[type='number']")).forEach(el => {
			      	if (el.id !== 'autoSwitch' + id) el.disabled = data.auto === 1;
			      });

			      //STARTTIME
			      const startSec = data.start || 0;
			      const startHours = Math.floor(startSec / 3600);
			      const startMins = Math.floor((startSec % 3600) / 60);
			      const startStr = `${startHours.toString().padStart(2, '0')}:${startMins.toString().padStart(2, '0')}`;
			      const startTime = document.getElementById("startTime" + id);
			      if (startTime) startTime.value = startStr;

			      //DURATION
			      const duration = document.getElementById("duration" + id);
			      if (duration) duration.value = Math.round((data.duration || 0) / 60);

			      //DAYS
			      const daysBinary = data.days || 0;
			      const dayMap = ['pon', 'wt', 'sr', 'czw', 'pt', 'sob', 'nd'];
			      document.querySelectorAll(`#automation${id} input[name='day${id}']`).forEach(chk => {
				      const index = dayMap.indexOf(chk.value);
				      if (index >= 0) {
					      chk.checked = ((daysBinary >> index) & 1) === 1;
				      }
			      });
          } 

				});
			}



			function updateTime(){
				fetch("/updateTime")
				.then((response) => response.json())
				.then((data) => {
					const hour = data.hour.toString().padStart(2, '0');
					const minute = data.minute.toString().padStart(2, '0');
					const second = data.second.toString().padStart(2, '0');
					document.getElementById("espTime").textContent = `Time: ${hour}:${minute}:${second}`;
				});
			}



			setInterval(() => {
        for(let t=1; t<=2; t++){ updateStatus(t); }
				updateTime();
			}, 500);


		</script>


	</body>
</html>

)rawliteral";




void setup() {

  Serial.begin(115200);
  while(!Serial){}


/** SETTING UP WIFI **/
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE);

  Serial.println();   Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500);  Serial.print("."); }
  Serial.print("  Connected. IP address: "); Serial.print(WiFi.localIP());
//



/** SETTING UP TIME **/
  configTime(3600, 3600, "pool.ntp.org");
  delay(200);
  updateTime();
  Serial.println();   Serial.print("Current Day: "); Serial.print(realday); Serial.print("  Current Time: "); Serial.print(realtime/100);  
//



/** SETTING UP ESP-NOW **/
  if (esp_now_init() != ESP_OK) {
    Serial.println();  Serial.print("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  

  //REGISTERRING ALL THE PEERS
  for(byte slave=0; slave<20; slave++){
    memcpy(slaveinfo[slave].peer_addr, slave_address[slave], 6);

    slaveinfo[slave].channel = 1;
    slaveinfo[slave].encrypt = false;

    slavedata[slave].id = slave;


    if(esp_now_add_peer(&slaveinfo[slave])!=ESP_OK){
      Serial.println();  Serial.print("Failed to add: Slave"); Serial.print(slave);
    }
    else{
      if(slave_address[slave][0]==0){
        break;
      }

      Serial.println();  Serial.print("Added: Slave"); Serial.print(slave);
    }

  }
//


/** SERVER HANDLING **/
  server.begin();

  server.on("/", handleRoot);
  server.on("/updateTime", sendTime);

  server.on("/status1", []() { status(1, 0); });
  server.on("/automation1", []() { automation(1, 0); });

  server.on("/status2", []() { status(2, 0); });
  server.on("/slider2", []() { slider(2, 0); });

  //server.on("/status3", []() { status(0, 2); });

//

}





byte slave;
byte channel;
unsigned long lastupdate = millis();

void loop() {

  server.handleClient();


  /** KEEPING TRACK OF TIME AND GETTING UPDATES**/
  if(millis()-lastupdate>=20){
    lastupdate = millis();

    //Manually Tracking Time
    realtime = realtime+2;
    if(realtime == 8640000){
      realtime=0;
      realday=realday+1;
      if(realday==7){ realday=0; }
    }

    //Updating time every hour
    if(realtime%360000==0){
      Serial.println(); Serial.print("Current Day: "); Serial.print(realday); Serial.print("  Current Time: "); Serial.print(realtime/100);
      updateTime();
    }
    
    

    /** UPDATING ONE SLAVE DATA EVERY 4sec **/
    while(realtime%30==0){
      
      slave = (realtime%600)/30;
        
      if(slave_address[slave][0]==0 || slave>=19){
        break;
      }

      if(slave!=0){
        Serial.println();   Serial.print("Update need from: S");  if(slave<10){Serial.print("0");}  Serial.print(slave);

        slavedata[slave].code = 100;  //ID for getting statistics
        esp_now_send(slave_address[slave], (uint8_t *)&slavedata[slave], sizeof(slavedata[slave]));
      }

      break;
    }


    /** AUTOMATION CHECKING **/
    if(realtime%6000==400){
      Serial.println();   Serial.print("Automation CHECK");
      //Serial.print(realtime/100); Serial.print(" <> "); Serial.println(slavedata[1].starttime[0]+3);

      for(byte slave=0; slave<19; slave++){
        if(slave_address[slave][0]==0){ break; }
        for(channel=0; channel<4; channel++){
          
          if(slavedata[slave].automation[channel]==1){
            if(slavedata[slave].days[channel] & (1 << realday)){

              if(realtime/100 == slavedata[slave].starttime[channel]+4){
                Serial.println();  Serial.print("Automation call: S");  if(slave<10){Serial.print("0");}  Serial.print(slave); Serial.print("  Channel:"); Serial.print(channel);  Serial.print("   ON!!!");
                status(slave, channel+100);
              }
              else if(realtime/100-slavedata[slave].duration[channel] == slavedata[slave].starttime[channel]+4){
                Serial.println();  Serial.print("Automation call: S");  if(slave<10){Serial.print("0");}  Serial.print(slave); Serial.print("  Channel:"); Serial.print(channel);  Serial.print("   OFF!!!");
                status(slave, channel+200);
              }

            }
          }

        }
      }

    }
    

  }

}



byte retrycounter=0;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status){

  Serial.print(status == 0 ? "   Succeeded   " : "   Failed   ");

  if(status!=0){

    //Checking which slave was the data for
    slave=0;
    for(slave; slave<19; slave++){
      if(memcmp(mac_addr, slave_address[slave], 6)==0){
        break;
      }
    }

    if(slavedata[slave].code==104 || slavedata[slave].code==105 || slavedata[slave].code==101){
      retrycounter++;
      
      if(retrycounter<4){
        Serial.println(); Serial.print("Retrying to send. Attempt: "); Serial.print(retrycounter);
        esp_now_send(slave_address[slave], (uint8_t *)&slavedata[slave], sizeof(slavedata[slave]));
      }
      else{
        retrycounter=0;
      }
    }
  }
  else{
    retrycounter=0;
  }
  

}



void OnDataRecv(uint8_t* mac, uint8_t* incoming, int len){

  memcpy(&buffer, incoming, sizeof(buffer));
  Serial.println();   Serial.print("Data received from: S");  if(buffer.id<10){Serial.print("0");}  Serial.print(buffer.id);  Serial.print("   Code: "); Serial.print(buffer.code);


  if(buffer.code == 100){   //Getting statistics
    //memcpy(&slavedata[buffer.id], incoming, sizeof(slavedata[buffer.id]));
    for(byte channel=0; channel<4; channel++){
      slavedata[buffer.id].state[channel]=buffer.state[channel];
      slavedata[buffer.id].duty[channel]=buffer.duty[channel];
      slavedata[buffer.id].power[channel]=buffer.power[channel];
      slavedata[buffer.id].runtime[channel]=buffer.runtime[channel];
  
    }
    
  }

}





/** CALLBACK WHEN NEED FOR TIME **/
void updateTime(){

  if(WiFi.status() == WL_CONNECTED){
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      return;
    }
    realtime = timeinfo.tm_hour*3600 + timeinfo.tm_min*60 + timeinfo.tm_sec;
    realtime=realtime*100;

    realday = timeinfo.tm_wday;
    realday = (realday == 0) ? 6 : realday-1;
  }

}




/** HANDLING SERVER **/
void handleRoot() {
  server.send_P(200, "text/html", html);
}


void sendTime() {

  String json = "{\"hour\":" + String((realtime/360000) % 24) + 
                ",\"minute\":" + String((realtime/6000) % 60) + 
                ",\"second\":" + String((realtime/100) % 60) + "}";
  server.send(200, "application/json", json);

  //Serial.print("Sending time"); Serial.println(json);

}



void status(byte slave, byte channel){

  byte flag = channel/100;
  channel = channel-flag*100;

  if(server.hasArg("toggle") || flag>0){

    if(flag==0){
      slavedata[slave].code = 101;
      slavedata[slave].state[channel] = !slavedata[slave].state[channel];
    }
    else if(flag==1){
      slavedata[slave].code = 104;
      slavedata[slave].state[channel] = 1;
    }
    else if(flag==2){
      slavedata[slave].code = 105;
      slavedata[slave].state[channel] = 0;
    }
    
    Serial.println();   Serial.print("Toggle update for: S");  if(slave<10){Serial.print("0");}  Serial.print(slave);

  }


  String json = "{\"state\":\"" + String((slavedata[slave].state[channel] > 0) ? "ON" : "OFF") + 
                "\",\"current\":" + String(slavedata[slave].power[channel]) +
                ",\"runtime\":" + String(slavedata[slave].runtime[channel]) +
                ",\"duty\":" + String(slavedata[slave].duty[channel]) +
                ",\"auto\":" + String(slavedata[slave].automation[channel]) +
                ",\"start\":" + String(slavedata[slave].starttime[channel]) +
                ",\"duration\":" + String(slavedata[slave].duration[channel]) +
                ",\"days\":" + String(slavedata[slave].days[channel]) + 
                "}";
                

  server.send(200, "application/json", json);
  //Serial.print("Status " + String(slavedata[s].id) + String(channel) + ":"); Serial.println(json);

  if(server.hasArg("toggle")==1 || flag>0){
    
    slave=slave;
    esp_now_send(slave_address[slave], (uint8_t *)&slavedata[slave], sizeof(slavedata[slave]));
  }

}



void slider(byte slave, byte channel){

  if(server.hasArg("duty")){

    slavedata[slave].duty[channel] = server.arg("duty").toInt();
    //Serial.println();   Serial.printf("Slider update for ", slavedata[s].id, ": duty=%d\n", slavedata[s].duty[channel]);

    server.send(200, "text/plain", "OK");

    slavedata[slave].code = 102;
    esp_now_send(slave_address[slave], (uint8_t *)&slavedata[slave], sizeof(slavedata[slave]));

  }

  
}



void automation(byte slave, byte channel) {

  slavedata[slave].automation[channel] = server.arg("auto") == "1";
  slavedata[slave].duration[channel] = server.arg("duration").toInt() * 60;
  slavedata[slave].days[channel] = csvToMask(server.arg("days"));

  int h; int m;
  if(sscanf(server.arg("start").c_str(), "%d:%d", &h, &m) == 2){ slavedata[slave].starttime[channel] = h * 3600 + m * 60; } 
  else{ slavedata[slave].starttime[channel] = 0; }

  Serial.println();   Serial.print("Automation info for: S");  if(slave<10){Serial.print("0");}  Serial.print(slave);
  Serial.print("   " + String(slavedata[slave].automation[channel]) + ", " + String(slavedata[slave].duration[channel]) + ", " + String(slavedata[slave].days[channel]) + ", " + String(slavedata[slave].starttime[channel]));
  server.send(200, "text/plain", "OK");

  slavedata[slave].code = 103;
  esp_now_send(slave_address[slave], (uint8_t *)&slavedata[slave], sizeof(slavedata[slave]));

}

uint8_t csvToMask(const String& csv){
  uint8_t m = 0;
  if (csv.indexOf("pon") >= 0) m |= 1 << 0;   //monday
  if (csv.indexOf("wt")  >= 0) m |= 1 << 1;   //tuesday
  if (csv.indexOf("sr")  >= 0) m |= 1 << 2;   //wednesday
  if (csv.indexOf("czw") >= 0) m |= 1 << 3;   //thursday
  if (csv.indexOf("pt")  >= 0) m |= 1 << 4;   //friday
  if (csv.indexOf("sob") >= 0) m |= 1 << 5;   //saturday
  if (csv.indexOf("nd")  >= 0) m |= 1 << 6;   //sunday
  return m;
}




