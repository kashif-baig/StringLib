#include "StringLib.h"

void setup() {

  Serial.begin(115200);

  StringBuilder<64> strBuilder;

  // Build a string.
  strBuilder.append_P(PSTR("Cookie: "));
  strBuilder.append_P(PSTR("auth_ticket=wnRg1je2xmbXEBk; "));
  strBuilder.append_P(PSTR("test_cookie=helloworld"));

  // Parse string. NOTE StringReader is destructive.
  StringReader reader = StringReader(strBuilder.toString());

  reader.readUntil(" ");

  while (reader.available())
  {
    char *cookieItem = reader.readUntil("; ");

    StringReader cookie = StringReader(cookieItem);

    while (cookie.available())
    {
      char *s = cookie.readUntil("=");
      if (s)
      {
        Serial.println(s);
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
