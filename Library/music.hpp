#include <stdio.h>
#include "Dependencies\irrKlang\irrKlang.h"

#if defined(WIN64)
#include <conio.h>
#else
#include "Dependencies\irrKlang\conio.h"
#endif


using namespace irrklang;


#pragma comment(lib, "irrKlang.lib")


ISoundEngine* engine;

int bgm()
{
	// start the sound engine with default parameters
	engine = createIrrKlangDevice();
	if (!engine)
	{
		printf("Could not startup engine\n");
		return 0; // error starting up the engine
	}


	// play some sound stream, looped
	engine->play2D("resource/audio/starwars.wav", true);

	printf("\nStart Music!\n");


	return 0;
}


void stop_bgm() {
	engine->drop(); // delete engine
}