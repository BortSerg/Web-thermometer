void HTTP_Main(void)
{
  File f = SPIFFS.open("/main.html", "r");
  String result = f.readString();
  f.close();
  server.send ( 200, "text/html", result );
  
}

void HTTP_Settings_WIFI_AP(void)
{
  File f = SPIFFS.open("/wifi_ap.html", "r");
  String result = f.readString();
  f.close();
  server.send ( 200, "text/html", result );
}

void HTTP_Light(void)
{
  File f = SPIFFS.open("/light.html", "r");
  String result = f.readString();
  f.close();
  server.send ( 200, "text/html", result );
}

void HTTP_Config(void)
{
  File f = SPIFFS.open("/config.json", "r");
  String result = f.readString();
  f.close();
  Serial.println(result);
  server.send ( 200, "application/json", result );
}

void HTTP_Temperature(void)
{
  String result = "{\n\t\"temp\" : " + String(getTemperature()) + "\n}\n";
  Serial.print(result);
  server.send ( 200, "application/json", result );
}
