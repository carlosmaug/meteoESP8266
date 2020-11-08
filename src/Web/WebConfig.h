//OTA
#ifndef UPDATE_PATH
#define UPDATE_PATH "/firmware"
#endif

#ifndef UPDATE_USERNAME
#define UPDATE_USERNAME "admin"
#endif

#ifndef UPDATE_PASSWORD
#define UPDATE_PASSWORD "admin"
#endif

#ifndef WEB_TITLE
#define WEB_TITLE "Datos meteorológicos"
#endif

#ifndef WEB_HEADER
#define WEB_HEADER  "<!DOCTYPE html>\
<html lang=es-ES>\
<head>\
<title>Datos meteorológicos</title>\
<meta charset=UTF-8>\
<style>div.caja{background-color:#005ac0;color:#fff;margin:2px auto;padding:20px;width:27%;min-width:170px;border-radius:15px;border-width:3px;border-color:#bff8ff;border-style:solid;min-height:103px;display:inline-block;text-align:left}div.img{float:left;height:112px}div.contenedor{max-width:93%;margin:0 auto;text-align:center}div.dato{min-height:112px}p.titulo{margin:0 auto 8px;font-size:26px}p.dato{font-size:30px;font-weight:700;margin:0 auto 8px}.tabla{display:table;width:100%;text-align:center}.fila{display:table-row}.celda{font-size:20px;display:table-cell}img{margin-right:25px}.pagina{background:linear-gradient(#003060,#09b8fc);max-width:90%;min-height:700px;border-radius:20px;margin:auto;padding:2%}body{font-family:arial,sans-serif}</style>\
</head>\
<body>\
<div class=pagina>\
<h1 style=text-align:center;color:aliceblue>{{TITLE}}</h1>\
<div class=contenedor>\n"
#endif

#ifndef WEB_FOOTER
#define WEB_FOOTER "</div></div>\
</body>\
</html>"
#endif
