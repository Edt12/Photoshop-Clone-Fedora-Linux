#pragma once
#include <array>
#include <imgui.h>

struct imageDetails
{
    unsigned char* image;
    int width;
    int height;
    int numColourChannels;
};

imageDetails loadImage(const char* filename);

unsigned int renderImage(const imageDetails* image_details,bool greyScale);

unsigned char* invertImage(imageDetails* image_details);

unsigned int* convertToIntArray(const imageDetails* image_details);

unsigned char* makeGreyScale(imageDetails* image_details);

unsigned int* addBlackToEdges(imageDetails* image_details);

unsigned char* crossCorrelate(std::array<int, 9 > filter,imageDetails* imageDetails);

unsigned char* convertToCharArray(const imageDetails* image_details,unsigned int * modifiedImage,bool isGreyScale);