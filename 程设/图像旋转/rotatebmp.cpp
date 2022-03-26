#include<iostream>
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
struct BMPHEADFILE
{
    DWORD bfSize;     //大小
    WORD bfReserved1; //保留字1
    WORD bfReserved2; //保留字2
    DWORD bfOffBits;  //偏移字节数
};
struct BMPINFOFILE
{
    DWORD biSize;          //大小
    DWORD biWidth;         //宽
    DWORD biHeight;        //高
    WORD biPlanes;         //平面数
    WORD biBitCount;       //每像素位数
    DWORD biCompression;   //压缩
    DWORD biSizeImage;     //压缩图字节数
    DWORD biXPelsPerMeter; //水平分辨率
    DWORD biYPelsPerMeter; //垂直分辨率
    DWORD biClrUsed;       //位图色彩数
    DWORD biClrImportant;  //位图重要的色彩数
};
struct RGBDATA
{
    BYTE blue;
    BYTE green;
    BYTE red;
};
int getZeroNum(BMPINFOFILE &info)
{
    int atc_width = (info.biWidth * info.biBitCount + 31) / 8;
    atc_width -= atc_width % 4;
    return atc_width - info.biWidth * info.biBitCount / 8;
}
int main(int argc,char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Wrong Argc!" << std::endl;
        return 0;
    }
    FILE *from;
    FILE *to;
    from = fopen(argv[1], "rb");
    to = fopen(argv[2], "wb");
    if (from == NULL)
    {
        std::cerr << "No Origin File!" << std::endl;
    }
    /*-------------basic information-----------*/
    WORD bfType;
    BMPHEADFILE head,nhead;
    BMPINFOFILE info,ninfo;
    fread(&bfType, 1, sizeof(WORD), from);
    fread(&head, 1, sizeof(BMPHEADFILE), from);
    fread(&info, 1, sizeof(BMPINFOFILE), from);
    nhead = head, ninfo = info;
    int height = info.biHeight, width = info.biWidth;
    int zero_need = getZeroNum(info);
    // std::cout << height << std::endl;
    int nheight = width, nwidth = height;
    ninfo.biWidth = (DWORD)(nwidth), ninfo.biHeight = (DWORD)(nheight);
    int nzero_need = getZeroNum(ninfo);
    ninfo.biSizeImage = (DWORD)((nwidth * 3 + nzero_need) * nheight);
    fwrite(&bfType, 1, sizeof(WORD), to);
    fwrite(&nhead, 1, sizeof(BMPHEADFILE), to);
    fwrite(&ninfo, 1, sizeof(BMPINFOFILE), to);
    /*-------------color information------------*/
    RGBDATA *DataColor = new RGBDATA[width * height];
    RGBDATA *nDataColor = new RGBDATA[nwidth * nheight];
    for (int i = 0; i < height; i++)
    {
        fread((char *)DataColor + i * width * 3, 3, width, from);
        fseek(from, zero_need, SEEK_CUR);
    }
    for (int i = 0; i < nheight; i++)
        for (int j = 0; j < nwidth; j++)
            *(nDataColor + i * nwidth + j) = *(DataColor + j * width + nheight - i - 1);
    for (int i = 0; i < nheight; i++)
    {
        fwrite((char *)nDataColor + i * nwidth * 3, 3, nwidth, to);
        fseek(to, nzero_need, SEEK_CUR);
    }
    fclose(from);
    fclose(to);
    delete[] DataColor;
    delete[] nDataColor;
    return 0;
}
