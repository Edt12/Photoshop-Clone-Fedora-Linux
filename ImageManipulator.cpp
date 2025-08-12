#include <cassert>
#include <ostream>
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
#include <cstdlib>
#include <filesystem>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <limits>
#define LOGFILEPATH "/home/ed/Documents/Photoshop-Clone-Fedora-Linux/renderLog.txt"
#include <cmath>
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

struct intermediateIntPixel
{
    int r;
    int g;
    int b;

};

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

//Overloads << operator when it is next to a pixel to ensure the values within can be printed
std::ostream& operator<<(std::ostream& os, pixel pixel)
{
    os << "{" << pixel.r << ", " << pixel.g<< ", " << pixel.b << "}";
    return os;
}
 
//converts array into array of pixels
pixel* convertToPixelArray(unsigned int* rawArray,imageDetails* imageDetails){
    pixel* pixelArray = new pixel[imageDetails-> height * imageDetails -> width];
    int rawArrayIndex = 0;

    for (size_t i = 0; i < (imageDetails->height * imageDetails->width); i++)  
    {
        pixelArray[i] =  pixel{rawArray[rawArrayIndex],rawArray[rawArrayIndex + 1],rawArray[rawArrayIndex + 2]};
        rawArrayIndex += 3;    
    
    }
    return pixelArray;
    
}
//Overload for taking in unsigned char and converting it to a c style array of pixels
pixel* convertToPixelArray(unsigned char* rawArray,imageDetails* imageDetails){
    unsigned int * rawArrayInt = convertToIntArray(imageDetails);
    pixel* pixelArray = new pixel[imageDetails-> height * imageDetails -> width];
    int rawArrayIndex = 0;

    for (size_t i = 0; i < (imageDetails->height * imageDetails->width); i++)
    {
        pixelArray[i] =  pixel{rawArrayInt[rawArrayIndex],rawArrayInt[rawArrayIndex + 1],rawArrayInt[rawArrayIndex + 2]};
        rawArrayIndex += 3;    
    
    }
    return pixelArray;
    
}

