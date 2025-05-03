#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cpu.h>
#include <tools.h>
#include <emulator.h>;
#include <cartridge.h>;
#include <bus.h>
#include <instructions.h>
#include <ext_instructions.h>
#include <io.h>
#include <mbc.h>
#include <timer.h>
#include <ppu.h>
#include <thread>
#include <interrupts.h>
CPU::~CPU() {}
Cartridge::~Cartridge() {}
Instruction::~Instruction() {}
ExtInstruction::~ExtInstruction() {}
IO::~IO() {}
CPU cpu;
Interrupt interrupts;
Tools tools;
Emulator emu;
Cartridge cart;
Bus bus;
Instruction insts;
ExtInstruction extInsts;
IO io;
MBC mbc;
Timer timer;
PPU ppu;


void UpdateCPU() {
	if (!emu.paused) {
		if (emu.romLoaded) {
			cpu.Cycle();
		}
	}

}




void UpdatePixels() {
	SDL_SetRenderDrawColor(emu.renderer, 0, 0, 0, 255);
	SDL_RenderClear(emu.renderer);

	// Fill pixel array with white (for testing)


	SDL_UpdateTexture(emu.texture, NULL, emu.pixels, emu.gbResX * sizeof(uint32_t));

	// In SDL3, use SDL_RenderTexture instead of SDL_RenderCopy
	SDL_RenderTexture(emu.renderer, emu.texture, NULL, NULL);

	SDL_RenderPresent(emu.renderer);
}
bool stay = true;




/* Called once per SDL event */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{

	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS; // exit the app
	}

	if (event->type == SDL_EVENT_QUIT)
	{
		exit(0);
	}

	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		if (event->key.key == SDLK_P)
		{
			if (stay)
				stay = false;
			else
				stay = true;

			//update_dbg_window();
		}
		if (event->key.key == SDLK_L) {

			std::ofstream configFile("cpulogs.txt");



			configFile << cpu.logdata << std::endl;

			configFile.close();
		}
		if (event->key.key == SDLK_O) {

			std::string thing;
			int aaa = 0;
			for (uint16_t i = 0x8000; i < 0x9FFF; i++) {
				//if(bus.bus_read(i) != 0x00)
				thing += (char)bus.bus_read(i);

			}
			std::ofstream configFile("vram_dump.txt");



			configFile << thing << std::endl;

			configFile.close();
		}
		if (event->key.key == SDLK_ESCAPE)
		{
			if (emu.paused)
				emu.paused = false;
			else
				emu.paused = true;
		}
	}
		


	return SDL_APP_CONTINUE;
}

/* Called once at shutdown */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{


	if (emu.texture) SDL_DestroyTexture(emu.texture);
	if (emu.renderer) SDL_DestroyRenderer(emu.renderer);
	if (emu.window) SDL_DestroyWindow(emu.window);

	SDL_Log("shutting down emulator..");
	exit(0);
}


int updatelog = 0;
void AppUpdates() {




	
		cpu.temp_t_cycles = 0;
		bool interrupted = interrupts.check();
		if (!interrupted)
		cpu.Cycle();

		timer.inc();




		//UpdatePixels();
		//SDL_Delay(1);
		emu.ticks++;
		

	
	updatelog++;

	
}
/* Called once at startup */
int main(int argc, char* argv[])
{


	if (!SDL_CreateWindowAndRenderer("GBGreen", emu.wt, emu.ht, 0, &emu.window, &emu.renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}



	float scaleX = (float)emu.wt / emu.gbResX;
	float scaleY = (float)emu.ht / emu.gbResY;


	SDL_SetRenderScale(emu.renderer, scaleX, scaleY);

	emu.texture = SDL_CreateTexture(emu.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, emu.gbResX, emu.gbResY);
	SDL_SetTextureScaleMode(emu.texture, SDL_SCALEMODE_NEAREST);
	cpu.initializeGameboy();

	if (!emu.texture) {
		SDL_Log("Failed to create texture: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	tools.getConfig();
	if (emu.selectedColorPallete == 0) {
		tools.readPaletteFile("palettes/Monochrome.gbcp");
	}
	else {
		tools.readPaletteFile("palettes/Green.gbcp");
	}
	tools.RenderToDisplay("assets/title");
	emu.running = true;
	emu.paused = false;
	emu.ticks = 0;
	if (argc <= 1)
	{
		std::cerr << "Path to ROM to be loaded must be given as argument\nType -help to see usage\n";
		emu.romLoaded = false;
	}
	else {

		if (!cart.LoadROM(argv[1])) //loading ROM provided as argument
		{
			std::cerr << "ROM could not be loaded. Possibly invalid path given.\n";
			emu.romLoaded = false;
		}
		else {
			emu.romLoaded = true;
		}
	}





	SDL_Event event;

	while (true) {
		while (SDL_PollEvent(&event)) {
			SDL_AppResult result = SDL_AppEvent(nullptr, &event);
			if (result == SDL_APP_SUCCESS) {
				SDL_AppQuit(nullptr, SDL_APP_SUCCESS);
				break;
			}
		}
		if (stay) {
			AppUpdates();
			

			if (updatelog == 144) {
				io.dbg_update();
				io.dbg_print();
				updatelog = 0;
				ppu.displayGraphics();

				UpdatePixels();

			}
		}
	}
	SDL_AppQuit(nullptr, SDL_APP_SUCCESS);
}