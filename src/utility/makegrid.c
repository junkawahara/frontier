//
// makegrid.c
//
// Copyright (c) 2012 Jun Kawahara
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printdiaggrid(int m, int n)
{
	int i;
	int x, col, row, pos;
	int need_space;
	int count = 1;
	int* f;
	int* finv;

	f = (int*)calloc(m * n + 1, sizeof(int));
	if (f == NULL) {
		fprintf(stderr, "malloc error\n");
		exit(1);
	}
	finv = (int*)calloc(m * n + 1, sizeof(int));
	if (finv == NULL) {
		fprintf(stderr, "malloc error\n");
		exit(1);
	}

	for (x = 0; x <= m + n; ++x) {
		for (col = x; col >= 0; --col) {
			row = x - col;
			if (row >= 0 && col < n && row < m) {
				f[row * n + col + 1] = count;
				++count;
			}
		}
	}
	for (i = 1; i <= m * n; ++i) {
		finv[f[i]] = i;
	}

	for (i = 1; i <= m * n; ++i) {
		need_space = 0;
		pos = finv[i];
		if (pos > n) { // up edge
			printf("%d", f[pos - n]);
			need_space = 1;
		}
		if (pos % n != 1) { // left edge
			if (need_space) {
				printf(" ");
			}
			printf("%d", f[pos - 1]);
			need_space = 1;
		}
		if (pos % n != 0) { // right edge
			if (need_space) {
				printf(" ");
			}
			printf("%d", f[pos + 1]);
			need_space = 1;
		}
		if (pos <= n * (m - 1)) { // down edge
			if (need_space) {
				printf(" ");
			}
			printf("%d", f[pos + n]);
			need_space = 1;
		}
		printf("\n");
	}
	free(finv);
	free(f);
}

void printgrid(int m, int n)
{
	int i;
	int need_space;
	for (i = 1; i <= m * n; ++i) {
		need_space = 0;
		if (i > n) { // up edge
			printf("%d", i - n);
			need_space = 1;
		}
		if (n >= 2 && i % n != 1) { // left edge
			if (need_space) {
				printf(" ");
			}
			printf("%d", i - 1);
			need_space = 1;
		}
		if (i % n != 0) { // right edge
			if (need_space) {
				printf(" ");
			}
			printf("%d", i + 1);
			need_space = 1;
		}
		if (i <= (m - 1) * n) { // down edge
			if (need_space) {
				printf(" ");
			}
			printf("%d", i + n);
			need_space = 1;
		}
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	int i, m = -1, n = -1, x;
	int diagonal = 0;
	if (argc < 2 || argc > 4) {
		fprintf(stderr, "Usage: makegrid [-d] m [n]\n"
						"  Output an m x n (or m x m) grid. \n"
						"  If '-d' is given, the order of edges becomes diagonal.\n");
		exit(1);
	}

	for (i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-d") == 0) {
			diagonal = 1;
		} else {
			x = atoi(argv[i]);
			if (x <= 0) {
				fprintf(stderr, "Please input a positive integer.\n");
				exit(1);
			}
			if (m < 0) {
				m = x;
			} else if (n < 0) {
				n = x;
			}
		}
	}

	if (n < 0) {
		n = m;
	}

	if (diagonal) {
		printdiaggrid(m, n);
	} else {
		printgrid(m, n);
	}
	return 0;
}
