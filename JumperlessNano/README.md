
```

opened nodeFile.txt

loading bridges from file


raw input file

bridges
{
1-2,
3-4,
5-6,
7-8,
9-10,
11-12,
12-13,
2-3,
10-11,
1-11,
0-7,
}

special functions
{
1-GND,
11-SUPPLY_5V,
6-I_P,
I_P-I_N,
}
    

splitting and cleaning up string

_
1-2,
3-4,
5-6,
7-8,
9-10,
11-12,
12-13,
2-3,
10-11,
1-11,
0-7,
^

_
1-GND,
11-SUPPLY_5V,
6-I_P,
I_P-I_N,
^

replacing special function names with defined ints

1-100,
11-105,
6-108,
108-109,



parsing bridges into array

[1-100],[11-105],[6-108],[108-109],[1-2],[3-4],[5-6],[7-8],[9-10],[11-12],[12-13],[2-3],[10-11],[1-11],[0-7],
bridge pairs = 15


connecting nodes into nets

1-GND
adding Node 1 to Net 1

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1                   {1-GND}                         3V3,5V,DAC_0,DAC_1
2       +5V             2               5V                      {0-0}                           GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS                   {0-0}                           I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS



11-5V
adding Node 11 to Net 2

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1                   {1-GND}                         3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS                   {0-0}                           I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS



6-I_POS
adding Node 6 to Net 6

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1                   {1-GND}                         3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6                 {6-I_POS}                       I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS



I_POS-I_NEG
Net 7 can't be combined with Net 6 due to Do Not Intersect rules, skipping (first net DNI to second net nodes)

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1                   {1-GND}                         3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6                 {6-I_POS}                       I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS



1-2
adding Node 2 to Net 1

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6                 {6-I_POS}                       I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS



3-4
found unused Net 8

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6                 {6-I_POS}                       I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               3,4                     {3-4}                           0


5-6
adding Node 5 to Net 6

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               3,4                     {3-4}                           0


7-8
found unused Net 9

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               3,4                     {3-4}                           0
9       Net 9           9               7,8                     {7-8}                           0


9-10
found unused Net 10

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11                   {11-5V}                         GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               3,4                     {3-4}                           0
9       Net 9           9               7,8                     {7-8}                           0
10      Net 10          10              9,10                    {9-10}                          0


11-12
adding Node 12 to Net 2

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12                {11-5V,11-12}                   GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               3,4                     {3-4}                           0
9       Net 9           9               7,8                     {7-8}                           0
10      Net 10          10              9,10                    {9-10}                          0


12-13
adding Node 13 to Net 2

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2                 {1-GND,1-2}                     3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12,13             {11-5V,11-12,12-13}             GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               3,4                     {3-4}                           0
9       Net 9           9               7,8                     {7-8}                           0
10      Net 10          10              9,10                    {9-10}                          0


2-3
found Node 3 in Net 8
combining Nets 1 and 8
deleted Net 8

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2,3,4             {1-GND,1-2,3-4}                 3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12,13             {11-5V,11-12,12-13}             GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               7,8                     {7-8}                           0
9       Net 9           9               9,10                    {9-10}                          0


10-11
found Node 10 in Net 9
combining Nets 2 and 9
deleted Net 9

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2,3,4             {1-GND,1-2,3-4}                 3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12,13,9,10        {11-5V,11-12,12-13,9-10}        GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               7,8                     {7-8}                           0


1-11
Net 2 can't be combined with Net 1 due to Do Not Intersect rules, skipping (first net DNI to second net nodes)

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2,3,4             {1-GND,1-2,3-4}                 3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12,13,9,10        {11-5V,11-12,12-13,9-10}        GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               7,8                     {7-8}                           0


0-7

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2,3,4             {1-GND,1-2,3-4}                 3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12,13,9,10        {11-5V,11-12,12-13,9-10}        GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               7,8                     {7-8}                           0


done


final netlist



Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
0       Empty Net       127             EMPTY_NET               {0-0}                           EMPTY_NET
1       GND             1               GND,1,2,3,4             {1-GND,1-2,3-4}                 3V3,5V,DAC_0,DAC_1
2       +5V             2               5V,11,12,13,9,10        {11-5V,11-12,12-13,9-10}        GND,3V3,DAC_0,DAC_1
3       +3.3V           3               3V3                     {0-0}                           GND,5V,DAC_0,DAC_1
4       DAC 0           4               DAC_0                   {0-0}                           GND,5V,3V3,DAC_1
5       DAC 1           5               DAC_1                   {0-0}                           GND,5V,3V3,DAC_0
6       I Sense +       6               I_POS,6,5               {6-I_POS,5-6}                   I_NEG
7       I Sense -       7               I_NEG                   {0-0}                           I_POS

Index   Name            Number          Nodes                   Bridges                         Do Not Intersects
8       Net 8           8               7,8                     {7-8}                           0
```
