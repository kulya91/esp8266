#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//搭建的话一般只用改动下面的配置文件，接受回调和发送部分
const char* ssid ="hhh";                  //无线网名字
const char* password ="123456789";        //无线网密码
const char* mqtt_server ="39.101.200.103";//你的服务器/局域网主机ip地址
int16_t port= 1883;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
/**
 * 设置WiFi
 */ 
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * 消息回调，在这里处理消息
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW); 
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }

}

/**
 * 断开重连
 */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
//初始化
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(115200);
  //连接wifi
  setup_wifi();
  //配置mqtt服务器地址和端口
  client.setServer(mqtt_server, port);
  //设置订阅消息回调
  client.setCallback(callback);
}
//循环任务
void loop() {
  //重连机制
  if (!client.connected()) {
    reconnect();
  }
  //不断监听信息
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    //每2s发布一次信息
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    //发送消息
    client.publish("huang", msg);
  }
}
