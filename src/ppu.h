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
    struct Tile {
        uint8_t data[16];  // 2 bytes per row (8 rows)
    };

 
  
    struct Stat {
        union {
            uint8_t value; // Full register as a byte
            struct {
                uint8_t mode_flag : 2;              // bits 0-1
                uint8_t coincidence_flag : 1;       // bit 2
                uint8_t hblank_interrupt : 1;       // bit 3
                uint8_t vblank_interrupt : 1;       // bit 4
                uint8_t oam_interrupt : 1;          // bit 5
                uint8_t coincidence_interrupt : 1;  // bit 6
                uint8_t unused : 1;                 // bit 7 (unused/reserved)
            };
        };
    };

    Stat STAT;
    struct Control {
        union {
            uint8_t value; // Full register as a byte
            struct {
                uint8_t bgDisplay : 1;
                uint8_t spriteDisplayEnable : 1;
                uint8_t spriteSize : 1;  // True means 8x16 tiles
                uint8_t bgDisplaySelect : 1;
                uint8_t bgWindowDataSelect : 1;
                uint8_t windowEnable : 1;
                uint8_t windowDisplaySelect : 1;
                uint8_t lcdEnable : 1;
            };
        };
    };
    uint8_t LY = 0;    // Current scanline (0-153)
    uint8_t LYC = 0;   // Line Compare
    Control LCDC;
    uint8_t SCY = 0;   // Scroll Y
    uint8_t SCX = 0;   // Scroll X
    uint8_t WY = 0;    // Window Y position
    uint8_t WX = 0;    // Window X position
    uint8_t BGP = 0xFC;// Background palette

    uint32_t frameBuffer[160 * 144];

    uint32_t background[256 * 256];
    uint32_t palette[4] = {
     0xFFFFFFFF,
     0xFFAAAAAA,
     0xFF555555,
     0xFF000000,
    };

    int mode = 0;
    int modeclock = 0;
    bool can_render = false;
    uint8_t* scanline;
    uint32_t tick;
    oam_entry oam_ram[40];
    
    
    void ppu_init();
    void ppu_tick();
    void displayGraphics();

    void ppu_oam_write(uint16_t address, uint8_t value);
    uint8_t ppu_oam_read(uint16_t address);
    uint8_t getPixel(const Tile& tile, uint8_t x, uint8_t y);
    uint32_t getPixelColor(uint8_t pixelValue);
    PPU::Tile getTile(uint8_t tileIndex);

    //void renderPixel(uint8_t rx, uint8_t ry, uint8_t x, uint8_t y);

 

  

};
extern PPU ppu;

