## ESP8266 mikroszámítógép

Az ESP8266-os egy remek mikrovezérlő, ami azzal lett a hobbisták kedvence, hogy képes a WiFi-hez is kapcsolódni.
Az én mikroszámítógépem is erre az eszközre épül, de kiegészítettem egy karakteres LCD-vel és egy gombmátrixxal, illetve hordozhatóvá tettem egy lítiumakkumlátor és az ahhoz szükséges töltésvezértlő áramkör hozzáadásával.

### Hardver
 - ESP8266-os mikrovezérlő ( ARM processzormag, 32 bites, WiFi)
 - 16x2-es karakteres LCD
 - 4x3-as multiplex gombmátrix
 - lítiumion akkumlátor töltésvezérlő áramkörrel
 - egyéb kiegészítő alkatrészek
 - saját tervezésű 2 rétegű NYÁK

### Flash
Az ESP8266-hoz a WeMOS D1 mini modulban 4 MB flashmemórát társítottak, amiből én 2 MB-ot fenntartok a kód részére, a másik 2 MB-on pedig egy fájlrendszert tárolok.

### Lua

Az ESP8266-ra egy Lua interpretert telepítettem, amegy a flash-ben tárolt Lua nyelvű programokat képes futtatni.
Ezek a programok képesek kezelni az LCD-t, illetve olvasni a gombok állapotát. A WiFi API-k implementálása is folyamatban van.

Tervben van még egy webes felület létrehozása, ami lehetővé tenné a lua programok kábel nélküli feltöltését az eszközre.
