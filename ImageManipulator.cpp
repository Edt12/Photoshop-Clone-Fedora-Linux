#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <KHR/khrplatform.h>
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <array>
#include <complex>
#include <cstdlib>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <vector>

#define LOGFILEPATH "/home/ed/Documents/C++ Projects/Photoshop Clone/renderLog.txt"
struct imageDetails
{
    unsigned char* image;
    int width;
    int height;
    int numColourChannels;
};

struct pixel
{
    unsigned int r;
    unsigned int g;
    unsigned int b;

};
//converts array into array of pixels
pixel* convertToPixelArray(unsigned int* rawArray,imageDetails* imageDetails){
    pixel* pixelArray = new pixel[imageDetails-> height * imageDetails -> width];
    int rawArrayIndex = 0;
    for (size_t i = 0; i < (imageDetails->height * imageDetails->width); i++)
    {
        pixelArray[i] =  pixel{rawArray[rawArrayIndex],rawArray[rawArrayIndex + 1],rawArray[rawArrayIndex + 2]};
        rawArrayIndex += 3;
    };
    return pixelArray;
    
}

unsigned int* convertPixelArrayToRawArray(imageDetails* imageDetails ,pixel* pixelArray){
    unsigned int* rawArray = new unsigned int[(imageDetails->height * imageDetails->width) * 3];
    int rawArrayIndex = 0;
    for (size_t i = 0; i < imageDetails->height * imageDetails->width; i++)
    {
        rawArray[rawArrayIndex] = pixelArray[i].r;
        rawArray[rawArrayIndex + 1] = pixelArray[i].g;
        rawArray[rawArrayIndex + 2] = pixelArray[i].b;
        rawArrayIndex+= 3;
    }
    return rawArray;
}
//Reads Image into memory
imageDetails loadImage(const char* filename)
{
    
    int width, height, nrChannels;
    unsigned char* image = stbi_load(filename, &width, &height, &nrChannels, 0);
    std::ofstream logFile(LOGFILEPATH,std::ios::app);
    if (image == nullptr)
    {
        logFile << "Image Reading failed" << std::endl;
    }
    

   return imageDetails{ image,width,height,nrChannels };
}
//Produces a texture which is then displayed by the gui
unsigned int renderImage(const imageDetails *image_details, bool greyScale)
{
 
    //logFile.close();    
    
	unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
    //opengl assumes pixel allingment of 4 it thinks image starts on a memory address of 4 as most jpegs dont without it this breaks
    //We are telling it here expect memory addresses to be a multiple of 1
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    GLenum error2 = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error2 << std::endl;
    }
    if (greyScale)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_details->width, image_details->height, 0, GL_RED, GL_UNSIGNED_BYTE, image_details->image);
    }
    else {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_details->width, image_details->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_details->image);
    }

    if (greyScale)
    {
		//SWIZZLE IS NEEDED AS OPENGL DOES NOT HAVE A GREYSCALE MODE SO WE HAVE TO MOVE THE RED INTO ALL THE CHANNELS TO HAVE GREYSCALE MODE!!!!!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

    }
    GLenum error3 = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error3 << std::endl;
    }

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GLenum error4 = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error4 << std::endl;
    }


	glGenerateMipmap(GL_TEXTURE_2D);
    GLenum error5 = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL Error: " << error5 << std::endl;
    }
	return texture;
}
//Converts a char array to an unsigned int array
unsigned int * convertToIntArray( const imageDetails* image_details)
{
    int size = 0;
    unsigned int* imageIntArray = new unsigned int[(image_details->height * image_details->width) * 3];
    for (int i = 0; i < (image_details->height * image_details -> width) * 3; ++i)
    {
        size++;
    	imageIntArray[i] = static_cast<int> (image_details->image[i]);
    } 
    std::cout << "SIZE " << size <<"\n";
    return imageIntArray;
}
//Converts a unsigned int array to a char array
unsigned char* convertToCharArray(const imageDetails* image_details,unsigned int * modifiedImage,bool isGreyScale)
{

    unsigned char* modifiedImageChar = new unsigned char[(image_details->height * image_details->width) * 3];

    int multiplier = 3;
    if (isGreyScale)
    {
        multiplier = 1;
    }
    for (int i = 0; i < (image_details->height * image_details->width ) * multiplier ; ++i)
    {
	    if (modifiedImage[i] > 255)
	    {
            std::cout << "Greater\n";
	    }
	    else if (modifiedImage[i] < 0)
	    {
            std::cout<< "LESS";
	    }
        modifiedImageChar[i] = static_cast<unsigned char> (modifiedImage[i]);
    }
    return modifiedImageChar;
}

