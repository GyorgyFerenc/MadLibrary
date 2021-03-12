#ifndef Matrix__HPP
#define Matrix__HPP

#include <vector>
#include "SimpleFunctions.hpp"

//TODO: document operator<<

namespace MadLibrary
{
    ///INIT
    template <class DataType>
    class Matrix;

    ///Exposition
    template <class DataType>
    class Matrix
    {
    protected:
        std::vector<std::vector<DataType>> vect;
        uint32_t row, col;

    public:
        //Statics
        static Matrix<DataType> ZeroMatrix(uint32_t row, uint32_t col); //Documented
        static Matrix<DataType> ZeroMatrix(uint32_t number);            //Documented
        static Matrix<DataType> IdentityMatrix(uint32_t size);          //Documented

        //Constructors
        Matrix(const uint32_t row, const uint32_t col, DataType fill); //Documented
        Matrix(const uint32_t row, const uint32_t col);                //Documented
        Matrix();                                                      //Documented
        Matrix(const Matrix &other);                                   //Documented

        void Fill(DataType data);                                     //Documented
        void Resize(const uint32_t row, const uint32_t col);          //Documented
        void Clean();                                                 //Documented
        DataType GetData(uint32_t row, uint32_t col) const;           //Documented
        DataType GetDeterminant() const;                              //Documented
        DataType GetSubDeterminant(uint32_t row, uint32_t col) const; //Documented
        uint32_t GetRow() const;                                      //Documented
        uint32_t GetColumn() const;                                   //Documented
        std::vector<DataType> &operator[](size_t position);           //Documented
        std::vector<DataType> operator[](size_t position) const;      //Documented
        Matrix<DataType> operator+(const Matrix<DataType> other);     //Documented
        void operator+=(const Matrix<DataType> other);                //Documented
        void operator=(const Matrix<DataType> &other);                //Documented
        Matrix<DataType> operator*(const Matrix<DataType> other);     //Documented
        Matrix<DataType> operator*(const DataType Data);              //Documented
        void operator*=(const Matrix<DataType> other);                //Documented
        void operator*=(const DataType Data);                         //Documented
        Matrix<DataType> operator/(Matrix<DataType> other);           //Documented
        Matrix<DataType> operator/(const DataType Data);              //Documented
        void operator/=(const Matrix<DataType> other);                //Documented
        void operator/=(const DataType Data);                         //Documented
        Matrix<DataType> GetInverse();                                //Documented
        explicit operator std::vector<std::vector<DataType>>();       //Documented
        ~Matrix();                                                    //Documented
    };

    //operator<<
    template <class DataType>
    std::ostream &operator<<(std::ostream &Os, const Matrix<DataType> &TheMatrix) //Documented
    {
        for (uint32_t i = 0; i < TheMatrix.GetRow(); i++)
        {
            for (uint32_t j = 0; j < TheMatrix.GetColumn(); j++)
            {
                Os << TheMatrix.GetData(i, j) << " ";
            }
            if (i < TheMatrix.GetRow() - 1)
                Os << "\n";
        }
        return Os;
    }

    //GetRow
    template <class DataType>
    uint32_t Matrix<DataType>::GetRow() const
    {
        return this->row;
    }

    //GetColumn
    template <class DataType>
    uint32_t Matrix<DataType>::GetColumn() const
    {
        return this->col;
    }

    //ZeroMatrix
    template <class DataType>
    Matrix<DataType> Matrix<DataType>::ZeroMatrix(uint32_t row, uint32_t col)
    {
        Matrix<DataType> temp(row, col, 0);
        return temp;
    }

    template <class DataType>
    Matrix<DataType> Matrix<DataType>::ZeroMatrix(uint32_t number)
    {
        return Matrix<DataType>::ZeroMatrix(number, number);
    }

    //ZeroMatrix
    template <class DataType>
    Matrix<DataType> Matrix<DataType>::IdentityMatrix(uint32_t size)
    {
        Matrix<DataType> temp(size, size, 0);
        for (uint32_t i = 0; i < size; i++)
        {
            temp[i][i] = 1;
        }
        return temp;
    }

    //operator=
    template <class DataType>
    void Matrix<DataType>::operator=(const Matrix &other)
    {
        this->Resize(other.GetRow(), other.GetColumn());
        for (uint32_t i = 0; i < other.GetRow(); i++)
        {
            vect[i] = other[i];
        }
    }

