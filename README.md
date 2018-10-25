# openmp
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
1. creation of a [gray][g] image copy from a rgb one which must be the input
2. given a image, creation of its [histogram][h]

![](/files/graya.gif)
_*subtask1 working on ubuntu_

## [task2](/reqs/task2.pdf) 

[g]: https://github.com/jupcan/openmp/tree/master/graya
[h]: https://github.com/jupcan/openmp/tree/master/histogram
