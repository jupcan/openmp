#include <QtGui/QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <stdio.h>
#include <omp.h>

#define N 5
const int M = N/2;

#define max(num1, num2) (num1>num2 ? num1 : num2)
#define min(num1, num2) (num1<num2 ? num1 : num2)
#define getGaussCoefficient(x, y) GaussMatrix[x + M][y + M]
#define getGaussCoefficient_v(y) GaussVector[y + M]
#define getGaussCoefficient_h(x) GaussVector[x + M]

int GaussMatrix[N][N] =	{{1,4,6,4,1},{4,16,24,16,4},{6,24,36,24,6},{4,16,24,16,4},{1,4,6,4,1}};
int GaussVector[N] = {1,4,6,4,1};
int height, width;

inline void convolution(QImage* image, QImage* result, int x, int y) {
  int i, j;
  int red, green, blue;
  int i_min, i_max, j_min, j_max;
  QRgb aux;
  red = green = blue = 0;
  i_min = max(-M, -x);
  i_max = min(M, width - x - 1);
  j_min = max(-M, -y);
  j_max = min(M, height - y - 1);

	for (j = j_min; j <= j_max ; j++)
	  for (i = i_min; i <= i_max; i++) {
	  int coef = getGaussCoefficient(i, j);
	  aux = image->pixel(x + i, y + j);
	  red += coef * qRed(aux);
	  green += coef * qGreen(aux);
	  blue += coef * qBlue(aux);
	};
  red /= 256; green /= 256; blue /= 256;
  result->setPixel(x, y, QColor(red, green, blue).rgba());
}

double naive_matrix(QImage* image, QImage* result) {
  double start_time = omp_get_wtime();
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width ; x++)
	  convolution(image, result, x, y);
  return omp_get_wtime() - start_time;
}

inline void convolution_1d_v(QImage* image, int x, int y, int* red, int* green, int* blue) {
	int j, j_min, j_max;
	QRgb aux;
	j_min = max(-M, -y);
	j_max = min(M, height - y - 1);

	for (j = j_min; j <= j_max ; j++) {
		int coef = getGaussCoefficient_v(M+j);
		aux = image->pixel(x, y + j);
		red[y*width+x] += coef * qRed(aux);
		green[y*width+x] += coef * qGreen(aux);
		blue[y*width+x] += coef * qBlue(aux);
	};
}

inline void convolution_1d_h(int* red, int* green, int* blue, int x, int y, QImage* result) {
  int i, i_min, i_max, red_aux, green_aux, blue_aux;
  i_min = max(-M, -x);
  i_max = min(M, width - x - 1);
  red_aux = green_aux = blue_aux = 0;

  for (i = i_min; i <= i_max; i++) {
    int coef = getGaussCoefficient_h(M+i);
    red_aux += coef * red[y*width+x];
    green_aux += coef * green[y*width+x];
    blue_aux += coef * blue[y*width+x];
  };

  red_aux /= 256; green_aux /= 256; blue_aux /= 256;
  result->setPixel(x, y, QColor(red_aux, green_aux, blue_aux).rgba());
}

double gaussian_vectors(QImage* image, QImage* result) {

  int *red = (int*)malloc(height * width * sizeof(int));
  int *green = (int*)malloc(height * width * sizeof(int));
  int *blue = (int*)malloc(height * width * sizeof(int));

  double start_time = omp_get_wtime();

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width ; x++)
		convolution_1d_v(image, x, y, red, green, blue);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width ; x++)
		convolution_1d_h(red, green, blue, x, y, result);

  return omp_get_wtime() - start_time;
}

double gaussian_vectors_parallel(QImage* image, QImage* result) {
  int *red = (int*)malloc(height * width * sizeof(int));
  int *green = (int*)malloc(height * width * sizeof(int));
  int *blue = (int*)malloc(height * width * sizeof(int));

  double start_time = omp_get_wtime();

  #pragma omp parallel for num_threads(N)
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width ; x++)
		convolution_1d_v(image, x, y, red, green, blue);

  #pragma omp parallel for num_threads(N)
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width ; x++)
		convolution_1d_h(red, green, blue, x, y, result);

  return omp_get_wtime() - start_time;
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QGraphicsScene scene;
  QGraphicsView view(&scene);
	double computeTime;

  if (argc != 2) {printf("Usage: <executable> <image file> \n"); return -1;}
  QPixmap qp = QPixmap(argv[1]);
  if(qp.isNull()) { printf("image not found\n"); return -1;}

	QImage srcImage(qp.toImage());
	height = srcImage.height();
	width = srcImage.width();

  QImage matrixImage(srcImage);
	computeTime = naive_matrix(&srcImage, &matrixImage);
	printf("naive matrix time: %0.9f seconds\n", computeTime);

  QImage vectorImage(srcImage);
  computeTime = gaussian_vectors(&srcImage, &vectorImage);
  printf("gaussian vectors time: %0.9f seconds\n", computeTime);

  QImage vectorPImage(srcImage);
  computeTime = gaussian_vectors(&srcImage, &vectorPImage);
  printf("gaussian vectors parallel time: %0.9f seconds\n", computeTime);

  QPixmap pixmap = pixmap.fromImage(vectorPImage);
  QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
  scene.addItem(item);

  view.show();
  return a.exec();
}

/*  ----------------------
    conclusion: parallel algorithm is a bit faster but it is not noticeable
    ----------------------  */
