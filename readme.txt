/*
	CSCI3260 Assignment 2 Keyboard / Mouse Events
	Name : Yau Yui Pan
	Student ID : 1155081383
*/

Manipulation:
	
	Lighting
	   Press q,w to control diffuse light
	   Press z,x to control specular light
	   Press " " (space) to passive mouse
	   Press (arrow) to control one of the car
	   Press s stop animation(airplane fly)





	Reset
		PRess "c" for reset scene


		
	//Selecting Controlling Object:  // function not for ordinary use just for debug
		Press "1","2","3","4" select object 
		/*
			#define INDEX_OF_JEEP 1
			#define INDEX_OF_AIRPLANE 2
			#define INDEX_OF_TOWER 3
			#define INDEX_OF_JEEP2 4
			#define INDEX_OF_JEEP3 5
			#define INDEX_OF_JEEP4 6
			#define INDEX_OF_JEEP5 7
		*/
		
		Rotating
			Press "r" or "t" for left/right rotation (base on one axis only)
			Press ","/"."/"/" for rotate base on x/y/z axis respectively
			// didnt check collision with ground
			// All object can only rotate base on one axis but with different anlge

		Scaling
			Press "f" for uniform scale up or "g" for uniform scale down 
			// aware than scaling may result overflowing to ground
			// didnt check collision with ground

		Translating
			Press "i" or "k" for y axis shift 
			Press "j" or "l" for x axis shift 
			Press "u" or "o" for z axis shift 
			// didnt check collision with ground