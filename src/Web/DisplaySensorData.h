#ifdef WEB_H

#ifndef NUMSEMSORS
#define NUMSENSORS 10
#endif

/*
   Template.h - Class to create html page from collected data. 
*/
#include "Web.h";

class Template : public Web {
public:
    Template();
    void updateRootTemplate();

    struct data {
        String name;
        String unit;
	    String img;
        float data;
	    float min;
	    float max;
    };

   /**
    * Contains dataset to render into the web
    */
    data   datos[NUMSENSORS];

    String readTemplate();
private:
    String _boxData();
};

Template::Template() {
}

/**
 * Returns the thml to display on the root web page
 */
String Template::readTemplate() {
    return getRaiz();
}

void Template::updateRootTemplate() {
    String html;

    String cabecera = "<!DOCTYPE html>\
<html lang=es-ES>\
<head>\
<title>Datos meteorológicos</title>\
<meta charset=UTF-8>\
<style>div.caja{background-color:#005ac0;color:#fff;margin:2px auto;padding:20px;width:27%;min-width:170px;border-radius:15px;border-width:3px;border-color:#bff8ff;border-style:solid;min-height:103px;display:inline-block;text-align:left}div.img{float:left;height:112px}div.contenedor{max-width:93%;margin:0 auto;text-align:center}div.dato{min-height:112px}p.titulo{margin:0 auto 8px;font-size:26px}p.dato{font-size:30px;font-weight:700;margin:0 auto 8px}.tabla{display:table;width:100%;text-align:center}.fila{display:table-row}.celda{font-size:20px;display:table-cell}img{margin-right:25px}.pagina{background:linear-gradient(#003060,#09b8fc);max-width:90%;min-height:700px;border-radius:20px;margin:auto;padding:2%}body{font-family:arial,sans-serif}</style>\
</head>\
<body>\
<div class=pagina>\
<h1 style=text-align:center;color:aliceblue>Datos meteorológicos</h1>\
<div class=contenedor>\n";

    String pie = "</div>\
</body>\
</html>";

    html = cabecera + _boxData() + pie;

    setRaiz(html);
}

/**
 * Creates the a html div for each sesor
 */
String Template::_boxData() {
    String html;
    char cData[10];
    char cMin[10];
    char cMax[10];
    int  i = 0;

    for (auto dato : datos) {
	if (dato.name != "") {
	    if (datos[i].min > dato.data)
	        datos[i].min = dato.data;

	    if (datos[i].max < dato.data)
	        datos[i].max = dato.data;

	    dtostrf(dato.data, 8, 2, cData);
	    dtostrf(datos[i].max, 8, 2, cMax);
	    dtostrf(datos[i].min, 8, 2, cMin);

            i++;

            html = html + String("<div class=caja>\
<div class=dato>\
<div class=img>\
<img src='") + dato.img + String("' alt=' '>\
</div>\
<p class=titulo>") + dato.name + String(":</p>\
<p class=dato>") + cData + " " + dato.unit + String("</p>\
</div>\
<div class=tabla>\
<div class=fila>\
<div class=celda>Máx: ") + cMax + String("</div>\
<div class=celda>Mín: ") + cMin + String("</div>\
</div>\
</div>\
</div>");
	}
    }

    return html;
}
#endif
