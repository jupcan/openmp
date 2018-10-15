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
#define NUM_GREY 256
#define NUM_THREADS 4

double computeHistogramSequential(QImage *image, int h[]) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    h[gray]++; // # pixels with gray value
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramParallelCritical(QImage *image, int h[]) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    #pragma omp critical
    h[gray]++;
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramParallelAtomic(QImage *image, int h[]) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    #pragma omp atomic
    h[gray]++;
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramParallelLocks(QImage *image, int h[]) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    omp_set_lock(&(h[gray]));
    h[gray]++;
    omp_unset_lock(&(h[gray]));
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramParallelReduction(QImage *image, int h[]) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  #pragma omp parallel for num_threads(NUM_THREADS) reduction(+:h[NUM_GREY])
  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    h[gray]+=1; // to make reduction detect the sum
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramManual(QImage *image, int h[]) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();
  for (int jj = 0; jj < NUM_THREADS; jj++) {
    for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
      QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii + jj*(image->byteCount()/NUM_THREADS));
      int gray = qGray(*rgbpixel);
      h[gray]++;
    }
  }
  return omp_get_wtime() - start_time;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap qp = QPixmap("test_1080p.bmp"); // ("c:\\test_1080p.bmp");
    if(qp.isNull())
    {
        printf("image not found\n");
		return -1;
    }

    QImage image = qp.toImage();
    double computeTime;
    int seqhistogram[NUM_GREY], prlhistogram[NUM_GREY];
    memset(seqhistogram, 0, NUM_GREY*sizeof(int));
    memset(prlhistogram, 0, NUM_GREY*sizeof(int));

    computeTime = computeHistogramSequential(&image, seqhistogram);
    printf("sequential time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelCritical(&image, prlhistogram);
    printf("parallel critical time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelAtomic(&image, prlhistogram);
    printf("parallel atomic time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelLocks(&image, prlhistogram);
    printf("parallel locks time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelReduction(&image, prlhistogram);
    printf("parallel reduction time: %0.9f seconds\n", computeTime);

    if(memcmp(seqhistogram, prlhistogram, 256) == 0) printf("allr8 histograms are identical\n");
    return 0;
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