//Inverts Image e.g r -255 ,g -255,b -255 returns a texture
unsigned char* invertImage(imageDetails* image_details)
{

    unsigned int* imageDataInt  = convertToIntArray(image_details);
    assert(imageDataInt != nullptr);

  
    for (int i = 0; i < (image_details -> height * image_details -> width) * 3; ++i)
    {
        imageDataInt[i] = 255 - imageDataInt[i];
    }
    
	unsigned char* modifiedArray = convertToCharArray(image_details, imageDataInt,false);
    std::ofstream logFile(LOGFILEPATH,std::ios::app);
    logFile << "INVERT IMAGE OUTPUT\n";
    for (int i = 0; i < 12; ++i)
    {
	    logFile << "Value " << (int)image_details->image[i] << "\n";
    }
    return modifiedArray;

    
}


//Turns image greyscale returns a texture
unsigned char* makeGreyScale(imageDetails* image_details)
{
	std::cout << "STARTING GREY SCALE";
    unsigned int* imageDataInt = convertToIntArray(image_details);
    assert(imageDataInt != nullptr);

    unsigned int* greyScaleArray = new unsigned int[image_details -> width * image_details-> height];


    
    std::array<unsigned int, 3> rgb;

    int counter = 0;
    int greyScaleCounter = 0;
   
    for (int i = 0; i < ((image_details->width * image_details->height) * 3); ++i)
    {

        rgb[counter] = imageDataInt[i];
        counter++;
    	if (counter % 3 == 0)
	    {
            int greyScale = (rgb[0] + rgb[1] + rgb[2]) / 3;
            
            greyScaleArray[greyScaleCounter] = greyScale;
            greyScaleCounter++;
            counter = 0;
            
	    }
    }

    return convertToCharArray(image_details, greyScaleArray,true);
}

unsigned char* addBlackToEdges(imageDetails* image_details){
    const int BLACKBORDERIMAGEWIDTH = (((image_details -> width) + 6) * image_details->numColourChannels);
    const int BLACKBORDERIMAGEHEIGHT = (((image_details -> height) + 6) * image_details->numColourChannels); 



    unsigned int * imageDataInt = convertToIntArray(image_details);
    assert(imageDataInt != nullptr);

    const int BLACKBORDERIMAGESIZE = BLACKBORDERIMAGEWIDTH * BLACKBORDERIMAGEHEIGHT;
    const int IMAGEDATAINTSIZE = image_details->width * image_details-> height * image_details-> numColourChannels;
    //each pixel is 3 values so to border a image with black we add 6 to its width and 6 to its height on either side
    unsigned int * imageDataIntBlackBorder = new unsigned int[BLACKBORDERIMAGESIZE];

    bool first = false;
    bool second = false;
    bool third = false;
    bool fourth = false;
    int index;
    int rowNumber = 1;
    int widthIndex =0;
    
    pixel* pixelArray = convertToPixelArray(imageDataInt,image_details );
    unsigned int * newArray = convertPixelArrayToRawArray(image_details,pixelArray);
    /**
    for (index = 0; index < BLACKBORDERIMAGESIZE; index++)
    {
        widthIndex++;
        if (widthIndex == BLACKBORDERIMAGEWIDTH - 3 && index > BLACKBORDERIMAGEWIDTH){
            std::cout <<"1\n";
            for (int i = 0; i < 6; i++)
            {
                 imageDataIntBlackBorder[index] = 255;
                 if (i != 5)
                 {
                     index++;   
                 }
            
            }
            
           
            
        }else if (index == BLACKBORDERIMAGEWIDTH){
            for (size_t i = 0; i < 3; i++)
            {
                imageDataIntBlackBorder[index] = 0;
                if (i != 2)
                {
                    index++;
                }
                
            }
            
        }else if (index > (BLACKBORDERIMAGESIZE - BLACKBORDERIMAGEWIDTH) -1){
            std::cout << "2" << std::endl;
            imageDataIntBlackBorder[index] = 100;
        } else if (index < BLACKBORDERIMAGEWIDTH){
    
            imageDataIntBlackBorder[index] = 50;
           
        }else{
         imageDataIntBlackBorder[index] = imageDataInt[index - (BLACKBORDERIMAGESIZE - IMAGEDATAINTSIZE)];
       }
       if (widthIndex == BLACKBORDERIMAGEWIDTH - 3)
       {
        widthIndex = -2;
       }
       
       
    }
    //modify width and height to allign with new width and height as 1 pixel has been added to edge of image
    image_details ->height = image_details->height + 6;
    image_details -> width = image_details -> width + 6;
    */
    return convertToCharArray(image_details,newArray,false);

}
