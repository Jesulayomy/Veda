#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html>\n"
                "<html lang=\"en\">\n"
                "  <head>\n"
                "    <meta charset=\"UTF-8\" />\n"
                "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n"
                "    <meta name=\"keywords\" content=\"HTML, CSS, Git, Layo\" />\n"
                "    <meta name=\"description\" content=\"Layo's HTML & CSS webpage\" />\n"
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
                "    <title>Html website</title>\n"
                "    <style>\n"
                "      img {\n"
                "        width: 60px;\n"
                "        height: 60px;\n"
                "        border-radius: 30px;\n"
                "        float: left;\n"
                "        margin-right: 20px;\n"
                "      }\n"
                "      p.username {\n"
                "        font-weight: bold;\n"
                "      }\n"
                "      em {\n"
                "        color: red;\n"
                "        font-style: normal;\n"
                "      }\n"
                "    </style>\n"
                "  </head>\n"
                "  <body>\n"
                "    <h1 id=\"top\">HTML and CSS</h1>\n"
                "    <img src=\"images/IMG_20220915_125908.jpg\" alt=\"Layo in 2022\" />\n"
                "    <p class=\"username\">@Jesulayomi11</p>\n"
                "    <p>\n"
                "      Layo on the long <em>needed</em> vacation!&copy; &lt;He was stressed&gt;\n"
                "    </p>\n"
                "    <a href=\"#section-html\">HTML</a>\n"
                "    <a href=\"#section-css\">Css</a>\n"
                "    <h2 id=\"section-html\">HTML</h2>\n"
                "    <p>\n"
                "      Modi, dolorum. Dolorem veniam voluptates saepe, omnis iste quo\n"
                "      perspiciatis temporibus quaerat doloremque velit deserunt magnam quibusdam\n"
                "      nam similique reiciendis adipisci provident odit ea, perferendis impedit\n"
                "      officia quisquam officiis?\n"
                "    </p>\n"
                "    <p>Inventore modi\n"
                "      nisi quasi et, eveniet ab similique officia sequi, accusantium corrupti\n"
                "      voluptas nostrum aspernatur obcaecati. Possimus quisquam ratione iste\n"
                "      incidunt excepturi. Ducimus distinctio tempore atque aliquam neque eum\n"
                "      nesciunt non officiis, aut deserunt illo maxime, enim earum aliquid\n"
                "      placeat. Totam cum iusto blanditiis! Corrupti eveniet eligendi dicta\n"
                "      voluptas!\n"
                "    </p>\n"
                "    <h2 id=\"section-css\">CSS</h2>\n"
                "    <p>\n"
                "      Lorem ipsum dolor sit amet consectetur adipisicing elit. Incidunt dolorem,\n"
                "      ab quasi officia a dolore laudantium quidem quisquam in voluptatum minus?\n"
                "      Modi reprehenderit suscipit consequuntur reiciendis. Corrupti, ullam!\n"
                "    <h4>Heading 4</h4>\n"
                "    <h5>Heading 5</h5>\n"
                "    <h6>Heading 6</h6>\n"
                "    <p>\n"
                "      <a href=\"company/contact.html\">Contact me</a>\n"
                "      <a href=\"#\">Top</a>\n"
                "      <a href=\"https://google.com\" target=\"_blank\">Google search</a>\n"
                "    </p>\n"
                "  </body>\n"
                "</html>\n";
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin("layomi", "1be36f1AINA");
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());

  // Define the route for the root path
  server.on("/", HTTP_GET, handleRoot);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  server.handleClient();  // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
}