#include <cstring>
#include <fstream>
#include <iostream>



class Tools {

public:
    void RenderToDisplay(std::string filepath);
    uint32_t hexStringToUint32(const std::string& hexStr);
    void readPaletteFile(const std::string& filename);
    void saveConfig();
    void getConfig();
};
extern Tools tools;