#include <QtGui/QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <stdio.h>
#include <omp.h>
#include <math.h>

/*  ----------------------
    a1 lab group
    Juan Perea Campos
    Javier Zaldivar Martín
    ----------------------  */

#define COLOUR_DEPTH 4
#define NUM_THREADS 4

int weight_y[3][3]={{1,2,1},{0,0,0},{-1,-2,-1}};
int weight_x[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
int cache[3][2]={{0,0},{0,0},{0,0}};

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
            pixelValue += weight_y[i + 1][j + 1] * blue;	// En pixelValue se calcula el componente y del gradiente
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
		      pixelValue += weight_y[i + 1][j + 1] * blue;	// En pixelValue se calcula el componente y del gradiente
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

double SobelLocal(QImage *srcImage, QImage *dstImage) {
	double start_time = omp_get_wtime();
  int pixelValue, ii, jj;
	int cache[3][3] = {{0, 0, 0},{0, 0, 0},{0, 0, 0}};

  for (ii = 1; ii < srcImage->height() - 1; ii++) {  	// Recorremos la imagen pixel a pixel, excepto los bordes
		for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 0; i++) {
				cache[j+1][i+1] = qBlue(srcImage->pixel(1+i, 1+j));
			}
	  }
		for (jj = 1; jj < srcImage->width() - 1; jj++) {
      // Aplicamos el kernel weight[3][3] al pixel y su entorno
      pixelValue = 0;
			cache[0][2] = qBlue(srcImage->pixel(jj, ii-1));
			cache[1][2] = qBlue(srcImage->pixel(jj, ii));
			cache[2][2] = qBlue(srcImage->pixel(jj+1, ii+1));
      for (int i = -1; i <= 1; i++) {	// Recorremos el kernel weight[3][3]
          for (int j = -1; j <= 1; j++) {
            pixelValue += weight_y[i + 1][j + 1]*cache[i + 1][j + 1];	// En pixelValue se calcula el componente y del gradiente
          }
      }
      if (pixelValue > 255) pixelValue = 255;
      if (pixelValue < 0) pixelValue = 0;
			cache[0][0] = cache[0][1];
			cache[0][1] = cache[0][2];

			cache[1][0] = cache[1][1];
			cache[1][1] = cache[1][2];

			cache[2][0] = cache[2][1];
			cache[2][1] = cache[2][2];
      dstImage->setPixel(jj,ii, QColor(pixelValue, pixelValue, pixelValue).rgba());	// Se actualiza la imagen destino
    }
  }
  return omp_get_wtime() - start_time;
}

double SobelLocalParallel(QImage *srcImage, QImage *dstImage) {
	double start_time = omp_get_wtime();
	int cache[3][3] = {{0, 0, 0},{0, 0, 0},{0, 0, 0}};

	#pragma omp parallel for schedule(dynamic,srcImage->height()/NUM_THREADS) private(cache)
  for (int ii = 1; ii < srcImage->height() - 1; ii++) {  	// Recorremos la imagen pixel a pixel, excepto los bordes
		for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 0; i++) {
				cache[j+1][i+1] = qBlue(srcImage->pixel(1+i, 1+j));
			}
	  }
		for (int jj = 1; jj < srcImage->width() - 1; jj++) {
      // Aplicamos el kernel weight[3][3] al pixel y su entorno
      int pixelValue = 0;
			cache[0][2] = qBlue(srcImage->pixel(jj, ii-1));
			cache[1][2] = qBlue(srcImage->pixel(jj, ii));
			cache[2][2] = qBlue(srcImage->pixel(jj+1, ii+1));
      for (int i = -1; i <= 1; i++) {	// Recorremos el kernel weight[3][3]
          for (int j = -1; j <= 1; j++) {
            pixelValue += weight_y[i + 1][j + 1]*cache[i + 1][j + 1];	// En pixelValue se calcula el componente y del gradiente
          }
      }
      if (pixelValue > 255) pixelValue = 255;
      if (pixelValue < 0) pixelValue = 0;
			cache[0][0] = cache[0][1];
			cache[0][1] = cache[0][2];

			cache[1][0] = cache[1][1];
			cache[1][1] = cache[1][2];

			cache[2][0] = cache[2][1];
			cache[2][1] = cache[2][2];
			#pragma omp critical
      dstImage->setPixel(jj,ii, QColor(pixelValue, pixelValue, pixelValue).rgba());	// Se actualiza la imagen destino
    }
  }
  return omp_get_wtime() - start_time;
}

