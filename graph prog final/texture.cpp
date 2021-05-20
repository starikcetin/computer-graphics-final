#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() {
	id = 0; width = 0; height = 0; data = nullptr; mode = 0;
}

int Texture::Load( const char *file , int wrap  ) {
	// open the image file
	data = stbi_load( file, &width, &height, &mode, 0 ) ;
	if ( data == NULL ) {
		printf("%s file not loaded\n", file);
		return  NULL ;
	}
	 flip() ;

	// Allocate unused Texture Names/id.
	glGenTextures(1, &id ) ;

	// In the first call : it creates a 2D Texture Object, 
	// and makes it active (called binding).
	glBindTexture( GL_TEXTURE_2D , id ) ;

	// clamping settings
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, wrap);

	// filter settings 
	// Minification Filter: GL_NEAREST (worst), GL_LINEAR, 
	// Mipmapping (Trilinear) Filters : 
	// GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, 
	// GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR (best) 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Magnification Filters : GL_NEAREST (blocky), GL_LINEAR (blurry)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if ( mode == 3 ) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	} else if ( mode == 4 ) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	} else return NULL ;
	
    // remove image from main memory. (since it was transferred to Texture object in Display Card's memory)
	stbi_image_free(data);
	data = NULL ;

	return 1; 
}

void  Texture::Unload() {
	if ( id == 0 ) {
		glDeleteTextures(1, &id ) ;
		id=0 ; width=0; height=0; mode = 0 ;
	}
}

void  Texture::flip() {
	int width_in_bytes = width * mode;
    unsigned char *top = NULL;
    unsigned char *bottom = NULL;
    unsigned char temp = 0;
    int half_height = height / 2;

    for (int row = 0; row < half_height; row++) {
       top = data + row * width_in_bytes;
       bottom = data + (height - row - 1) * width_in_bytes;
       for (int col = 0; col < width_in_bytes; col++) {
         temp = *top;
         *top = *bottom;
         *bottom = temp;
         top++;
         bottom++;
      }
    }
}

void Texture::use() {
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::minFilter(int filter) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture::magFilter(int filter) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::wrapping(int wrap) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::on() {
	glEnable(GL_TEXTURE_2D);
}
void Texture::off() {
	glDisable(GL_TEXTURE_2D);
}

void Texture::modulate() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Texture::replace() {
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

GLuint Texture::getId() {
	return id;
}
