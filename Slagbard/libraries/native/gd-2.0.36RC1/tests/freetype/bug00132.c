/* $Id: bug00132.c,v 1.1.2.3 2007/11/21 12:27:17 pajoye Exp $ */

#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
 	gdImagePtr im, im2;
 	int error = 0;
	FILE *fp;
	char path[2048];
	const char *file_exp = "bug00132_exp.png";

	im = gdImageCreateTrueColor(50, 30);
    if (!im) {
        printf("can't get truecolor image\n");
        return 1;
    }

	gdImageAlphaBlending(im, 0);
	gdImageFilledRectangle(im, 0, 0, 200, 200, gdTrueColorAlpha(0, 0, 0, 127));

	sprintf(path, "%s/freetype/DejaVuSans.ttf", GDTEST_TOP_DIR);
	gdImageStringFT(im, NULL,  0xFFFFFF, path, 14.0, 0.0, 10, 20, "&thetasym; &theta;");

	sprintf(path, "%s/freetype/%s", GDTEST_TOP_DIR, file_exp);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
		printf("Reference image and destination differ\n");
	}

	gdImageDestroy(im);

	return error;
}