double SobelCompleto(QImage *srcImage, QImage *dstImage) {
	double start_time = omp_get_wtime();
  int pixelValueY, pixelValueX, gradient, ii, jj, blue;

  for (ii = 1; ii < srcImage->height() - 1; ii++) {  	// Recorremos la imagen pixel a pixel, excepto los bordes
    for (jj = 1; jj < srcImage->width() - 1; jj++) {

      // Aplicamos los dos kernels al pixel y su entorno
      pixelValueY = pixelValueX = 0;
      for (int i = -1; i <= 1; i++) {
          for (int j = -1; j <= 1; j++) {
						blue = qBlue(srcImage->pixel(jj+j, ii+i));
            pixelValueY += weight_y[i + 1][j + 1] * blue;
						pixelValueX += weight_x[i + 1][j + 1] * blue;
          }
      }
			gradient = sqrt((pow(pixelValueY,2) + pow(pixelValueX,2))); // Calculamos el gradiente
			int newPixel = gradient;
      if (newPixel > 255) newPixel = 255;
      if (newPixel < 0) newPixel = 0;
      dstImage->setPixel(jj,ii, QColor(newPixel, newPixel, newPixel).rgba());	// Se actualiza la imagen destino
    }
  }
  return omp_get_wtime() - start_time;
}

double SobelCompletoParallel(QImage *srcImage, QImage *dstImage) {
	double start_time = omp_get_wtime();

	#pragma omp parallel for schedule(dynamic,srcImage->height()/NUM_THREADS)
  for (int ii = 1; ii < srcImage->height() - 1; ii++) {  	// Recorremos la imagen pixel a pixel, excepto los bordes
    for (int jj = 1; jj < srcImage->width() - 1; jj++) {

      // Aplicamos los dos kernels al pixel y su entorno
      int pixelValueY = 0;
			int pixelValueX = 0;
      for (int i = -1; i <= 1; i++) {
          for (int j = -1; j <= 1; j++) {
						int blue = qBlue(srcImage->pixel(jj+j, ii+i));
            pixelValueY += weight_y[i + 1][j + 1] * blue;
						pixelValueX += weight_x[i + 1][j + 1] * blue;
          }
      }
			int gradient = sqrt((pow(pixelValueY,2) + pow(pixelValueX,2))); // Calculamos el gradiente
			int newPixel = gradient;
      if (newPixel > 255) newPixel = 255;
      if (newPixel < 0) newPixel = 0;
			#pragma omp critical
      dstImage->setPixel(jj,ii, QColor(newPixel, newPixel, newPixel).rgba());	// Se actualiza la imagen destino
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

		if (auxImage == sobelImage) printf("sobel sequetial and parallel algorithms otuput images are the same\n");
		else printf("sobel sequetial and parallel algorithms otuput images are different\n");

		QImage localImage(image);
		computeTime = SobelLocal(&image, &localImage);
		printf("sobel local time: %0.9f seconds\n", computeTime);

		QImage localPImage(image);
		computeTime = SobelLocalParallel(&image, &localPImage);
		printf("sobel local parallel time: %0.9f seconds\n", computeTime);

		if (localImage == sobelImage) printf("sobel sequential and sobel local algorithms otuput images are the same\n");
		else printf("sobel sequential and sobel local algorithms otuput images are different\n");

		QImage completeImage(image);
		computeTime = SobelCompleto(&image, &completeImage);
		printf("sobel complete time: %0.9f seconds\n", computeTime);

		QImage completePImage(image);
		computeTime = SobelCompletoParallel(&image, &completePImage);
		printf("sobel complete parallel time: %0.9f seconds\n", computeTime);

		if (completeImage == completePImage) printf("sobel complete and sobel complete parallel algorithms otuput images are the same\n");
		else printf("sobel complete and sobel complete parallel algorithms otuput images are different\n");

    QPixmap pixmap = pixmap.fromImage(completePImage);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    scene.addItem(item);

    view.show();
    return a.exec();
}

/*  ----------------------
    conclusion: local approach to compute sobel is way faster than the other methods since
		using less accesses to memory and thus efficiency is higher. we can also appreciate that
		in all cases the parallel versions are slower cause of critical sections and not all
		threads been able to write down the obtained results at once. the complete sobel is also
		as fast as just the sequential one despite applying both kernels which is not an operation
		to take into account time consuming talking, we can not say the same about pragmas using.
    ----------------------  */
