#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "Pratic2.4"
#define WIFI_PASSWORD "Pratic@2001"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "chakrabortypratic032@gmail.com" 
#define AUTHOR_PASSWORD "bxknurcvmxjwgtww"

/* Recipient's email*/
#define RECIPIENT_EMAIL "chakrabortypratic12@gmail.com"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Declare the session config data */
ESP_Mail_Session session;

/* Declare the message class */
SMTP_Message message;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

void setup(){

  pinMode(2 , INPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);
  
  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Set the message headers */
  message.sender.name = "Pratic";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Help";
  message.addRecipient("Hospital", RECIPIENT_EMAIL);



  //Send raw text message
  String textMsg = "The user is in danger, needs help ASAP";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

}

void loop(){
   String send_mssg = "";
   long start_event = millis();

   while(send_mssg != "_" && (millis() - start_event <= 18000)){
      send_mssg = Serial.readStringUntil('\n');
      send_mssg.trim();
   }
   
   
   if(send_mssg != "_"){
    Serial.println("Into DeepSleep");
    ESP.deepSleep(0); 
   }
   
   else if (send_mssg == "_"){
    smtp.connect(&session);
    MailClient.sendMail(&smtp, &message);
    Serial.println("Into DeepSleep");
    ESP.deepSleep(0);
   }
     
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}
