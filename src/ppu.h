#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
class PPU{
public:
    typedef struct {
        uint8_t y;
        uint8_t x;
        uint8_t tile;

        uint8_t f_cgb_pn : 3;
        uint8_t f_cgb_vram_bank : 1;
        uint8_t f_pn : 1;
        uint8_t f_x_flip : 1;
        uint8_t f_y_flip : 1;
        uint8_t f_bgp : 1;

    } oam_entry;

    /*
     Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
     Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
     Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
     Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
     Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
     Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
     */

   
        oam_entry oam_ram[40];
    

    void ppu_init();
    void ppu_tick();

    void ppu_oam_write(uint16_t address, uint8_t value);
    uint8_t ppu_oam_read(uint16_t address);


};
extern PPU ppu;

