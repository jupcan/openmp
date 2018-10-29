#include <QtGui/QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <stdio.h>
#include <omp.h>

/*  ----------------------
    a1 lab group
    Juan Perea Campos
    Javier Zaldivar Martín
    ----------------------  */

#define COLOUR_DEPTH 4
#define NUM_THREADS 4

int weight[3][3] = 	{{ 1,  2,  1 }, { 0,  0,  0 }, { -1,  -2,  -1 }};

double SobelBasico(QImage *srcImage, QImage *dstImage) {
	double start_time = omp_get_wtime();
  int pixelValue, ii, jj, blue;

  for (ii = 1; ii < srcImage->height() - 1; ii++) {  	// Recorremos la imagen pixel a pixel, excepto los bordes
    for (jj = 1; jj < srcImage->width() - 1; jj++) {

      // Aplicamos el kernel weight[3][3] al pixel y su entorno
      pixelValue = 0;
      for (int i = -1; i <= 1; i++) {	// Recorremos el kernel weight[3][3]
          for (int j = -1; j <= 1; j++) {
						blue = qBlue(srcImage->pixel(jj+j, ii+i));	// Sintaxis pixel: pixel(columna, fila), es decir pixel(x,y)
            pixelValue += weight[i + 1][j + 1] * blue;	// En pixelValue se calcula el componente y del gradiente
          }
      }
      if (pixelValue > 255) pixelValue = 255;
      if (pixelValue < 0) pixelValue = 0;
      dstImage->setPixel(jj,ii, QColor(pixelValue, pixelValue, pixelValue).rgba());	// Se actualiza la imagen destino
    }
  }
  return omp_get_wtime() - start_time;
}

/*  ----------------------
  	time SobelParallel (static,6): 0,309954822
		time SobelParallel (dynamic,6): 0,346445351
		time SobelParallel (static,image_height/num_of_cores): 0,311226297
		time SobelParallel (dynamic,image_height/num_of_cores): 0,281694964
    ----------------------  */

double SobelParallel(QImage *srcImage, QImage *dstImage) {
	double start_time = omp_get_wtime();

	#pragma omp parallel for schedule(dynamic,srcImage->height()/NUM_THREADS)
	for (int ii = 1; ii < srcImage->height() - 1; ii++) {  	// Recorremos la imagen pixel a pixel, excepto los bordes
		for (int jj = 1; jj < srcImage->width() - 1; jj++) {
			// Aplicamos el kernel weight[3][3] al pixel y su entorno
			int pixelValue = 0;
		  for (int i = -1; i <= 1; i++) {	// Recorremos el kernel weight[3][3]
		  	for (int j = -1; j <= 1; j++) {
					int blue = qBlue(srcImage->pixel(jj+j, ii+i));	// Sintaxis pixel: pixel(columna, fila), es decir pixel(x,y)
		      pixelValue += weight[i + 1][j + 1] * blue;	// En pixelValue se calcula el componente y del gradiente
		    }
		  }
		  if (pixelValue > 255) pixelValue = 255;
		  if (pixelValue < 0) pixelValue = 0;
			#pragma omp critical
		  dstImage->setPixel(jj,ii, QColor(pixelValue, pixelValue, pixelValue).rgba());	// Se actualiza la imagen destino
		}
	}
	return omp_get_wtime() - start_time;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    if (argc != 2) {printf("try again. use: <executable> <image file> \n"); return -1;}
    QPixmap qp = QPixmap(argv[1]);
    if(qp.isNull()) { printf("image not found\n"); return -1;}

    QImage image = qp.toImage();
    QImage sobelImage(image);
    double computeTime = SobelBasico(&image, &sobelImage);
    printf("sobel basic time: %0.9f seconds\n", computeTime);

		QImage auxImage(image);
		computeTime = SobelParallel(&image, &auxImage);
		printf("sobel parallel time: %0.9f seconds\n", computeTime);

		if (auxImage == sobelImage) printf("scanline and scanline parallel algorithms otuput images are the same\n");
		else printf("scanline and scanline parallel algorithms otuput images are different\n");

    QPixmap pixmap = pixmap.fromImage(auxImage);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    scene.addItem(item);

    view.show();
    return a.exec();
}

/*  ----------------------
    conclusion: the best time is obtained using dynamic,image_height/num_of_cores so
		that's the one we are keeping. sometimes we were getting a segmentation fault when
		executing so we were in need of protecting the writting in dstImage with a critical
		section, if not we get better result but the probability of program to fail.
    ----------------------  */
