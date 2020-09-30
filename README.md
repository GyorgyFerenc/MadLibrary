# MadLibrary
 Provide usefull resources, codes etc.
 
- [Installation](#Installation) 
- [Documentation](#Documentation)

# Installation
 It provides and easy installation.
 Download the Includes folder with the files and put in your project folder, then in your project include the MadLibrary.hpp.
```cpp
#include "Includes/MadLibrary.hpp"
```
Then you ready to go.

# Documentation

- [Simple Functions](#Simple Functions) 
- [Matrix](#Matrix)
- [Graph](#Graph)

## Simple Functions

### toCleverString
```cpp
template <typename DataType>
std::string toCleverString(DataType data);
```
1. It formats the data into a string.

__Parameters__:
- DataType data: The data to format.

__Return Value__:
    The formated string.

### StringParser
```cpp
std::vector<std::string> StringParser(std::string TheString);
std::vector<std::string> StringParser(std::string TheString, bool CheckForWhiteSpaces, std::vector<char> OtherCharToCheckFor);
```
1. It divides the string into separate parts according to whitespaces.
2. It divides the string into separate parts according to the given chars.

__Parameters__:
- std::string TheString: The string to parse.
- bool CheckForWhiteSpaces:If it's true then it looks for whitespaces too, otherwise not.
- std::vector<char> OtherCharToCheckFor: The vector of chars to look for.

__Return Value__:
    The vector of strings.

### AppendVectors
```cpp
template <typename DataType>
std::vector<DataType> AppendVectors(std::vector<DataType> &First, const std::vector<DataType> &Second);
```
It appends the second vector to the first, stores it in the first and returns it as well.

__Parameters__:
- std::vector<DataType> &First: The first vector.
- std::vector<DataType> &Second: Iterator to the end of the container.

__Return Value__:
    Returns the combined vector.

### Abs
```cpp
template<class DataType>
DataType Abs(DataType data);
```
It is function returning the absolute value of any data.

__Parameters__:
- DataType data: the data.

__Return Value__:
    Absolute value of any data.
    
### PolygonArea
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

### MergeSort
```cpp
template<typename Iter>
void MergeSort(Iter beg, Iter end);
template <class Iter, class Comper>
void MergeSort(Iter beg, Iter end, Comper comp);
```
1. Sorts the element of a container.
2. Sorts the element of a container using a custom comper algorithm.
See: [Merge Sort](https://en.wikipedia.org/wiki/Merge_sort)

__Parameters__:
- Iter beg: Iterator to the begining of the container.
- Iter end: Iterator to the end of the container.

__Return Value__:
    There is no return value.

### StringToNumber
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

### NumberSize
```cpp
template <class DataType>
uint32_t NumberSize(DataType Number);
```
Return the size of the number.

__Parameters__:
- DataType Number: The number.

__Return Value__:
    Return the size of the number.
    
### SimplePow
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
    
### PrimeCheck
```cpp
template <class DataType>
bool PrimeCheck(DataType prime);
```
Check if the number is a prime.

__Parameters__:
- DataType prime: The number to check.

__Return Value__:
    Returns true if it's a prime, otherwise false.

### DeleteDigit
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

### Map
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
            static Matrix<DataType> ZeroMatrix(uint32_t row, uint32_t col);
            static Matrix<DataType> ZeroMatrix(uint32_t number);
            static Matrix<DataType> IdentityMatrix(uint32_t size);
            Matrix(const uint32_t row, const uint32_t col, DataType fill);
            Matrix(const uint32_t row, const uint32_t col);
            Matrix();
            Matrix(const Matrix &other);
            void Fill(DataType data);
            void Resize(const uint32_t row, const uint32_t col);
            void Clean();
            DataType GetData(uint32_t row, uint32_t col) const;
            DataType GetDeterminant() const;
            DataType GetSubDeterminant(uint32_t row, uint32_t col) const;
            uint32_t GetRow() const;
            uint32_t GetColumn() const;
            std::vector<DataType> &operator[](size_t position);
            std::vector<DataType> operator[](size_t position) const;
            Matrix<DataType> operator+(const Matrix<DataType> other);
            void operator+=(const Matrix<DataType> other);
            void operator=(const Matrix<DataType> &other);
            Matrix<DataType> operator*(const Matrix<DataType> other);
            Matrix<DataType> operator*(const DataType Data);
            void operator*=(const Matrix<DataType> other);
            void operator*=(const DataType Data);
            Matrix<DataType> operator/(Matrix<DataType> other);
            Matrix<DataType> operator/(const DataType Data);
            void operator/=(const Matrix<DataType> other);
            void operator/=(const DataType Data);
            Matrix<DataType> GetInverse();
            explicit operator std::vector<std::vector<DataType>>();
            ~Matrix();
    };
```

It is a container, a representation of a mathematic matrix.
- DataType: The types of data to store.

See: [Matrix](https://en.wikipedia.org/wiki/Matrix_(mathematics))

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

### Operator+=
```cpp
void operator+=(const Matrix<DataType> other);
```

It adds the Matrices and assigns the values to the first Matrix.
See: [Matrix Addition](https://en.wikipedia.org/wiki/Matrix_addition)

__Paramters__:
- const Matrix<DataType> other: The Matrix to add to.
    
__Return Value__:
    There is no return value.

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
Matrix<DataType> operator*(const DataType Data);
```

1. It multiples the Matrices.
See: [Matrix Multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication)
2. It multiples the Matrix with a constant.

__Paramters__:
- const Matrix<DataType> other: The Matrix to multiple with.
- const DataType Data: The data to multiple with.
    
__Return Value__:
    It returns the newly created Matrix.

### Operator*=
```cpp
void operator*=(const Matrix<DataType> other);
void operator*=(const DataType Data);
```

1. It multiples the Matrices and assigns the values to the first Matrix.
See: [Matrix Multiplication](https://en.wikipedia.org/wiki/Matrix_multiplication)
2. It multiples the Matrix with a constant and assigns the values to the Matrix.

__Paramters__:
- const Matrix<DataType> other: The Matrix to multiple with.
- const DataType Data: The data to multiple with.
    
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
    
### GetDeterminant
```cpp
DataType GetDeterminant() const;
```

It calculates the Matrix determinant.
See: [Determinant](https://en.wikipedia.org/wiki/Determinant)

__Paramters__:
    There is no parameter.
    
__Return Value__:
    Returns the calculated determinant.

### GetSubDeterminant
```cpp
DataType GetSubDeterminant(uint32_t row, uint32_t col) const;
```

### Cast Operator
```cpp
operator std::vector<std::vector<DataType>>();
```

It makes a 2d std::vector out of the matrix.

__Paramters__:
    There is no parameter.
    
__Return Value__:
    Returns the 2d std::vector.


It gives the minor of the matrix.
See: [Minor](https://en.wikipedia.org/wiki/Minor_(linear_algebra))

__Paramters__:
- uint32_t row: The row coordinate.
- uint32_t col: The column coordinate.
    
__Return Value__:
    Returns the minor.

### GetInverse
```cpp
Matrix<DataType> GetInverse();
```
It creates the Inverse of the Matrix.
See: [Invertable Matrix](https://en.wikipedia.org/wiki/Invertible_matrix)

__Parameters__:
    There is no parameter.
    
__Return Value__:
    Returns the newly created Matrix.

### Operator/
```cpp
Matrix<DataType> operator/(Matrix<DataType> other);
Matrix<DataType> operator/(const DataType Data);
```

1. It divedes the Matrices.
See: [Invertible Matrix](https://en.wikipedia.org/wiki/Invertible_matrix)
2. It divedes the Matrix with a constant.

__Paramters__:
- const Matrix<DataType> other: The Matrix to multiple with.
- const DataType Data: The data to multiple with.
    
__Return Value__:
    It returns the newly created Matrix.

### Operator/=
```cpp
void operator/=(const Matrix<DataType> other);
void operator/=(const DataType Data);
```

1. It divedes the Matrices and assigns the values to the first Matrix.
See: [Invertible Matrix](https://en.wikipedia.org/wiki/Invertible_matrix)
2. It divedes the Matrix with a constant and assigns the values to the first Matrix.

__Paramters__:
- const Matrix<DataType> other: The Matrix to multiple with.
- const DataType Data: The data to multiple with.
    
__Return Value__:
    There is no return value.

## Graph 

```cpp
    template <typename VertexData, typename EdgeData>
    class Graph : public UniqueGraph<uint32_t, VertexData, EdgeData>
    {
    private:
        uint32_t IDcount = 0;

    public:
        void AddVertex(VertexData Data);
        Graph(uint32_t NumberOfVertices, std::vector<VertexData> VertexDatas);
        Graph(uint32_t NumberOfVertices);
        Graph();
    };
```

It is a representation of a graph, where the vertices are represented by numbers. This class inherits from the UniqueGraph.
- VertexData: The type of data to store at the graph's vertex.
- EdgeData: The type of data to store at the edge between two vertices.

See: [Graph](https://en.wikipedia.org/wiki/Graph_(discrete_mathematics))

### **Functions**:

### Constructors
```cpp
Graph(uint32_t NumberOfVertices, std::vector<VertexData> VertexDatas);
Graph(uint32_t NumberOfVertices);
Graph();
```
1. It creates the graph and assigns the data to vertices.
2. It creates the graph.
3. It creates an empty graph.
    
__Parameters__:
- uint32_t NumberOfVertices: the number of vertices.
- std::vector<VertexData> VertexDatas: the data to assign to the vertices.

__Return Value__:
    There is no return value.

### AddVertex

```cpp
void AddVertex(VertexData Data);
```
  
__Parameters__:
- uint32_t NumberOfVertices: the number of vertices.
- std::vector<VertexData> VertexDatas: the data to assign to the vertices.

__Return Value__:
    There is no return value.

## UniqueGraph

```cpp
    template <typename VertexType, typename VertexData, typename EdgeData>
    class UniqueGraph
    {
    protected:
        void TopologicalSortUtil(VertexType Current, std::vector<VertexType> &Visited, std::stack<VertexType> &TheTopologicalStack);
        std::map<VertexType, VertexData> Vertices;
        std::map<VertexType, std::map<VertexType, EdgeData>> Edges;

    public:
        UniqueGraph(std::vector<VertexType> Vertecies, std::vector<VertexData> VertexDatas);
        UniqueGraph(std::vector<VertexType> Vertecies);
        UniqueGraph();
        std::vector<VertexType> GetVertices();
        void AddVertex(VertexType Vertex, VertexData Data);
        void AddEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data);
        void AddBidirectionalEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data);
        VertexData GetVertexData(VertexType Vertex);
        EdgeData GetEdgeData(VertexType VertexFrom, VertexType VertexTo);
        void DeleteVertex(VertexType Vertex);
        void DeleteEdge(VertexType VertexFrom, VertexType VertexTo);
        void Dijkstra(VertexType Source, std::vector<EdgeData> &Distance, std::vector<VertexType> &Previous);
        template <typename Compare>
        void Dijkstra(VertexType Source, std::vector<EdgeData> &Distance, std::vector<VertexType> &Previous, Compare CompAlg);
        void BreadthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices);
        std::vector<VertexType> BreadthFirstSearch(VertexType Source);
        template <typename Function>
        void BreadthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices, Function TheFunction);
        template <typename Function>
        void DepthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices, Function TheFunction);
        void DepthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices);
        std::vector<VertexType> DepthFirstSearch(VertexType Source);
        void GetTopologicalSort(std::vector<VertexType> &Vertices);
        std::vector<VertexType> GetTopologicalSort();
        UniqueGraph<VertexType, VertexData, EdgeData> PrimSpanningTreeGraph();
        std::map<VertexType, VertexType> PrimSpanningTreeParent();
    };
```
It is a representation of a graph.
- VertexType: The type of vertices's indexes.
- VertexData: The type of data to store at the graph's vertex.
- EdgeData: The type of data to store at the edge between two vertices.

### **Functions**:

### Constructors
```cpp
UniqueGraph(std::vector<VertexType> Vertecies, std::vector<VertexData> VertexDatas);
UniqueGraph(std::vector<VertexType> Vertecies);
UniqueGraph();
```
1. It creates the graph and assigns the data to vertices.
2. It creates the graph.
3. It creates an empty graph.
    
__Parameters__:
- std::vector<VertexType> Vertecies: the indexes of vertices.
- std::vector<VertexData> VertexDatas: the data to assign to the vertices.

__Return Value__:
    There is no return value.

### GetVerticess
```cpp
std::vector<VertexType> GetVertices();
```
It returns the vertex indexes in a vector.
    
__Parameters__:
    There is no parameter.

__Return Value__:
    Returns the indexes.

### AddVertex
```cpp
void AddVertex(VertexType Vertex, VertexData Data);
```
It adds the vertex to the graph with the data given.
    
__Parameters__:
- VertexType Vertex: The index of the vertex to add.
- VertexData Data: The data to store in that index.

__Return Value__:
    There is no return value.

### AddEdge
```cpp
void AddEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data);
```
It adds an edge to the graph with the data given.
    
__Parameters__:
- VertexType VertexFrom: The index of the starting vertex.
- VertexType VertexTo: The index of the end vertex.
- EdgeData Data: The data to store.

__Return Value__:
    There is no return value.

__Return Value__:
    There is no return value.

### AddBidirectionalEdge
```cpp
void AddBidirectionalEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data); 
```
It adds a bidirectional edge to the graph with the data given.
    
__Parameters__:
- VertexType VertexFrom: The index of the starting vertex.
- VertexType VertexTo: The index of the end vertex.
- EdgeData Data: The data to store.

__Return Value__:
    There is no return value.

### GetVertexData
```cpp
VertexData GetVertexData(VertexType Vertex);
```
It returns the data at the specified vertex.
    
__Parameters__:
- VertexType Vertex: The index of the vertex.

__Return Value__:
    Returns the data stored.