intermediateIntPixel* convertToIntermediatePixelArray(int* rawArray,imageDetails* imageDetails){
    intermediateIntPixel* pixelArray = new intermediateIntPixel[imageDetails-> height * imageDetails -> width];
    int rawArrayIndex = 0;

    for (size_t i = 0; i < (imageDetails->height * imageDetails->width); i++)  
    {
        pixelArray[i] =  intermediateIntPixel{rawArray[rawArrayIndex],rawArray[rawArrayIndex + 1],rawArray[rawArrayIndex + 2]};
        rawArrayIndex += 3;    
    
    }
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

int* convertIntermediatePixelArrayToSignedIntArray(imageDetails* imageDetails ,intermediateIntPixel* pixelArray){
    int* rawArray = new int[(imageDetails->height * imageDetails->width) * 3];
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

//Warning only use this post normalisation as otherwise will get incorrect valuies
unsigned int* convertIntermediatePixelArrayToRawArray(imageDetails* imageDetails ,intermediateIntPixel* pixelArray){
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
imageDetails loadImage(const char* filePath)
{
    
    int width, height, nrChannels;
    unsigned char* image = stbi_load(filePath, &width, &height, &nrChannels, 0);
    std::ofstream logFile(LOGFILEPATH,std::ios::app);
    if (image == nullptr)
    {
        logFile << "Image Reading failed" << std::endl;
        logFile << "Path used " << filePath << "\n";
    }
    

   return imageDetails{ image,width,height,nrChannels };
}
//Produces a texture which is then displayed by the gui
unsigned int renderImage(const imageDetails *image_details)
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
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_details->width, image_details->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_details->image);
    
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

//Converts a unsigned int array to a char array
unsigned char* convertToCharArray(const imageDetails* image_details,unsigned int * modifiedImage,bool isGreyScale) {
    unsigned char* modifiedImageChar = new unsigned char[(image_details->height * image_details->width) * 3];

    int multiplier = 3;
    if (isGreyScale)
    {
        multiplier = 1;
    }
    for (int i = 0; i < (image_details->height * image_details->width ) * multiplier ; ++i)
    {
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
unsigned char* makeGreyScale(imageDetails* imageDetailsInput)
{
	std::cout << "STARTING GREY SCALE";
    assert(imageDetailsInput != nullptr);
    //REWORK WITH PIXELS
    pixel* pixelArray = convertToPixelArray(imageDetailsInput->image,imageDetailsInput);

   
    for (int i = 0; i < ((imageDetailsInput->width * imageDetailsInput->height)); ++i)
    {
        unsigned int greyscaleValue = (pixelArray[i].r + pixelArray[i].b + pixelArray[i].g)/3;
        pixelArray[i] = pixel{greyscaleValue,greyscaleValue,greyscaleValue};
    }
    
    unsigned int * intArray = convertPixelArrayToRawArray(imageDetailsInput,pixelArray);
    return convertToCharArray(imageDetailsInput,intArray,false);
}



unsigned int* addBlackToEdges(imageDetails* image_details){
    const int BLACKBORDERIMAGEWIDTH = (((image_details -> width) + 2));
    const int BLACKBORDERIMAGEHEIGHT = (((image_details -> height) + 2)); 



    unsigned int * imageDataInt = convertToIntArray(image_details);
    assert(imageDataInt != nullptr);

    const int BLACKBORDERIMAGESIZE = BLACKBORDERIMAGEWIDTH * BLACKBORDERIMAGEHEIGHT;
    const int IMAGEDATAINTSIZE = image_details->width * image_details-> height;

    pixel* pixelBlackBorderArray = new pixel[BLACKBORDERIMAGESIZE];
    
    pixel* pixelArrayOriginal = convertToPixelArray(imageDataInt,image_details );
    int originalArrayIndex = 0;
    for (size_t y = 0; y < BLACKBORDERIMAGEHEIGHT; y++)
    {
        for (size_t x = 0; x < BLACKBORDERIMAGEWIDTH; x++)
        {
            int index = y * BLACKBORDERIMAGEWIDTH + x;
            if (y == 0||y == BLACKBORDERIMAGEHEIGHT - 1||x == BLACKBORDERIMAGEWIDTH -1||x == 0)
            {
                pixelBlackBorderArray[index] = pixel{0,0,0};//black
            } else
            {
                assert(originalArrayIndex < image_details->height * image_details->width);
                pixelBlackBorderArray[index] = pixelArrayOriginal[originalArrayIndex];
                originalArrayIndex++;                

            }
            assert(index < BLACKBORDERIMAGESIZE);
        }
        
        
    }
       
    //modify width and height to allign with new width and height as 1 pixel has been added to edge of image
    image_details ->height = image_details->height + 2;
    image_details -> width = image_details -> width + 2;
    unsigned int * blackBorderImageDataInt = convertPixelArrayToRawArray(image_details,pixelBlackBorderArray);


    return blackBorderImageDataInt;

}

unsigned char* crossCorrelate(std::array<int, 9 > filter,imageDetails* imageDetails){
    std::cout << "height "<< imageDetails->height<< " width" << imageDetails -> width << std::endl;
    unsigned int* blackEdgedImageArray = addBlackToEdges(imageDetails);
    std::cout << "new height "<< imageDetails->height<< " new width" << imageDetails -> width << std::endl;

    pixel* pixelArrayOriginal = convertToPixelArray(blackEdgedImageArray,imageDetails);
    intermediateIntPixel* intermediatePixelArray = new intermediateIntPixel[(imageDetails->width -2)* (imageDetails->height - 2)];
    intermediateIntPixel* normalisedPixelArray = new intermediateIntPixel[(imageDetails->width -2)* (imageDetails->height - 2)];
    pixel* pixelArrayOutput = new pixel[(imageDetails->width - 2) *(imageDetails -> height - 2)]; 
    int max;
    int min;
    bool first = true; 
    int intermediatePixelArrayIndex = 0;
    const size_t arrayComparisonSize = 4;
    for (size_t y = 1; y < imageDetails->height - 1; y++)
    {
        for (size_t x = 1; x < imageDetails->width - 1; x++)
        {
            
            int redProduct = 0;
            int blueProduct = 0;
            int greenProduct = 0;
            //multiplys by indexes and gets product around google cross correlation on images for more information
            //find some way of improving this so it works for things larger than 3 by 3 filters
            //for colour need the rgb and b band
            int yIndexOffset = -1;
            int filterIndex = 0;
            //3 for 3 by 3 filters
            if (first){
                std::cout << "CENTER Index " << y << " "<< x << std::endl;
            }   
            for (size_t i = 0; i < 3; i++)
            {
                if (first){
                    std::cout << "indexes y " << y - yIndexOffset << " x "<< x << std::endl;
                    std::cout << "indexes y " << y - yIndexOffset << " x "<< x -1 << std::endl;
                    std::cout << "indexes y " << y - yIndexOffset << " x "<< x + 1<< std::endl;


                }
                redProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x + 1))].r;
                greenProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x + 1))].g;
                blueProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x + 1))].b;
                filterIndex++;
            
            
                redProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x))].r;
                greenProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x))].g;
                blueProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x))].b;
                filterIndex++;
            
            
                redProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x - 1))].r;
                greenProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x - 1))].g;
                blueProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x - 1))].b;
                filterIndex++;
                yIndexOffset++;
            }

            if (first){
                max = redProduct;
                min = blueProduct;
                first = false;
            }
            std::array<int,arrayComparisonSize> comparedValuesMax = {redProduct,greenProduct,blueProduct,max};
            
            max = *std::max_element(comparedValuesMax.begin(),comparedValuesMax.end());
            comparedValuesMax[arrayComparisonSize - 1] = min;

            min = *std::min_element(comparedValuesMax.begin(),comparedValuesMax.end());
            assert(intermediatePixelArrayIndex < ((imageDetails->height - 2 )* (imageDetails->width - 2)));
            intermediatePixelArray[intermediatePixelArrayIndex] = intermediateIntPixel{redProduct,greenProduct,blueProduct};
            intermediatePixelArrayIndex++;
            
        }
    }

