# MadLibrary
 Provide usefull resources, codes etc.
 
[Installation](#Installation) 
[Documentation](#Documentation)

# Installation
 It provides and easy installation.
 Download the Includes folder with the files and put in your project folder, then in your project include the MadLibrary.hpp.
```cpp
#include "Includes/MadLibrary.hpp"
```
Then you ready to go.

# Documentation
## Dijsktra
 
```cpp
void Dijkstra(std::vector<std::vector<uint32_t>> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<uint32_t>& distance);

void Dijkstra(MadLibrary::Matrix<uint32_t> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<uint32_t>& distance);
```
It is a Dijkstra pathfinding algorithm to find the shortest path from the source point to the all other in a graph.
See: [Dijkstra](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)

__Parameters__:
- std::vector<std::vector<uint32_t>> Graph: A 2d uint32_t Adjacency matrix using the std::vector. See: [Adjacency_matrix](https://en.wikipedia.org/wiki/Adjacency_matrix)
- MadLibrary::Matrix<uint32_t> Graph: A 2d uint32_t Adjacency matrix using the MadLibrary::Matrix. See: [Adjacency_matrix](https://en.wikipedia.org/wiki/Adjacency_matrix)
- uint32_t source: The source node, from where the paths are measured.
- std::vector<uint32_t>& previous: The std::vector where is the previous nodes are stored.
- std::vector<uint32_t>& distance: The std::vector where is the distance from the source are stored.

__Return Value__:
    There is no return value.
    
## Abs
```cpp
template<class DataType>
DataType Abs(DataType data);
```
It is function returning the absolute value of any data.

__Parameters__:
- DataType data: the data.

__Return Value__:
    Absolute value of any data.
    
## PolygonArea
```cpp
template<class DataType,class DataType2>
void PolygonArea(std::vector<DataType> X, std::vector<DataType> Y,DataType2 &temp);
```
It calculates the area of a given polygon that has no holes in it.
See: [Shoelace Formula](https://en.wikipedia.org/wiki/Shoelace_formula)

__Parameters__:
- std::vector<DataType> X: The st::vector of X coordinates of the edges.
- std::vector<DataType> Y: The st::vector of Y coordinates of the edges.
- DataType2 &temp: variable where is the area is stored

__Return Value__:
    There is no return value.

    
## MergeSort
```cpp
template<typename Iter>
void MergeSort(Iter beg, Iter end);
```
Sorting the element of a container.
See: [Merge Sort](https://en.wikipedia.org/wiki/Merge_sort)
Note: This code was written by [Zach Whaley](https://github.com/zachwhaley).

__Parameters__:
- Iter beg: Iterator to the begining of the container.
- Iter end: Iterator to the end of the container.

__Return Value__:
    There is no return value.

## StringToNumber
```cpp
template <class DataType,class RandomAccesIterator>
void StringToNumber(RandomAccesIterator begin,RandomAccesIterator end,DataType& toData);
```
Extract the data out of the string.

__Parameters__:
- RandomAccesIterator begin: RandomAccesIterator to the begining of the data.
- RandomAccesIterator end: RandomAccesIterator to the end of the data.
- DataType& toData: The variable to store the data.

__Return Value__:
    There is no return value.

## NumberSize
```cpp
template <class DataType>
uint32_t NumberSize(DataType Number);
```
Return the size of the number.

__Parameters__:
- DataType Number: The number.

__Return Value__:
    Return the size of the number.
    
## SimplePow
```cpp
template <class DataType>
DataType SimplePow(DataType Number,uint32_t exp);
```
Rraises the number to a positive whole number exponent.

__Parameters__:
- DataType Number: The number.
- uint32_t exp: The exponent.

__Return Value__:
    The new number.
    
## PrimeCheck
```cpp
template <class DataType>
bool PrimeCheck(DataType prime);
```
Check if the number is a prime.

__Parameters__:
- DataType prime: The number to check.

__Return Value__:
    Returns true if it's a prime, otherwise false.

## DeleteDigit
```cpp
template <class DataType>
DataType DeleteDigit(DataType Number,uint32_t pos);
```
Delete a digit from the number.

__Parameters__:
- DataType Number: The number to delete from.
- uint32_t pos: The position from to delete, zero indexed.

__Return Value__:
    Returns the new number.

## Map
```cpp
double Map(double value, double start1, double stop1, double start2, double stop2);
```
It creates a value between an interval based on the value provided and the original interval.
__Parameters__:
- double value: the value to map. Note: start1<=value<=stop1.
- double start1: the first interval mininum.
- double stop1: the first interval maximum.
- double start2: the map interval mininum.
- double stop2: the map interval maximum.

__Return Value__:
    Returns the mapped value.

## Matrix
```cpp
template <class DataType>
    class Matrix
    {
        private:
            std::vector<std::vector<DataType>> vect;
            uint32_t row,col;
        public:
            static Matrix<DataType> ZeroMatrix(uint32_t row,uint32_t col);
            static Matrix<DataType> ZeroMatrix(uint32_t number);
            static Matrix<DataType> IdentityMatrix(uint32_t size);
            Matrix(const uint32_t row,const uint32_t col, DataType fill);
            Matrix(const uint32_t row,const uint32_t col);
            Matrix();
            Matrix(const Matrix& other);
            void Fill(DataType data);
            void Resize(const uint32_t row,const uint32_t col);
            void Clean();
            DataType GetData(uint32_t row,uint32_t col) const;
            DataType GetDeterminant() const;
            uint32_t GetRow() const;
            uint32_t GetColumn() const;
            std::vector<DataType>& operator[](size_t position);
            std::vector<DataType> operator[](size_t position) const;
            Matrix<DataType> operator+(const Matrix<DataType> other);
            void operator=(const Matrix& other);
            Matrix<DataType> operator*(const Matrix<DataType> other);
            ~Matrix();
    };
```

It is a container, a representation of a mathematic matrix.

### **Functions**:

### Constructors
```cpp
Matrix(const uint32_t row,const uint32_t col, DataType fill);
Matrix(const uint32_t row,const uint32_t col);
Matrix();
Matrix(const Matrix& other);
```
1. Creates a Matrix and fills it with the given data.
2. Creates an empty Matrix.
3. Creates a blank Matrix object.
4. The copy constructor.
    
__Parameters__:
- const uint32_t row: The number of the rows in the Matrix.
- const uint32_t col: The number of the columns in the Matrix.
- DataType fill: The data to be filled with.

__Return Value__:
    There is no return value.

### ZeroMatrix
```cpp
static Matrix<DataType> ZeroMatrix(uint32_t row,uint32_t col);
static Matrix<DataType> ZeroMatrix(uint32_t number);
```
It creates a Zero Matrix.
See: [Zero Matrix](https://en.wikipedia.org/wiki/Zero_matrix)

__Parameters__:
- uint32_t row: The number of the rows in the Matrix.
- uint32_t col: The number of the columns in the Matrix.
- uint32_t number: The number of the columns and rows.

__Return Value__:
    Returns the newly created Matrix.

### IdentityMatrix
```cpp
static Matrix<DataType> IdentityMatrix(uint32_t size);
```
It creates an Identity Matrix.
See:[Identity Matrix](https://en.wikipedia.org/wiki/Identity_matrix)

__Parameters__:
- uint32_t number: The number of the columns and rows.

__Return Value__:
    Returns the newly created Matrix.

### Fill
```cpp
void Fill(DataType data);
```
Fills the Matrix with the given data.

__Parameters__:
- DataType data: The data to be filled with.

__Return Value__:
    There is no return value.

### Resize
```cpp
void Resize(const uint32_t row,const uint32_t col);
```
Resizes the Matrix.

__Parameters__:
- const uint32_t row: The number of rows.
- const uint32_t col: The number of columns.

__Return Value__:
    There is no return value.

### Clean
```cpp
void Clean();
```
It cleans the Matrix.

__Paramters__:
    There is no parameter.
    
__Return Value__:
    There is no return value.
    
### GetData
```cpp
DataType GetData(uint32_t row,uint32_t col) const;
```
It returns the data in the given position.

__Paramters__:
- uint32_t row: The row coordinate.
- uint32_t col: The column coordinate.
    
__Return Value__:
    Returns the data.

### GetRow
```cpp
uint32_t GetRow() const;
```
It returns the number of the rows.

__Paramters__:
    There is no parameter.
    
__Return Value__:
    Returns the number of the rows.
    
### GetColumn
```cpp
uint32_t GetColumn() const;
```
It returns the number of the columns.

__Paramters__:
    There is no parameter.
    
__Return Value__:
    Returns the number of the columns
    
### Operator[]
```cpp
std::vector<DataType>& operator[](size_t position);
std::vector<DataType> operator[](size_t position) const;
```
1. It access the data at a given position.
2. It returns the data at a given position.

Note: Because the Matrix uses std::vectors to store data if 

__Paramters__:
- size_t position: the position.
    
__Return Value__:
    Returns the number of the columns

### Operator+
```cpp
Matrix<DataType> operator+(const Matrix<DataType> other);
```

It adds the Matrices.
See: [Matrix Addition](https://en.wikipedia.org/wiki/Matrix_addition)

__Paramters__:
- const Matrix<DataType> other: The Matrix to add to.
    
__Return Value__:
    Returns the newly created Matrix.

### Operator=
```cpp
void operator=(const Matrix<DataType>& other);
```

It assigns the values to the Matrix.

__Paramters__:
- const Matrix<DataType>& other: The Matrix from the values are taken.
    
__Return Value__:
    There is no return value.

### Operator*
```cpp
Matrix<DataType> operator*(const Matrix<DataType> other);
```

It multiples the Matrices.
See: [Matrix Multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication)

__Paramters__:
- const Matrix<DataType> other: The Matrix to multiple with.
    
__Return Value__:
    There is no return value.

### Deconstructor
```cpp
~Matrix();
```

It frees up the used memory.

__Paramters__:
    There is no parameter.
    
__Return Value__:
    There is no return value.
    
### Deconstructor
```cpp
DataType GetDeterminant() const;
```

It calculates the Matrix determinant.
See: [Determinant](https://en.wikipedia.org/wiki/Determinant)

__Paramters__:
    There is no parameter.
    
__Return Value__:
    Returns the calculated determinant.
