#ifdef WEB_H

#ifndef NUMSEMSORS
#define NUMSENSORS 10
#endif

/*
   DisplaySensorData.h - Class to create html page from collected data. 
*/
#include "Web.h";

class DisplaySensorData : public Web {
public:
    DisplaySensorData();
    void updateSensorData();

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

    String readSensorData();
private:
    String _boxData();
};

DisplaySensorData::DisplaySensorData() {
}

/**
 * Returns the thml to display on the root web page
 */
String DisplaySensorData::readSensorData() {
    return getRaiz();
}

void DisplaySensorData::updateSensorData() {
    String html;

    html = this->_header + _boxData() + WEB_FOOTER;

    setRaiz(html);
}

/**
 * Creates the a html div for each sesor
 */
String DisplaySensorData::_boxData() {
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