//Normalisation of values to between 0 and 255 std::cout << "Min " << min << "Max " << max << std::endl; std::cout << "Sample Value Pre Normalised  " << intermediatePixelArray[(imageDetails->width - 4) * (imageDetails-> height - 4)].r<< std::endl; //MAKE ANOTHER ARRAY SO YOU CAN SEE THE CHANGE BETTER
    for (size_t i = 0; i < ((imageDetails->width - 2) * (imageDetails->height - 2)); i++)
    {
        normalisedPixelArray[i].r =  (intermediatePixelArray[i].r - min) * 255/(max - min);
        normalisedPixelArray[i].g =  (intermediatePixelArray[i].g - min) * 255/(max - min);
        normalisedPixelArray[i].b =  (intermediatePixelArray[i].b - min) * 255/(max - min);
        
    }
    std::cout << "Sample Value Normalised " << intermediatePixelArray[(imageDetails->width - 4) * (imageDetails-> height - 4)].r<< std::endl;
    for (size_t i = 0; i < ((imageDetails-> width - 2) * (imageDetails->height - 2)); i++)
    {
        pixelArrayOutput[i].r =  static_cast<unsigned int>(normalisedPixelArray[i].r);
        pixelArrayOutput[i].g =  static_cast<unsigned int>(normalisedPixelArray[i].g);
        pixelArrayOutput[i].b =  static_cast<unsigned int>(normalisedPixelArray[i].b);

    }
    std::cout << "I " << intermediatePixelArray[0].r << " NEW " << pixelArrayOutput[0].r << std::endl;
    imageDetails->width = imageDetails->width -2;
    imageDetails->height = imageDetails -> height -2;

    return convertToCharArray(imageDetails,convertPixelArrayToRawArray(imageDetails,pixelArrayOutput),false);
}
//Cross correlates without normalising mostly used for edge detection
int* crossCorrelateRaw(std::array<int, 9 > filter,imageDetails* imageDetails){
    std::cout << "height "<< imageDetails->height<< " width" << imageDetails -> width << std::endl;
    unsigned int* blackEdgedImageArray = addBlackToEdges(imageDetails);
    std::cout << "new height "<< imageDetails->height<< " new width" << imageDetails -> width << std::endl;

    pixel* pixelArrayOriginal = convertToPixelArray(blackEdgedImageArray,imageDetails);
    intermediateIntPixel* intermediatePixelArray = new intermediateIntPixel[(imageDetails->width -2)* (imageDetails->height - 2)];
    intermediateIntPixel* normalisedPixelArray = new intermediateIntPixel[(imageDetails->width -2)* (imageDetails->height - 2)];
    pixel* pixelArrayOutput = new pixel[(imageDetails->width - 2) *(imageDetails -> height - 2)]; 
    bool first = true; 
    int intermediatePixelArrayIndex = 0;
    const size_t arrayComparisonSize = 4;
    for (size_t y = 1; y < imageDetails->height - 1; y++)
    {
        for (size_t x = 1; x < imageDetails->width - 1; x++)
        {
            
            int redProduct = 0;
            int blueProduct = 0;
            int greenProduct = 0;
            //multiplys by indexes and gets product around google cross correlation on images for more information
            //find some way of improving this so it works for things larger than 3 by 3 filters
            //for colour need the rgb and b band
            int yIndexOffset = -1;
            int filterIndex = 0;
            //3 for 3 by 3 filters
            if (first){
                std::cout << "CENTER Index " << y << " "<< x << std::endl;
                first = false;
            }   
            for (size_t i = 0; i < 3; i++)
            {
                if (first){
                    std::cout << "indexes y " << y - yIndexOffset << " x "<< x << std::endl;
                    std::cout << "indexes y " << y - yIndexOffset << " x "<< x -1 << std::endl;
                    std::cout << "indexes y " << y - yIndexOffset << " x "<< x + 1<< std::endl;


                }
                redProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x + 1))].r;
                greenProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x + 1))].g;
                blueProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x + 1))].b;
                filterIndex++;
            
            
                redProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x))].r;
                greenProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x))].g;
                blueProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x))].b;
                filterIndex++;
            
            
                redProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x - 1))].r;
                greenProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x - 1))].g;
                blueProduct += filter[filterIndex] * pixelArrayOriginal[((y - yIndexOffset) * imageDetails->width + (x - 1))].b;
                filterIndex++;
                yIndexOffset++;
            }
            assert(intermediatePixelArrayIndex < ((imageDetails->height - 2 )* (imageDetails->width - 2)));
            intermediatePixelArray[intermediatePixelArrayIndex] = intermediateIntPixel{redProduct,greenProduct,blueProduct};
            intermediatePixelArrayIndex++;

     
            
        }
    }
    return convertIntermediatePixelArrayToSignedIntArray(imageDetails,intermediatePixelArray);
    
}

