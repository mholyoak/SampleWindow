#include "MyBitmap.h"
#include <stdint.h>
#include <strings.h>
#include <stdio.h>
#include <algorithm>

typedef struct tagBITMAPFILEHEADER
{
    uint16_t bfType;  //specifies the file type
    uint32_t bfSize;  //specifies the size in bytes of the bitmap file
    uint16_t bfReserved1;  //reserved; must be 0
    uint16_t bfReserved2;  //reserved; must be 0
    uint32_t bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    uint32_t biSize;  //specifies the number of bytes required by the struct
    uint32_t biWidth;  //specifies width in pixels
    uint32_t biHeight;  //species height in pixels
    uint16_t biPlanes; //specifies the number of color planes, must be 1
    uint16_t biBitCount; //specifies the number of bit per pixel
    uint32_t biCompression;//spcifies the type of compression
    uint32_t biSizeImage;  //size of image in bytes
    uint32_t biXPelsPerMeter;  //number of pixels per meter in x axis
    uint32_t biYPelsPerMeter;  //number of pixels per meter in y axis
    uint32_t biClrUsed;  //number of colors used by th ebitmap
    uint32_t biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;


CMyBitmap::CMyBitmap(const QString yuvFileName, uint32_t imageWidth, uint32_t imageHeight)
{
    uint32_t bmpFileHeaderSize = sizeof(BITMAPFILEHEADER);
    uint32_t bmpInfoHeaderSize = sizeof(BITMAPINFOHEADER);
    size_t totalSize = size_t(bmpFileHeaderSize + bmpInfoHeaderSize + (imageWidth * imageHeight * 3));

    BITMAPFILEHEADER bmpHeader;

    memset(&bmpHeader, 0, bmpFileHeaderSize);
    bmpHeader.bfType = ('M' << 8) | 'B';
    bmpHeader.bfSize = uint32_t(totalSize);

    BITMAPINFOHEADER bmpInfo;

    memset(&bmpInfo, 0, bmpInfoHeaderSize);
    bmpInfo.biSize = bmpInfoHeaderSize;
    bmpInfo.biWidth = imageWidth;
    bmpInfo.biHeight = imageHeight;
    bmpInfo.biPlanes = 1;
    bmpInfo.biBitCount = 24;
    bmpInfo.biCompression = 0; // BI_RGB
    bmpInfo.biSizeImage = imageWidth * imageHeight * 3;

    _data.resize(int(totalSize));

    uint8_t* dataPtr = reinterpret_cast<uint8_t*>(_data.data());

    memset(dataPtr, 0, totalSize);
    //memcpy(dataPtr, &bmpHeader, sizeof(BITMAPFILEHEADER));

    dataPtr[0] = 'B';
    dataPtr[1] = 'M';
    dataPtr[2] = uint8_t(totalSize & 0x000000FF);
    dataPtr[3] = uint8_t((totalSize & 0x0000FF00) >> 8);
    dataPtr[4] = uint8_t((totalSize & 0x00FF0000) >> 16);
    dataPtr[5] = uint8_t((totalSize & 0xFF000000) >> 24);

    uint32_t imageOffset = 14 + 40;
    dataPtr[10] = uint8_t(imageOffset & 0xFF);
    dataPtr[11] = uint8_t((imageOffset >> 8) & 0xFF);
    dataPtr[12] = uint8_t((imageOffset >> 16) & 0xFF);
    dataPtr[13] = uint8_t((imageOffset >> 24) & 0xFF);

    memcpy(dataPtr + 14, &bmpInfo, sizeof(BITMAPINFOHEADER));

    QByteArray yuvData(int(totalSize), 0);
    uint8_t* yuvDataPtr = reinterpret_cast<uint8_t*>((yuvData.data()));

    FILE * topFileStream = fopen(yuvFileName.toStdString().c_str(), "r");
    assert(topFileStream);
    fread(yuvDataPtr, imageWidth * imageHeight * 3, 1, topFileStream);

    for (uint32_t row = 0; row < imageHeight; row++)
    {
        uint8_t* yRow = yuvDataPtr + (row * imageWidth);
        uint8_t* uRow = yuvDataPtr + (imageWidth * imageHeight) + (row * imageWidth);
        uint8_t* vRow = yuvDataPtr + (2 * imageWidth * imageHeight) + (row * imageWidth);

        uint8_t* rgbRow = dataPtr + imageOffset + (imageWidth * 3 * (imageHeight - row - 1));
        for (uint32_t col = 0; col < imageWidth; col++)
        {
            int yVal = int(yRow[col]) - 16;
            int uVal = int(uRow[col]) - 128;
            int vVal = int(vRow[col]) - 128;

            uint8_t redValue = uint8_t(std::min(255, (298 * yVal + 409 * vVal + 129) >> 8));
            uint8_t greenValue = uint8_t(std::max(0, (298 * yVal - 100 * uVal - 209 * vVal + 128) >> 8));
            uint8_t blueValue = uint8_t(std::min(255, (298 * yVal + 516 * uVal + 128) >> 8));

            /*
            uint8_t redValue = uint8_t(std::min(int(255), int(int(yRow[col] - 16) + int((1.403 * double(vRow[col]))))));
            uint8_t greenValue = uint8_t(std::max(int(0), int(yRow[col]) - int(0.344 * double(uRow[col]) - int(0.714 * double(vRow[col])))));
            uint8_t blueValue = uint8_t(std::min(int(255), int(yRow[col]) + int((1.770 * double(uRow[col])))));

            uint8_t redValue = uint8_t(uint8_t(uint16_t(yRow[col]) + uint16_t((1.403 * double(vRow[col])))));
            uint8_t greenValue = uint8_t(yRow[col] + uint16_t((0.344 * double(uRow[col])) - (0.714 * double(vRow[col]))));
            uint8_t blueValue = uint8_t(yRow[col] + uint16_t((1.770 * double(uRow[col]))));
            */
            rgbRow[col * 3] = blueValue;
            rgbRow[col * 3 + 1] = greenValue;
            rgbRow[col * 3 + 2] = redValue;
            redValue = 0;
        }
    }

    _pixmap.loadFromData(_data);
}


