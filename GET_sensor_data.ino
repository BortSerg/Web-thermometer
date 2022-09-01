//=================Получение тепературы с датчика DS18B20=================
float getTemperature() // 1-Wire ds18b20
{
  float temp;
  byte count = 0;
  do
  {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
    count++;
  } while ((temp <= (-50.0) || temp >= 85.0) & count <= 5);
  return temp;
}

//=================Функция вывода на ндикатор температуры на улице=================
void getTM1637Temperature() //
{
  int temp = getTemperature(); //временная локальная переменная
  tm1637.display(0, 30);       //  t
  tm1637.display(1, 27);       //  o
  tm1637.display(2, 32);       //  u
  tm1637.display(3, 30);       //  t
  delay(600);

  if (temp <= -10)
  {
    tm1637.display(0, 34);             // -
    tm1637.display(1, abs(temp) / 10); // temp
    tm1637.display(2, abs(temp) % 10); // temp
    tm1637.display(3, 36);             // degree
  }

  if ((temp > -10) & (temp < 0))
  {
    tm1637.display(0, 30);             // t
    tm1637.display(1, 34);             // -
    tm1637.display(2, abs(temp) % 10); // temp
    tm1637.display(3, 36);             // degree
  }

  if ((temp >= 0) & (temp < 10))
  {
    tm1637.display(0, 30);        // t
    tm1637.display(1, 35);        // NULL
    tm1637.display(2, temp % 10); // temp
    tm1637.display(3, 36);        // degree
  }

  if (temp >= 10)
  {
    tm1637.display(0, 30);        //  t
    tm1637.display(1, temp / 10); //  temp
    tm1637.display(2, temp % 10); //  temp
    tm1637.display(3, 36);        //  degree
  }
}