#include <QApplication>
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

#define COLOUR_DEPTH 4 //to iterate by 4 (r, g, b, alpha)

double computeHistogramSequential(QImage *image) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();
  int histogram[256];
  memset(histogram, 0, sizeof(histogram));

  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    /*  1. tell qt each step is a rgb pixel
        2. we create a new gray pixel
        3. we asign a rgb sequence to the previously created gray pixel */
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    histogram[ii] = gray
    //*rgbpixel = QColor(gray, gray, gray).rgba();
  }
  return omp_get_wtime() - start_time;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QPixmap qp = QPixmap("test_1080p.bmp"); // ("c:\\test_1080p.bmp");
    if(qp.isNull())
    {
        printf("image not found\n");
		return -1;
    }

    QImage image = qp.toImage();
    QImage seqImage(image);
    double computeTime = computeHistogramSequential(&seqImage);
    printf("sequential time: %0.9f seconds\n", computeTime);

    QImage auxImage(image);
    computeTime = computeHistogramParallel(&auxImage);
    printf("parallel time: %0.9f seconds\n", computeTime);

	if (auxImage == seqImage) printf("sequential and parallel algorithms otuput images are the same\n");
	else printf("sequential and parallel algorithms output images are different\n");

    QPixmap pixmap = pixmap.fromImage(auxImage);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    scene.addItem(item);

    view.show();
    return a.exec();
}

/*  ----------------------
    conclusion: once having gone through all the different methods and its implementations
    we can see that the parallel executing times can be equal or bigger than the sequential
    ones, something that at first does not make much sense, but comes from the fact that
    despite doing it in a parallel way we have to write down results somewhere in memory and
    several threads can not write at once hence the times mentioned before. So, for some
    things, doing it in a parallel way can not give us better results time executing
    talking than doing it in a sequential one, although for big amount of computations it
    should always give better results and thus processor/computer perfomance.
    ----------------------  */
