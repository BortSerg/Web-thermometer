void Init_html() {
  
  server.on ( "/", HTTP_Main );                                       //Гдавная страница
  server.on ( "/wifi_ap", HTTP_Settings_WIFI_AP );                    //Страница с настройками wifi/ap
  server.on ( "/config", HTTP_Config );                               //Возвращает json строку с настройками
  server.on ( "/temperature", HTTP_Temperature );                     //Возвращает json строку с температурой
  server.on ( "/save", HTTP_handleSave );                             //Запись данных в память устройства/json файл
  server.on ( "/light", HTTP_Light );                                 //Страница с настройками яркости индикатора
  server.on ( "/reboot", HTTP_handleReboot );                         //Перезагрузка
  httpUpdater.setup(&server);                                         //обновления через web-интерфейс  http://{local_IP}/update

  server.begin();
}