    //operator+
    template <class DataType>
    Matrix<DataType> Matrix<DataType>::operator+(Matrix<DataType> other)
    {
        Matrix<DataType> temp(this->GetRow(), this->GetColumn());
        for (size_t i = 0; i < this->GetRow(); i++)
        {
            for (size_t j = 0; j < this->GetColumn(); j++)
            {
                temp[i][j] = this->GetData(i, j) + other.GetData(i, j);
            }
        }
        return temp;
    }

    //operator+=
    template <class DataType>
    void Matrix<DataType>::operator+=(const Matrix<DataType> other)
    {
        this->operator=(this->operator+(other));
    }

    //operator*
    template <class DataType>
    Matrix<DataType> Matrix<DataType>::operator*(const Matrix<DataType> other)
    {
        Matrix<DataType> temp(this->GetRow(), other.GetColumn());
        for (size_t i = 0; i < temp.GetRow(); i++)
        {
            for (size_t j = 0; j < temp.GetColumn(); j++)
            {
                DataType tempData = this->GetData(0, 0) - this->GetData(0, 0);
                for (size_t z = 0; z < other.GetRow(); z++)
                {
                    tempData += this->GetData(i, z) * other.GetData(z, j);
                }
                temp[i][j] = tempData;
            }
        }
        return temp;
    }

    template <class DataType>
    Matrix<DataType> Matrix<DataType>::operator*(const DataType Data)
    {
        Matrix<DataType> temp(this->GetRow(), this->GetRow());
        for (uint32_t i = 0; i < this->GetRow(); i++)
        {
            for (uint32_t j = 0; j < this->GetColumn(); j++)
            {
                temp[i][j] = this->GetData(i, j) * Data;
            }
        }
        return temp;
    }

    //operator*=
    template <class DataType>
    void Matrix<DataType>::operator*=(const Matrix<DataType> other)
    {
        this->operator=(this->operator*(other));
    }

    template <class DataType>
    void Matrix<DataType>::operator*=(const DataType Data)
    {
        this->operator=(this->operator*(Data));
    }

    //operator/
    template <class DataType>
    Matrix<DataType> Matrix<DataType>::operator/(Matrix<DataType> other)
    {
        return this->operator*(other.GetInverse());
    }

    template <class DataType>
    Matrix<DataType> Matrix<DataType>::operator/(const DataType Data)
    {
        Matrix<DataType> temp(this->GetRow(), this->GetRow());
        for (uint32_t i = 0; i < this->GetRow(); i++)
        {
            for (uint32_t j = 0; j < this->GetColumn(); j++)
            {
                temp[i][j] = this->GetData(i, j) / Data;
            }
        }
        return temp;
    }

    //operator/=
    template <class DataType>
    void Matrix<DataType>::operator/=(const Matrix<DataType> other)
    {
        this->operator=(this->operator/(other));
    }

    template <class DataType>
    void Matrix<DataType>::operator/=(const DataType Data)
    {
        this->operator=(this->operator/(Data));
    }

    //GetData
    template <class DataType>
    DataType Matrix<DataType>::GetData(uint32_t row, uint32_t col) const
    {
        return this->vect[row][col];
    }

