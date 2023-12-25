# MEDIAN FILTER

## PRINCIPLE
The Median filter smooths a signal by removing spikes. The median filter can be used both on images and on 1-dimensional signal problems, which will be covered here. The median is calculated by sorting a list of numbers and finding the number in the middle.

The median filter has a window size, determining how far it will look back. For example, a window size of 9 will have eight elements looking back and one new element. The filter returns the median of these nine elements.

## THEORY EXAMPLE
And ADC loads samples in a 5 sample window of the filter, at time `t0` the windows looks like this:
```
[2, 3, (5), 80, 81]
```
At time `t0` the median value is `5`. Note that the window is sorted.

At time `t1` the ADC returns a new sample with value `90`. Lets say the oldest inserted sample in the window is the one with value `3`.
```
t0:            [2, 3,  (5), 80, 81]
t1:  ADC -90-> [2, 5, (80), 81, 90] -> 80 median value a.k.a filtered output
                                    +-X-> 3 discarded
```
The new sample is correctly inserted in the window (keep sorting), the oldest one is popped and the filter returns `80` as the new median value.

## HOW TO USE THIS FILTER
`#include` the single [header file library](./median_filter.h) in your project. Look in the header description for a howto snippet.

## WHEN TO USE THIS FILTER

## REFENCES
-[wikipedia](https://en.wikipedia.org/wiki/Median_filter)
