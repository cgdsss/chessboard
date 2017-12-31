#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"
using namespace cv;
jmp_buf env;

void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

void draw_image (HPDF_Doc pdf, const char* filename, float x, float y)
{
    HPDF_Page page = HPDF_GetCurrentPage (pdf);
    HPDF_Image image;
    image = HPDF_LoadPngImageFromFile (pdf, filename);
    /* Draw image to the canvas. */
    HPDF_Page_DrawImage (page, image, 0, 0, x, y);
}

int main (int argc, char **argv)
{
    HPDF_Doc  pdf;
    HPDF_Font font;
    HPDF_Page page;
    char fname[256] = "chessboard";
    int length_size;
    int width_size;
    float grid_size;
    HPDF_Destination dst;
    grid_size = atof(argv[1]);
    length_size = atoi(argv[2]);
    width_size = atoi(argv[3]);
    std::cout << "grid size: " << grid_size << "mm" << std::endl;
    std::cout << "size num of length: " << length_size << std::endl;
    std::cout << "size num of width: " << width_size << std::endl;
    strcat (fname, ".pdf");

    int blockSize=75;
    int imageSize_length=blockSize*length_size;
    int imageSize_width=blockSize*width_size;
    Mat chessBoard(imageSize_width,imageSize_length,CV_8UC3,Scalar::all(0));
    unsigned char color=0;
     for(int i=0;i<imageSize_length;i=i+blockSize){
      color=~color;
       for(int j=0;j<imageSize_width;j=j+blockSize){
       Mat ROI=chessBoard(Rect(i,j,blockSize,blockSize));
       ROI.setTo(Scalar::all(color));
       color=~color;
      }
     }
    imwrite("chessboard.png", chessBoard);

    pdf = HPDF_New (error_handler, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        return 1;
    }
    /* error-handler */
    if (setjmp(env)) {
        HPDF_Free (pdf);
        return 1;
    }
    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    /* create default-font */
    font = HPDF_GetFont (pdf, "Helvetica", NULL);

    /* add a new page object. */
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetWidth (page, length_size * grid_size * 0.03937 * 72);
    HPDF_Page_SetHeight (page, width_size* grid_size * 0.03937 * 72);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    HPDF_Page_SetFontAndSize (page, font, 12);


    draw_image (pdf, "chessboard.png", length_size * grid_size * 0.03937 * 72, width_size* grid_size * 0.03937 * 72);


    /* save the document to a file */
    HPDF_SaveToFile (pdf, fname);

    /* clean up */
    HPDF_Free (pdf);

    std::cout << "chessboard: " << length_size * grid_size/10.0 << "cm x " << width_size* grid_size/10.0 << "cm \n";

    return 0;
}