unsigned char* copyRedToAllPixelValues(imageDetails* imageDetailsInput){
    pixel* pixels = convertToPixelArray(convertToIntArray(imageDetailsInput),imageDetailsInput);
    for (size_t i = 0; i < (imageDetailsInput->width * imageDetailsInput->height); i++)
    {
        pixels[i] = pixel{pixels[i].r,pixels[i].r,pixels[i].r};
    
    }
    unsigned int* intermediateIntArray = convertPixelArrayToRawArray(imageDetailsInput,pixels);
    unsigned char* outputArray = convertToCharArray(imageDetailsInput,intermediateIntArray,false);
    return outputArray;
}

unsigned char* detectEdges(imageDetails* imageDetailsInput){
    bool first = true;
    imageDetailsInput->image = makeGreyScale(imageDetailsInput);
    imageDetails sobelXDetails = imageDetails{imageDetailsInput->image,imageDetailsInput->width,imageDetailsInput->height,imageDetailsInput->numColourChannels};
    imageDetails sobelYDetails = imageDetails{imageDetailsInput->image,imageDetailsInput->width,imageDetailsInput->height,imageDetailsInput->numColourChannels};
    int* sobelXDetailsOutput = crossCorrelateRaw(std::array<int,9>{-1,0,1,-2,0,2,-1,0,1},&sobelXDetails);
    std::cout << "Sample Val 1 " << (unsigned int)sobelXDetails.image[0] << (unsigned int)sobelXDetails.image[1]<< (unsigned int)sobelXDetails.image[2] << std::endl;


    int* sobelYDetailsOutput = crossCorrelateRaw(std::array<int,9>{-1,-2,-1,0,0,0,1,2,1},&sobelYDetails);
    std::cout << "Sample Val 2 " <<(unsigned int)sobelYDetails.image[0] << " " << (unsigned int)sobelYDetails.image[1]<< " " <<  (unsigned int)sobelYDetails.image[2] << std::endl; 

    //turning into int arrays
    intermediateIntPixel * sobelXPixels = convertToIntermediatePixelArray(sobelXDetailsOutput,&sobelXDetails);
    intermediateIntPixel * sobelYPixels = convertToIntermediatePixelArray(sobelYDetailsOutput,&sobelYDetails);
    
    intermediateIntPixel * intermediateImage = new intermediateIntPixel[sobelXDetails.height * sobelXDetails.width];

    int max;
    int min;
    //perform sqrt(sobelX2 + sobelY2)
    for (size_t i = 0; i < imageDetailsInput->height * imageDetailsInput->width; i++)
    {   
        unsigned int gradiantMangitude = (sobelXPixels[i].r *sobelXPixels[i].r) + (sobelYPixels[i].r * sobelYPixels[i].r);
        gradiantMangitude = sqrt(gradiantMangitude);
        intermediateImage[i].r = gradiantMangitude;
        
        if (first){
            max = intermediateImage[i].r;
            min = intermediateImage[i].r;
            first = false;
        }
        intermediateImage[i].g = gradiantMangitude;

        intermediateImage[i].b = gradiantMangitude;

        
        //getting max and min so i can normalise tha values
        if (intermediateImage[i].r > max) {
            max = intermediateImage[i].r;
        }
        if (intermediateImage[i].g > max){
            max = intermediateImage[i].g;
            
        }
        if (intermediateImage[i].b > max){
            max = intermediateImage[i].b;
        }


        if (intermediateImage[i].r < min) {
            min = intermediateImage[i].b;
        }
        if (intermediateImage[i].g < min){
            min = intermediateImage[i].g;
            
        }
        if (intermediateImage[i].b < min){
            min = intermediateImage[i].b;
        }
        assert(i < (imageDetailsInput->height * imageDetailsInput->width));
    }
    std::cout << "Sample Val 3 " <<intermediateImage[0].r << " " << intermediateImage[0].g<< " "<< intermediateImage[0].b << std::endl;

    //Normalisation of values to between 0 and 255
    for (size_t i = 0; i < imageDetailsInput->height * imageDetailsInput->width; i++){
        intermediateImage[i].r =  (intermediateImage[i].r - min) * 255/(max - min);
        intermediateImage[i].g =  (intermediateImage[i].g - min) * 255/(max - min);
        intermediateImage[i].b =  (intermediateImage[i].b - min) * 255/(max - min);
    }
    std::cout << "Sample Val 4 " <<intermediateImage[0].r << " " << intermediateImage[0].g<< " "<< intermediateImage[0].b << std::endl;
    return convertToCharArray(imageDetailsInput,convertIntermediatePixelArrayToRawArray(imageDetailsInput,intermediateImage),false);


   
}
