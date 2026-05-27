#include <stdio.h>
#include <stdlib.h>

void heapify(int *heap, int n, int i)
{
    int smallest = i, left = 2 * i + 1, right = 2 * i + 2;
    if (left < n && heap[left] < heap[smallest])
        smallest = left;
    if (right < n && heap[right] < heap[smallest])
        smallest = right;
    if (smallest != i)
    {
        int temp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = temp;
        heapify(heap, n, smallest);
    }
}

void buildHeap(int *heap, int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(heap, n, i);
}

int extractMin(int *heap, int *n)
{
    int min = heap[0];
    heap[0] = heap[--(*n)];
    heapify(heap, *n, 0);
    return min;
}

void insertHeap(int *heap, int *n, int val)
{
    heap[(*n)++] = val;
    int i = *n - 1;
    while (i && heap[(i - 1) / 2] > heap[i])
    {
        int temp = heap[i];
        heap[i] = heap[(i - 1) / 2];
        heap[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

int main(void)
{
    int n;
    scanf("%d", &n);
    int *heap = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &heap[i]);
    }
    buildHeap(heap, n);

    long long total = 0;
    while (n > 1)
    {
        int first = extractMin(heap, &n);
        int second = extractMin(heap, &n);
        int sum = first + second;
        total += sum;
        insertHeap(heap, &n, sum);
    }

    printf("%lld\n", total);
    free(heap);
    return 0;
}