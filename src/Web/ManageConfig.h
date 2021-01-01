#ifdef WEB_H

/*
   ManageConfig.h - Class to manage system config.
*/
#include "Web.h";

class ManageConfig : public Web {
public:
    ManageConfig();
    void updateConfig();

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

    String readConfig();
private:
    String _boxData();
};

ManageConfig::ManageConfig() {
}

/**
 * Returns the thml to display on the root web page
 */
String ManageConfig::readConfig() {
    return this->getHtml();
}

void ManageConfig::updateConfig() {
    String html;

    html = this->_header + this->_boxData() + WEB_FOOTER;

    this->setRaiz(html);
}

/**
 * Creates the a html div for each sesor
 */
String ManageConfig::_boxData() {
    String html;
    int  i = 0;

    for (auto dato : datos) {
	if (dato.name != "") {
            i++;

            html = html + String("<div class=caja>\
<div class=dato>\
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
