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

double computeHistogramSequential(QImage *image, int *h) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    /*  1. tell qt each step is a rgb pixel
        2. we create a new gray pixel
        3. we increment gray value in histogram */
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    h[gray]++; //pixels with gray value
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramParallelCritical(QImage *image, int *h) {
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

double computeHistogramParallelAtomic(QImage *image, int *h) {
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

static omp_lock_t lock[NUM_GREY];
void createLocks() {
   for (int i = 0; i < NUM_GREY; i++) {
		omp_init_lock(&lock[i]);
  }
}

double computeHistogramParallelLocks(QImage *image, int *h) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();
  createLocks();

  #pragma omp parallel for num_threads(NUM_THREADS)
  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    omp_set_lock(&lock[gray]);
      h[gray]++;
    omp_unset_lock(&lock[gray]);
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramParallelReduction(QImage *image, int *h) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  #pragma omp parallel for num_threads(NUM_THREADS) reduction(+:h[:NUM_GREY])
  for (int ii = 0; ii < image->byteCount(); ii += COLOUR_DEPTH) {
    QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + ii);
    int gray = qGray(*rgbpixel);
    h[gray] = h[gray] + 1; //to make reduction detect the sum
  }
  return omp_get_wtime() - start_time;
}

double computeHistogramManual(QImage *image, int *h) {
  double start_time = omp_get_wtime();
  uchar *pixelPtr = image->bits();

  for (int ii = 0; ii < NUM_THREADS; ii++) {
    int s = (ii*image->byteCount())/NUM_THREADS;
    int f = ((ii + 1)*image->byteCount())/NUM_THREADS;
    for (int jj = s; jj < f; jj += COLOUR_DEPTH) {
      QRgb* rgbpixel = reinterpret_cast<QRgb*>(pixelPtr + jj);
      int gray = qGray(*rgbpixel);
      h[gray]++;
    }
  }
  return omp_get_wtime() - start_time;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap qp = QPixmap("test_1080p.bmp"); //("c:\\test_1080p.bmp");
    if(qp.isNull())
    {
        printf("image not found\n");
		return -1;
    }

    QImage image = qp.toImage();
    double computeTime;
    int seqhis[NUM_GREY], prlchis[NUM_GREY], prlahis[NUM_GREY], prllhis[NUM_GREY], prlrhis[NUM_GREY], mnlhis[NUM_GREY];
    memset(seqhis, 0, NUM_GREY*sizeof(int));
    memset(prlchis, 0, NUM_GREY*sizeof(int));
    memset(prlahis, 0, NUM_GREY*sizeof(int));
    memset(prllhis, 0, NUM_GREY*sizeof(int));
    memset(prlrhis, 0, NUM_GREY*sizeof(int));
    memset(mnlhis, 0, NUM_GREY*sizeof(int));

    computeTime = computeHistogramSequential(&image, seqhis);
    printf("sequential time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelCritical(&image, prlchis);
    printf("parallel critical time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelAtomic(&image, prlahis);
    printf("parallel atomic time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelLocks(&image, prllhis);
    printf("parallel locks time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramParallelReduction(&image, prlrhis);
    printf("parallel reduction time: %0.9f seconds\n", computeTime);
    computeTime = computeHistogramManual(&image, mnlhis);
    printf("parallel manual time: %0.9f seconds\n", computeTime);

    /*we use memcmp to compare the bytes of memory of each histogram
      thus to see if they are identical or not*/
    if(memcmp(seqhis, prlrhis, NUM_GREY) == 0 && memcmp(prlrhis, prlahis, NUM_GREY) == 0 && memcmp(prlahis, prllhis, NUM_GREY) == 0 \
    && memcmp(prllhis, prlrhis, NUM_GREY) == 0 && memcmp(prlrhis, mnlhis, NUM_GREY) == 0) {
      printf("allr8 histograms are identical\n");
    } else printf("smth wrong histograms are different\n");
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

    we can also appreciate that the manual time is the lower one but this is also due to what
    we were explaining before, for computing the histogram is not that worth a parallel implementation.
    if we have to choose, reduction and manual are the way to in this case.
    ----------------------  */