    //GetDeterminant
    template <class DataType>
    DataType Matrix<DataType>::GetDeterminant() const
    {
        DataType Det = 0;
        //Check for a row with zeros
        for (size_t i = 0; i < this->GetRow(); i++)
        {
            bool a = true;
            for (size_t j = 0; j < this->GetColumn(); j++)
            {
                if (this->GetData(i, j) != 0)
                {
                    a = false;
                    break;
                }
            }
            if (a)
            {
                return 0;
            }
        }
        //Check for identical rows
        for (size_t i = 0; i < this->GetRow() - 1; i++)
        {
            bool a = true;
            for (size_t j = 0; j < this->GetColumn(); j++)
            {
                if (this->GetData(i, j) != this->GetData(i + 1, j))
                {
                    a = false;
                    break;
                }
            }
            if (a)
            {
                return 0;
            }
        }
        //Check for a column with zeros
        for (size_t i = 0; i < this->GetColumn(); i++)
        {
            bool a = true;
            for (size_t j = 0; j < this->GetRow(); j++)
            {
                if (this->GetData(j, i) != 0)
                {
                    a = false;
                    break;
                }
            }
            if (a)
            {
                return 0;
            }
        }
        //Check for identical columns
        for (size_t i = 0; i < this->GetColumn() - 1; i++)
        {
            bool a = true;
            for (size_t j = 0; j < this->GetRow(); j++)
            {
                if (this->GetData(j, i) != this->GetData(j, i + 1))
                {
                    a = false;
                    break;
                }
            }
            if (a)
            {
                return 0;
            }
        }
        if (this->GetRow() == 2)
            return this->GetData(0, 0) * this->GetData(1, 1) - this->GetData(1, 0) * this->GetData(0, 1);
        if (this->GetRow() == 1)
            return this->GetData(0, 0);
        for (size_t i = 0; i < this->GetColumn(); i++)
        {
            Det += SimplePow(-1, i) * this->GetData(0, i) * this->GetSubDeterminant(0, i);
        }
        return Det;
    }

    //GetSubDeterminant
    template <class DataType>
    DataType Matrix<DataType>::GetSubDeterminant(uint32_t row, uint32_t col) const
    {
        Matrix<DataType> newMatrix(this->GetRow() - 1, this->GetColumn() - 1, 0);
        size_t t = 0;

        for (size_t i = 0; i < newMatrix.GetRow(); i++)
        {
            size_t z = 0;
            for (size_t j = 0; j < newMatrix.GetColumn(); j++)
            {
                if (row == i)
                {
                    t = 1;
                }
                if (col == j)
                {
                    z = 1;
                }
                newMatrix[i][j] = this->GetData(i + t, j + z);
            }
        }
        return newMatrix.GetDeterminant();
    }

    //GetInverse
    template <class DataType>
    Matrix<DataType> Matrix<DataType>::GetInverse()
    {
        Matrix<DataType> newMatrix(this->GetRow(), this->GetColumn());

        for (uint32_t i = 0; i < newMatrix.GetRow(); i++)
        {
            for (uint32_t j = 0; j < newMatrix.GetColumn(); j++)
            {
                newMatrix[j][i] = SimplePow(-1, i + j) * this->GetSubDeterminant(i, j);
            }
        }
        return (newMatrix * (1 / this->GetDeterminant()));
    }

    //operator[]
    template <class DataType>
    std::vector<DataType> &Matrix<DataType>::operator[](size_t position)
    {
        return vect[position];
    }

    template <class DataType>
    std::vector<DataType> Matrix<DataType>::operator[](size_t position) const
    {
        return vect[position];
    }

    //cast operator
    template <class DataType>
    Matrix<DataType>::operator std::vector<std::vector<DataType>>()
    {
        return this->vect;
    }

    //Resize
    template <class DataType>
    void Matrix<DataType>::Resize(const uint32_t row, const uint32_t col)
    {
        this->row = row;
        this->col = col;
        this->Clean();
        vect.resize(row);
        for (size_t i = 0; i < vect.size(); i++)
        {
            std::vector<DataType> v(col);
            vect[i] = v;
        }
    }

    //Clean
    template <class DataType>
    void Matrix<DataType>::Matrix::Clean()
    {
        this->vect.clear();
    }

    //Fill
    template <class DataType>
    void Matrix<DataType>::Fill(DataType data)
    {
        for (uint32_t i = 0; i < this->row; i++)
        {
            for (uint32_t j = 0; j < this->col; j++)
            {
                this->vect[i][j] = data;
            }
        }
    }

    //Constructor
    template <class DataType>
    Matrix<DataType>::Matrix(uint32_t row, uint32_t col, DataType data)
    {
        this->Resize(row, col);
        this->Fill(data);
    }

    template <class DataType>
    Matrix<DataType>::Matrix(const Matrix &other)
    {
        this->operator=(other);
    }

    template <class DataType>
    Matrix<DataType>::Matrix(uint32_t row, uint32_t col)
    {
        this->Resize(row, col);
    }

    template <class DataType>
    Matrix<DataType>::Matrix()
    {
        this->Resize(0, 0);
    }

    //Destructor
    template <class DataType>
    Matrix<DataType>::~Matrix()
    {
        this->Clean();
    }
} // namespace MadLibrary

#endif