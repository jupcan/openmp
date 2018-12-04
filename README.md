# openmp
[![uclm](https://img.shields.io/badge/uclm-project-red.svg?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAC9UlEQVR42o3S3UtTYRwH8F//QhBE3WT0elGr6CZUCLzoRUQt6ibICrESS1MDi7pJwcSXgsCGlG+1LFFzzpzONqduTp3TqVO36V7OzubZ2TznbDvn7NW5nmlXWdADPzg8/D6c3/N9HohSDPCrDgg53bDJByERj0OEpCCEE8cjXlrJaBbOcys2iHpp1OOBgN4MaG/b/BXHfKxgkwuaNvkQE6X8WNDiTI16qP/AicTlMElPeeXTtdTY7G1Kpa/iLU5dnONvhHDyH9hBJGEGu2RV2t93PWaXrb2xAO/kTJgMb5EUM9MGWZQJ5PrnTH9gMwYx2n865PLOrr5uK+XXcLV/YfUD3t5fFFgwN0Y89JzlTUcxb3PNc2YsjVHrdzAKBX1gh+KhsIXokgtJqbopxvIvEa7y600i38xSnXd4qpwa1zcTvcqGqNdHMBPzpzijHSDGcic2WV4Xj0QTGwptBd4meejTGb+gKcS+acMD1mj7Ro3OfcWE3fddnbJnKMRExMuYglbXWUCjjCTQitEBu2dQU05rFp6gsOrJftXzqI9d8gxpajzDk9XUqK6MVs+Xx9igLtnPmewz4GiRnEFprmxtbSXWO4crUCgVrs7hfDTyeLIpiBG29a6fBTxGlPkX116grQBrwnBHq+QCOD9LwflpQIDSNVAjM8IQSVWQfWN1lgZRQRLjH8WF7h5FJW9brww63I2c2WG0N/WkOUVSAHJADZ6BCXAIu/eiP9ehs79Do97xzxrbk5hdsYo9UlVejAnU0lOGFnvT932ubsW2A01WMUxml8Bo2l3QZD7ai+6wnLc5XyGnSuyslTC5UYOOUTJz/enBifR80GaXgjanDGAoJRMGU67Cj/0ZMJZ+DyzVrYdplT4PocXf2B4wWIrwVslJzcUCkB+4AiNHc1HlAMgFN7dr6EgWqC8VgrVeBI7mPkBPUZuUYfeGlehR7HGhbKYzi0F57BqMn7uVrN3Y9rYD0HMEontE4NMuK7yyyVS3WAmujqFd+Bcdh3NlWlsAggAAAABJRU5ErkJggg==&longCache=true&colorA=b30135&colorB=555555&style=for-the-badge)](https://www.uclm.es)  
computer architecture lab project
> use of openmp and qt graphical environment to solve several tasks specified by the teacher - [uclm](https://www.uclm.es/) computer science

## installation
**g++ compiler** -fopenmp flag which is available in all  major distributions, if not
```
sudo apt-get install g++
```
**qt environment** used in some tasks
```
sudo apt-get install qt4-qmake libqt4-dev
```

## [task1](/reqs/task1.pdf)
divided into two differents subtasks each of one tested in a sequential and parallel way:
1. creation of a [gray][g] image copy from a rgb one which must be the [input][i]
2. given a image, creation of its [histogram][h]

![](/files/graya.jpg)
_*subtask1 output_

## [task2](/reqs/task2.pdf)
given an image apply a [sobel][s] algorithm to generate its equivalent

![](/files/sobel.jpg)
_*task2 output_

## [task3](/reqs/task3.pdf)
given an image apply a [gaussian][gg] algorithm to remove noise from it and blur it

![](/files/gaussian.jpg)
_*task3 output_

[i]: https://github.com/jupcan/openmp/blob/master/files/test_1080p.bmp
[g]: https://github.com/jupcan/openmp/tree/master/graya
[h]: https://github.com/jupcan/openmp/tree/master/histogram
[s]: https://github.com/jupcan/openmp/tree/master/sobel
[gg]: https://github.com/jupcan/openmp/tree/master/gaussian
