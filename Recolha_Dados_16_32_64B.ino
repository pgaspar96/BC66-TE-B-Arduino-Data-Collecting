#include <Arduino.h>
#include <nbClient.h>
#include <PubSubClient.h>
#include <string.h>

//---------------------------------------------CALL BACKS---------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
  Serial.printf("[RECEIVE] (%s) %.*s\n", topic, length, payload);
}

int cmd_callback(char *line, unsigned int length, void *user)
{
  char *str;
  str = trimwhitespace(line);
  Serial.printf("[RESPONSE] %s\n", str);
}
int cmd_extended(char *line, unsigned int length, void *user)
{
  char *str;
  str = trimwhitespace(line);
  //memmove(str, str+7, strlen(str));
  Serial.printf("%s\n", str);
}

int cmd_extended_2(char *line, unsigned int length, void *user)
{
  char *str;
  str = trimwhitespace(line);
  //memmove(str, str+7, strlen(str));
  Serial.printf("%s\n", str);
}

int cmd_extended_3(char *line, unsigned int length, void *user)
{
  char *str;
  str = trimwhitespace(line);
  //memmove(str, str+7, strlen(str));
  Serial.printf("%s\n", str);
}

int cmd_extended_4(char *line, unsigned int length, void *user)
{
  char *str;
  str = trimwhitespace(line);
  //memmove(str, str+7, strlen(str));
  Serial.printf("%s\n", str);
}

int cmd_callback_connection(char *line, unsigned int length, void *user)
{
  char *str;
  str = trimwhitespace(line);
  //memmove(str, str+7, strlen(str));
  if(strcmp(str,"+CSCON: 0,1"))
  {
    mqtt_setup();
    }
  Serial.printf("%s\n", str);
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//

//------------------------------------------------MQTT------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
nbClient tcp;
PubSubClient client("broker.emqx.io", 1883, mqtt_callback, tcp);

void mqtt_setup()
{
  Serial.println("Connecting to MQTT server...");
  int t = millis();
  if (client.connect("arduinoClient", "", ""))
  {
    Serial.println("Connected");
    Serial.printf("Time Elapsed: %d ms\n", millis() - t);
  }else {
      Serial.print("\n\rFailed to connenct with state");
      Serial.print(client.state());
      delay(2000);
  }
}
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//

//----------------------------------------------Commands----------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
void engineering_mode()
{
  char cmd_engineering_mode_1[] = "AT+QENG=0";
  Ql_RIL_SendATCmd(cmd_engineering_mode_1, Ql_strlen(cmd_engineering_mode_1), cmd_extended, NULL, 2000);
  delay(75);
  char cmd_engineering_mode_2[] = "AT+QENG=1";
  Ql_RIL_SendATCmd(cmd_engineering_mode_2, Ql_strlen(cmd_engineering_mode_2), cmd_extended_2, NULL, 2000);
  delay(75);
}

void get_extended_signal_quality()
{
  char cmd[] = "AT+CESQ";
  Ql_RIL_SendATCmd(cmd, Ql_strlen(cmd), cmd_extended_3, NULL, 2000);
  delay(75);
  char cmd_csq[] = "AT+CSQ";
  Ql_RIL_SendATCmd(cmd_csq, Ql_strlen(cmd_csq), cmd_extended_4, NULL, 2000);
  delay(75);
}
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//

//-----------------------------------------------SETUP------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
void setup()
{
  int t = millis();
  String imei, mcc_mnc, sim_imsi, sim_iccid, uid;
  Dev.noSleep(); // disable PSM sleep

  Serial.begin(115200);
  Serial.debug(); // enable debug for this port, DBG( work as printf )

  Serial.printf("Arduino      %s\n", Dev.getVersion());
  Dev.getImei(imei);
  Serial.printf("IMEI         %s\n", imei.c_str());
  Dev.getUid(uid);
  Serial.printf("UID          %s\n", uid.c_str());

  Dev.waitSimReady();

  Dev.getMccMnc(mcc_mnc);
  Serial.printf("MCCMNC       %s\n", mcc_mnc.c_str());
  Dev.getImsi(sim_imsi);
  Serial.printf("IMSI         %s\n", sim_imsi.c_str());
  Dev.getIccid(sim_iccid);
  Serial.printf("ICCID        %s\n", sim_iccid.c_str());

  Dev.waitCereg();  
  delay(200); // must have some delay, afrer this time next values is available

  Serial.printf("Rx level     %d dbm\n", Dev.getReceiveLevel());
  Serial.printf("Rx quality   %d\n", Dev.getQuality());
  Serial.printf("Rx access    %d\n", Dev.getAccess());
  Serial.printf("Cell cid     %d\n", Dev.getCid());
  Serial.printf("Cell tac     %d\n", Dev.getTac());
  char mlts[322]; 
  Dev.getMlts(mlts, 322);
  Serial.printf("Cell mlts    %s\n", mlts);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  Serial.printf("Time Elapsed: %d mili seconds\n", millis() - t);
  mqtt_setup();

  char cmd_ciot[] = "AT+CCIOTOPT?";
  Serial.printf("cmd: %s\n", cmd_ciot);
  Ql_RIL_SendATCmd(cmd_ciot, Ql_strlen(cmd_ciot), cmd_callback, NULL, 2000);
  delay(100);

  char cmd_qband[] = "AT+QBAND?";
  Serial.printf("cmd: %s\n", cmd_qband);
  Ql_RIL_SendATCmd(cmd_qband, Ql_strlen(cmd_qband), cmd_callback, NULL, 2000);
  delay(100);
  
  char cmd_ced[] = "AT+CEDRXRDP";
  Serial.printf("cmd: %s\n", cmd_ced);
  Ql_RIL_SendATCmd(cmd_ced, Ql_strlen(cmd_ced), cmd_callback, NULL, 2000);
  delay(100);
}
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//

//------------------------------------------------LOOP------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
void loop()
{ 
  static int t = millis();
  static int sample = 1;
  static int message_num = 1;
  static char cmd_cscon[] = "AT+CSCON?";
  static char cmd_creg[] = "AT+CEREG?";

  if(millis()-t>500)
  {
    while(millis()-t>500)
    {
      t = t + 500;
    }
    
    if(sample<=500)
    {
      client.publish("te b device", "This is a 64-bytes-long message This is a 64-bytes-long message");
    }
    else if(sample>500 && sample<=1000)
    {
      client.publish("te b device", "This is a 32-bytes-long message");
    }
    else
    {
      client.publish("te b device", "This is 16B msg");
    }
    
    if(!(message_num%2))
    {
      //Serial.printf("Sample %d:   %d,%d dbm    Extended  ",sample++ , Dev.getReceiveLevel(), Dev.getQuality());
      Serial.printf("Sample %d\n",sample++);
      engineering_mode();
      get_extended_signal_quality();
    }
    if(!(message_num%10))
    {
      Serial.printf("%s: ", cmd_cscon);
      Ql_RIL_SendATCmd(cmd_cscon, Ql_strlen(cmd_cscon), cmd_callback_connection, NULL, 2000);
      Serial.printf("%s: ", cmd_creg);
      Ql_RIL_SendATCmd(cmd_creg, Ql_strlen(cmd_creg), cmd_callback, NULL, 2000);
    }
    message_num++;
  }
    
}
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------//
