#include "rgbimage.h"
#include "color.h"
#include "assert.h"

RGBImage::RGBImage( unsigned int Width, unsigned int Height)
{
    m_Width = Width;
    m_Height = Height;
    
    m_Image = new Color[m_Height*m_Width];
    //initialize?
    
}

RGBImage::~RGBImage()
{
    //delete m_Image;
}

void RGBImage::setPixelColor( unsigned int x, unsigned int y, const Color& c)
{
    if(x < this->width() && y < this->height())
    {
        m_Image[x + this->width()*y] = c;
    }
}

const Color& RGBImage::getPixelColor( unsigned int x, unsigned int y) const
{
    if(x < this->width() && y < this->height())
    {
        return m_Image[x + this->width()*y];
    }
    return Color(); //dummy (remove)
}

unsigned int RGBImage::width() const
{
    return this->m_Width;
}
unsigned int RGBImage::height() const
{
    
    return this->m_Height;
}

unsigned char RGBImage::convertColorChannel( float v)
{
    if(v<=0.0f)
    {
        return 0;
    }
    if(v>=1.0f)
    {
        return 255;
    }
    
    return (unsigned char) (v*255);
}

/* save as 24bit-rgb-bmp (windows bitmap)
 * bmp is composed of a bitmapfileheader (14 byte), a bitmapinfoheader (40 byte) and imagedata (24 bit * size = 3 byte * size)
 * order: blue, green, red */
bool RGBImage::saveToDisk( const char* Filename)
{
    // TODO: fopen, fwrite, fclose
    FILE * file;
    unsigned int w = width();
    unsigned int h = height();
    int filesize = 14 + 40 + 3 * w * h; //in byte
    
    // structure s.a https://de.wikipedia.org/wiki/Windows_Bitmap
    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
    
    // fill end of each line, so (width+padding)%4
    unsigned char bmppad[3] = {0,0,0};
    
    //filesize
    bmpfileheader[ 2] = (unsigned char)(filesize);
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);
    
    //width in pixel
    bmpinfoheader[ 4] = (unsigned char)(w);
    bmpinfoheader[ 5] = (unsigned char)(w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(w>>16);
    bmpinfoheader[ 7] = (unsigned char)(w>>24);
    
    //amount(height) in pixel
    //positiv = "bottom-up"-bitmap - negativ = "top-down"-bitmap
    bmpinfoheader[ 8] = (unsigned char)(h);
    bmpinfoheader[ 9] = (unsigned char)(h>> 8);
    bmpinfoheader[10] = (unsigned char)(h>>16);
    bmpinfoheader[11] = (unsigned char)(h>>24);
    
    
    file = fopen(Filename, "wb");    // "write", "binary file"
    if (file == NULL)
    {
        return false;
    }
    
    fwrite(bmpfileheader, 1, 14, file);
    fwrite(bmpinfoheader, 1, 40, file);
    
    //bilddata (size = w*h*3 + padding) for
    // length of each row: a multible of 4 bytes
    for(int i=h-1; i>=0; i--) //bottom-up
    {
        for (int j=0; j<w; j++)
        {
            unsigned char rgb[3] = {convertColorChannel(getPixelColor(j, i).B), convertColorChannel(getPixelColor(j, i).G), convertColorChannel(getPixelColor(j, i).R)};
            fwrite(rgb, sizeof(char), sizeof(rgb), file);
        }
        fwrite(bmppad, sizeof(char),(4-(w*3)%4)%4, file);
    }
    
    if(fclose(file) == EOF)
    {
        return false;
    }
    return true;
    
}

RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor) {
    assert(src.height() == dst.height());
    assert(src.width() == dst.width());
    
    float K[3][3] = {
        {1.0f, 0.0f, -1.0f},
        {2.0f, 0.0f, -2.0f},
        {1.0f, 0.0f, -1.0F}
    };
    
    //Transponierte Matrix für sumY
    float KT[3][3] = {
        { 1.0f,  2.0f,  1.0f},
        { 0.0f,  0.0f,  0.0f},
        {-1.0f, -2.0f, -1.0f}
    };
    
    for (int x = 1; x < src.height() - 2; x++) {
        for (int y = 1; y < src.width() - 2; y++) {
            float sumX = 0;
            float sumY = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    // nur einen Farbkanala nehmen, weil bei graustufen alle denselben Wert haben
                    float color = src.getPixelColor(x + i, y + j).R;
                    sumX += color * K[i + 1][j + 1];
                    sumY += color * KT[i + 1][j + 1];
                }
            }
            float sum = sqrt(sumX * sumX + sumY * sumY) * factor;
            dst.setPixelColor(x, y, Color(sum, sum, sum));
        }
    }
    
    return dst;
}
