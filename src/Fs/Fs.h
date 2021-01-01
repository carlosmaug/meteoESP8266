#ifndef DEBUG
#define DEBUG false
#endif
#ifndef FFS_H
#define FFS_H
/*
   Fs.h - Class to access file system
*/

class Files {
public:
    Files();
    bool   Open(char* path, char* mode);
    String Read();
    bool   Write(String content);
    void   Umount();

protected:    
    File f;
};

Files::Files() {
    SPIFFS.begin();
}

bool Files::Open(char* path, char* mode) {
    this->f = SPIFFS.open(path, mode);

    if (this->f)
        return true;

    return false;    
}	

String Files::Read() {
   return this->f.readString();
}

bool Files::Write(String content) {
   this-f.println(content);
}

void Files::Umount() {
    SPIFFS.end();
}	

#endif
