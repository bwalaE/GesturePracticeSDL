#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

//screen properties, which are set in init()
int SCREEN_WIDTH;
int SCREEN_HEIGHT;
int SCREEN_XPOS;
int SCREEN_YPOS;
bool display2 = false;

class MenuTexture {
public:
	//Initializes variables
	MenuTexture();

	//Deallocates memory
	~MenuTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Deallocates texture
	void free();

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL);

	//Gets image dimensions
	int getWidth();
	int getHeight();

	//image dimension setters
	void setWidth(int x);
	void setHeight(int x);

	//gets SDL_Texture
	SDL_Texture* getTexture();

private:
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Starts up SDL and creates window
bool init();

//loads the images onto their textures
bool loadImages();

//Loads media
bool loadMedia();

//fit and position screen
void fitWindow(int w, int h, int x, int y);

//Frees media and shuts down SDL
void close();

//loads a texture
SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//tri button surface
SDL_Surface* triBtnSurface = NULL;

//general renderer
SDL_Renderer* gRenderer = NULL;

//textures
MenuTexture triTexture;
MenuTexture logoTexture;

//rectangles for holding textures
SDL_Rect gTriRect;
SDL_Rect gLogoRect;

MenuTexture::MenuTexture() {
	//initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

MenuTexture::~MenuTexture() {
	//deallocate
	free();
}

bool MenuTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFE, 0x52, 0xE9));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void MenuTexture::free() {
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void MenuTexture::render(int x, int y, SDL_Rect* clip) {

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int MenuTexture::getWidth()
{
	return mWidth;
}

int MenuTexture::getHeight()
{
	return mHeight;
}

void MenuTexture::setWidth(int x) {
	mWidth = x;
}

void MenuTexture::setHeight(int x) {
	mHeight = x;
}

SDL_Texture* MenuTexture::getTexture() {
	return mTexture;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Monitor 1 or 2?
		if (display2 == false) { //monitor 1
			fitWindow(900, 797, 636, 26);
		}
		else { //monitor 2

		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		SDL_SetWindowPosition(gWindow, SCREEN_XPOS, SCREEN_YPOS);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else // window created successfully
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadImages() {
	bool success = true;

	//load images onto the textures
	if (!triTexture.loadFromFile("res/tri.png")) {
		printf("Failed to map png image to texture\n", SDL_GetError());
		success = false;
	}
	else if (!logoTexture.loadFromFile("res/logo.png")) {
		printf("Failed to map png image to texture\n", SDL_GetError());
		success = false;
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	if (!loadImages())
	{
		printf("Error loading images! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//setup rectangles
		gTriRect.x = 30;
		gTriRect.y = 500;
		gTriRect.w = 100;
		gTriRect.h = 100;

		gLogoRect.x = 0;
		gLogoRect.y = 0;
		gLogoRect.w = SCREEN_WIDTH - 20;
		gLogoRect.h = 200;
	}

	return success;
}

void fitWindow(int w, int h, int x, int y) {
	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
	SCREEN_XPOS = x;
	SCREEN_YPOS = y;
}

void close()
{
	//Free loaded image
	triTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}

				//make screen surface grey
				//SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0x3E, 0x3E, 0x3E));

				//Update the surface
				//SDL_UpdateWindowSurface(gWindow);

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0x50, 0x50, 0x50, 0xFF);
				SDL_RenderClear(gRenderer);

				//render menu
				//renderMenuImages();
				SDL_RenderSetViewport(gRenderer, &gLogoRect);
				SDL_RenderCopy(gRenderer, logoTexture.getTexture(), NULL, NULL);
				//renderButtons();
				SDL_RenderSetViewport(gRenderer, &gTriRect);
				SDL_RenderCopy(gRenderer, triTexture.getTexture(), NULL, NULL);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}