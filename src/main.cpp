#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <windows.h>
#include <cpu.h>


/* Screen dimensions */
const int wt = 320;
const int ht = 288;
const int gbResX = 160;
const int gbResY = 144;
uint32_t pixels[160 * 144];
static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* texture = nullptr;
bool romLoaded = false;

int GetPixelLocationFromCoordinate(int x, int y) {
	return y * gbResX + x;
}
CPU::~CPU() {

}
CPU cpu;

void RenderToDisplay(std::string filepath) {
	std::fstream file(filepath + ".gbp", std::ios::in);  // Explicitly open for reading

	if (!file.is_open()) {  // Fixed typo: is.open() -> is_open()
		std::cerr << "Failed to open '" << filepath << ".gbp' file!" << std::endl;  // Fixed: test -> cerr
		// Changed from SRL_PPL_PRILINE to standard error code
	}

	// Read the entire file into a vector (dynamic array)
	std::vector<char> charArray;
	char ch;  // Fixed variable name: pp -> ch
	while (file.get(ch)) {
		charArray.push_back(ch);
	}
	charArray.erase(std::remove(charArray.begin(), charArray.end(), '\n'), charArray.end());

	// If you need a raw char array instead of vector:
	char* rawArray = new char[charArray.size()];  // Fixed syntax: char: -> char*, new char( -> new char[
	std::copy(charArray.begin(), charArray.end(), rawArray);

	// Don't forget to delete[] rawArray when done!




	for (int i = 0; i < (gbResX * gbResY); i++) {

	

		

		if (rawArray[i] == '0') {
			pixels[i] = 0xffffffff;
		}
		else if (rawArray[i] == '1') {
			pixels[i] = 0xffAAAAAA;
		}
		else if (rawArray[i] == '2') {
			pixels[i] = 0xff555555;
		}
		else {
			pixels[i] = 0xff000000;
		}
		

	}
	delete[] rawArray;
}
/* Called once at startup */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{


	if (!SDL_CreateWindowAndRenderer("GBGreen", wt, ht, 0, &window, &renderer)) {
		SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}



	float scaleX = (float)wt / gbResX;
	float scaleY = (float)ht / gbResY;

	SDL_SetRenderScale(renderer, scaleX, scaleY);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, gbResX, gbResY);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	if (!texture) {
		SDL_Log("Failed to create texture: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	RenderToDisplay("assets/title");
	if (argc <= 1)
	{
		std::cerr << "Path to ROM to be loaded must be given as argument\nType -help to see usage\n";
		romLoaded = false;
	}
	else {

		if (!cpu.LoadROM(argv[1])) //loading ROM provided as argument
		{
			std::cerr << "ROM could not be loaded. Possibly invalid path given\n";
			romLoaded = false;
		}
		else {
			romLoaded = true;
		}
	}





	return SDL_APP_CONTINUE;
}
void UpdatePixels() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// Fill pixel array with white (for testing)


	SDL_UpdateTexture(texture, NULL, pixels, gbResX * sizeof(uint32_t));

	// In SDL3, use SDL_RenderTexture instead of SDL_RenderCopy
	SDL_RenderTexture(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);
}
/* Called once per frame */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	if (romLoaded) {
		cpu.Cycle();
	}
	//cpu.d_PrintState();

	UpdatePixels();
	//Sleep(20);

	return SDL_APP_CONTINUE;
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
			exit(0);
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
	if (texture) SDL_DestroyTexture(texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);

	SDL_Log("shutting down emulator..");
}
