#pragma once

#include <gl/freeglut.h>

class  Texture {
public:
	Texture(); 
	int  Load( const char *file , int clamp = GL_REPEAT  ) ;
	void minFilter(int filter); // set minification filter 
	void magFilter(int filter); // set magnification filter
	void wrapping(int mode);    // set wrapping mode
	static void replace();      // set replace shader
	static void modulate();     // set modulate shader
	static void off();          // disable Texture Mapping
	static void on();           // enable Texture Mapping
	void Unload();              // remove Texture
	void use();                 // activate Texture
	GLuint getId();

private:
	GLuint id;
	int width;
	int height;
	int mode;   // 3: RGB, 4: RGBA
	unsigned char* data;
	void flip();
} ;

