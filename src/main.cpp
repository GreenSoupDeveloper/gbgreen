#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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

CPU::~CPU() {}
Cartridge::~Cartridge() {}
Instruction::~Instruction() {}
ExtInstruction::~ExtInstruction() {}
CPU cpu;
Tools tools;
Emulator emu;
Cartridge cart;
Bus bus;
Instruction insts;
ExtInstruction extInsts;


/* Called once at startup */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
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
			std::cerr << "ROM could not be loaded. Possibly invalid path given\n";
			emu.romLoaded = false;
		}
		else {
			emu.romLoaded = true;
		}
	}







	return SDL_APP_CONTINUE;
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
		if (event->key.key == SDLK_ESCAPE)
		{
			if (emu.paused)
				emu.paused = false;
			else
				emu.paused = true;
		}
		if (emu.paused) {
			if (event->key.key == SDLK_UP)
			{
				emu.menuItemSelected -= 1;
				if (emu.menuItemOpened == 0) {
					if (emu.menuItemSelected < 0) {
						emu.menuItemSelected = 3;
					}
				}
				else {
					if (emu.menuItemSelected < 0) {
						emu.menuItemSelected = 0;
					}
				}
			}
			if (event->key.key == SDLK_DOWN)
			{
				emu.menuItemSelected += 1;
				if (emu.menuItemOpened == 0) {
					if (emu.menuItemSelected > 3) {
						emu.menuItemSelected = 0;
					}
				}
				else {
					if (emu.menuItemSelected > 0) {
						emu.menuItemSelected = 0;
					}
				}
			}
			if (event->key.key == SDLK_LEFT)
			{
				emu.menuItemOptionSelected -= 1;
				if (emu.menuItemSelected == 0) {
					if (emu.menuItemOptionSelected < 0) {
						emu.menuItemOptionSelected = 2;
					}


					emu.selectedColorPallete = emu.menuItemOptionSelected;
					tools.saveConfig();
				}
			}
			if (event->key.key == SDLK_RIGHT)
			{
				emu.menuItemOptionSelected += 1;

				if (emu.menuItemSelected == 0) {
					if (emu.menuItemOptionSelected > 2) {
						emu.menuItemOptionSelected = 0;
					}


					emu.selectedColorPallete = emu.menuItemOptionSelected;
					tools.saveConfig();
				}

			}
			if (event->key.key == SDLK_RETURN)
			{

				if (emu.menuItemSelected == 0) {
					if (emu.menuItemOpened == 0)
						emu.paused = false;

					//hi
				}
				else if (emu.menuItemSelected == 1) {
					if (emu.menuItemOpened == 0) {

						if (!cart.LoadROM("test.gb")) //loading ROM provided as argument
						{
							std::cerr << "ROM could not be loaded. Possibly invalid path given\n";
							emu.romLoaded = false;
						}
						else {
							emu.romLoaded = true;
						}
						emu.paused = false;
					}
				}
				else if (emu.menuItemSelected == 2) {
					if (emu.menuItemOpened == 0) {
						emu.menuItemOpened = 3;
						emu.paused = true;
						emu.menuItemSelected = 0;
						emu.menuItemOptionSelected = emu.selectedColorPallete;
					}

				}

				else if (emu.menuItemSelected == 3) {
					if (emu.menuItemOpened == 0) {
						emu.paused = false;
						exit(0);
					}
				}
			}
		}

		for (int i = 0; i < 16; ++i)
		{
			/*if (event->key.key == keymap[i])
			{
				chip8.set_keypad_value(i, 1);
			}*/
		}
	}

	if (event->type == SDL_EVENT_KEY_UP)
	{
		/*for (int i = 0; i < 16; ++i)
		{
			if (event->key.key == keymap[i])
			{
				chip8.set_keypad_value(i, 0);
			}
		}*/
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
}
/* Called once per frame */
SDL_AppResult SDL_AppIterate(void* appstate)
{

	if (emu.paused) {
		if (emu.menuItemOpened == 0) {
			if (emu.menuItemSelected == 0)
				tools.RenderToDisplay("assets/menu1_sel1");
			else if (emu.menuItemSelected == 1)
				tools.RenderToDisplay("assets/menu1_sel2");
			else if (emu.menuItemSelected == 2)
				tools.RenderToDisplay("assets/menu1_sel3");
			else
				tools.RenderToDisplay("assets/menu1_sel4");
		}
		else {
			if (emu.menuItemSelected == 0) {
				if (emu.menuItemOptionSelected == 0)
					tools.readPaletteFile("palettes/Monochrome.gbcp");
				else if (emu.menuItemOptionSelected == 1)
					tools.readPaletteFile("palettes/Green.gbcp");
				else
					tools.readPaletteFile("palettes/VeryGreen.gbcp");


				if (emu.menuItemOptionSelected == 0)
					tools.RenderToDisplay("assets/menu2_sel1-1");
				else if (emu.menuItemOptionSelected == 1)
					tools.RenderToDisplay("assets/menu2_sel1-2");
				else
					tools.RenderToDisplay("assets/menu2_sel1-3");
			}
		}

	}
	else {
		if (!emu.romLoaded) {
			tools.RenderToDisplay("assets/title");
			emu.menuItemSelected = 0;
			emu.menuItemOpened = 0;
		}
	}
	if (emu.romLoaded) {
		cpu.Cycle();
	}
	//cpu.d_PrintState();

	UpdatePixels();
	SDL_Delay(10);
	emu.ticks++;

	return SDL_APP_CONTINUE;
}