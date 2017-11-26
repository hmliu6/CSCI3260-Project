#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

GLuint loadBMP_custom(const char * imagepath);

GLuint loadBMPtoTexture(const char * imagepath);

GLuint loadBMP_Data(const char * imagepath, unsigned char *& img_data, int & img_w, int & img_h);

#endif
