
#include <SPI.h>
#include <Ethernet.h>
// MAC адрес можно выбрать любой - на результат это не повлияет
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
/* IP адрес нужно выбирать исходя из IP адреса основного шлюза - выполнив команду ipconfig в командной строке, получим IPv4 и IPv6 адреса, а так же IP основного шлюза и маску подсети. Если основной шлюз имеет вид 192.168.0.1, то выберем например 192.168.0.177 
Основной шлюз можно узнать так же в настройках подключения.*/
IPAddress ip(192, 168, 1, 177);
// Выбираем порт, к которому будем подключаться. По умолчанию при HTTP запросе идет подключение в 80 порту, но можно выбрать любое значение
EthernetServer server(80);

char post;
char buf[30];
#define bufferMax 128
int pinState[] = {0, 0, 0, 0};  // Состояние пинов

boolean startGet=false;
String getData = "";
int pins []={7,6};
void setup() {
  Serial.begin(9600); // открываем монитор COM-порта
  while (!Serial) {
    ; // ждем подключения COM-порта
  }
  // начинаем интернет-соединение
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("ip сервера:");
  Serial.println(Ethernet.localIP());
}

void loop()
{
// ожидание подключения клиентов
EthernetClient client = server.available();
if (client)
{
      boolean currentLineIsBlank = true;
      while (client.connected())
      {
        if (client.available())
        {
            char c = client.read();
            if(startGet==true)// данные после '?'
                getData+=c; 
            if(c == '?') // начало сбора данных после '?'
                startGet=true;
            if (c == '\n' && currentLineIsBlank) // окончание получения
            {
                if(getData.length()<1) // запрос без get-данных
            {
                pinState[0]=0;
                pinState[1]=0;
            }
            else
            {
                pinState[0]=int(getData[5])-48;
                pinState[1]=int(getData[12])-48;
            }
              // отправка заголовков клиенту
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              // формирование страницы ответа
              client.println("<!DOCTYPE HTML>");
              client.println("<html>");
              client.println("<h3>Ethernet shield + LEDS</h3>");
              client.println("<form method='get'>");
              // светодиод 1
              client.print("<div>");
              client.print("led1 off<input type='radio' name='led1' value=0 onclick='document.getElementById(\"submit\").click();' ");
              if (pinState[0] == 0)
              client.print("checked");
              client.println(">");
              client.print("<input type='radio' name='led1' value=1 onclick='document.getElementById(\"submit\").click();' ");
              if (pinState[0] == 1)
              client.print("checked");
              client.println("> on");
              client.println("</div>");
              // светодиод 2
              client.print("<div>");
              client.print("led2 off<input type='radio' name='led2' value=0 onclick='document.getElementById(\"submit\").click();' ");
              if (pinState[1] == 0)
              client.print("checked");
              client.println(">");
              client.print("<input type='radio' name='led2' value=1 onclick='document.getElementById(\"submit\").click();' ");
              if (pinState[1] == 1)
              client.print("checked");
              client.println("> on");
              client.println("</div>");
              client.println("<input type='submit' id='submit' style='visibility:hidden;' value='Refresh'>");
              client.println("</form>");
              client.println("</html>");
              break;
            }
                if (c == '\n')
                  {currentLineIsBlank = true;}
                else if (c != '\r')
                  {currentLineIsBlank = false;}
        }
      }
}
// задержка для получения клиентом данных
delay(1);
// закрыть соединение
client.stop();
for(int i=0;i<2;i++) // светодиоды включить или выключить
{digitalWrite(pins[i],pinState[i]);}
startGet=false;
getData="";
}
