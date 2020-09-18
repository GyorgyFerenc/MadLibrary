#ifndef SimpleFuntions__HPP
#define SimpleFuntions__HPP

#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <bitset>
#include <map>

#if defined(_GLIBCXX_LIST) || defined(_LIST_)
#define ListIncluded true
#else
#define ListIncluded false
#endif

#if defined(_GLIBCXX_ARRAY) || defined(_ARRAY_)
#define ArrayIncluded true
#else
#define ArrayIncluded false
#endif

#if defined(_GLIBCXX_FORWARD_LIST) || defined(_FORWARD_LIST_)
#define ForwardListIncluded true
#else
#define ForwardListIncluded false
#endif

namespace MadLibrary
{
    ///INIT
    //AppendVectors //Documented
    template <typename DataType>
    std::vector<DataType> AppendVectors(std::vector<DataType> &First, const std::vector<DataType> &Second);

    //StringParser
    std::vector<std::string> StringParser(std::string TheString, bool CheckForWhiteSpaces, std::vector<char> OtherCharToCheckFor);
    std::vector<std::string> StringParser(std::string TheString);

    //toCleverString
    template <typename DataType>
    std::string toCleverString(DataType data);
    template <typename DataType>
    std::string toCleverString(DataType data, uint32_t flag);
    std::string toCleverString(const std::string data);
    std::string toCleverString(char data);
    std::string toCleverString(const char *data);
    std::string toCleverString(char *data);
    template <typename DataType1, typename DataType2>
    std::string toCleverString(std::pair<DataType1, DataType2> thePair);
    template <typename DataType1, typename DataType2>
    std::string toCleverString(std::map<DataType1, DataType2> theMap);
    template <typename DataType>
    std::string toCleverString(std::vector<DataType> vect);
    template <typename DataType>
    std::string toCleverString(std::initializer_list<DataType> initList);
#if ListIncluded
    template <typename DataType>
    std::string toCleverString(std::list<DataType> TheList);
#endif
#if ArrayIncluded
    template <typename DataType, std::size_t Size>
    std::string toCleverString(std::array<DataType, Size> TheArray);
#endif
#if ForwardListIncluded
    template <typename DataType>
    std::string toCleverString(std::forward_list<DataType> TheForwardList);
#endif

    //Abs //Documented
    template <class DataType>
    DataType Abs(DataType data);

    //PolygonArea //Documented
    template <class DataType, class DataType2>
    void PolygonArea(std::vector<DataType> X, std::vector<DataType> Y, DataType2 &temp);

    //MergeSort //Documented
    template <class Iter, class Comper>
    void MergeSort(Iter beg, Iter end, Comper comp);
    template <class Iter>
    void MergeSort(Iter beg, Iter end);

    //StringToNumber //Documented
    template <class DataType, class RandomAccesIterator>
    void StringToNumber(RandomAccesIterator begin, RandomAccesIterator end, DataType &toData);

    //NumberSize //Documented
    template <class DataType>
    uint32_t NumberSize(DataType Number);

    //SimplePow //Documented
    template <class DataType>
    DataType SimplePow(DataType Number, uint32_t exp);

    //PrimeCheck //Documented
    template <class DataType>
    bool PrimeCheck(DataType prime);

    //DeleteDigit //Documented
    template <class DataType>
    DataType DeleteDigit(DataType Number, uint32_t pos);

    //Map //Documented
    double Map(double value, double start1, double stop1, double start2, double stop2);

    ///Exposition

    //AppendVectors
    template <typename DataType>
    std::vector<DataType> AppendVectors(std::vector<DataType> &First, const std::vector<DataType> &Second)
    {
        First.insert(First.end(), Second.begin(), Second.end());
        return First;
    }

    //StringParser
    std::vector<std::string> StringParser(std::string TheString, bool CheckForWhiteSpaces, std::vector<char> OtherCharToCheckFor)
    {
        if (CheckForWhiteSpaces)
            AppendVectors(OtherCharToCheckFor, {0x20, 0x0c, 0x0a, 0x0d, 0x09, 0x0b});
        OtherCharToCheckFor.push_back('\0');
        std::vector<std::string> Parsed;
        uint32_t pos1 = 0;
        bool FirstSet = false;
        for (size_t i = 0; i < TheString.size() + 1; i++)
        {
            if (FirstSet)
            {
                if (std::find(OtherCharToCheckFor.begin(), OtherCharToCheckFor.end(), TheString[i]) != OtherCharToCheckFor.end())
                {
                    Parsed.push_back(TheString.substr(pos1, i - pos1));
                    FirstSet = false;
                }
            }
            else
            {
                if (std::find(OtherCharToCheckFor.begin(), OtherCharToCheckFor.end(), TheString[i]) == OtherCharToCheckFor.end())
                {
                    FirstSet = true;
                    pos1 = i;
                }
            }
        }

        return Parsed;
    }

    std::vector<std::string> StringParser(std::string TheString)
    {
        return StringParser(TheString, 1, {});
    }
    //toCleverString
    template <typename DataType>
    std::string toCleverString(DataType data)
    {
        std::stringstream SStream;
        SStream << data;
        std::string theString = SStream.str();
        return theString;
    }

    std::string toCleverString(const std::string data)
    {
        std::stringstream SStream;
        SStream << "\"" << data << "\"";
        std::string theString = SStream.str();
        return theString;
    }

    std::string toCleverString(char data)
    {
        std::stringstream SStream;
        SStream << "\'" << data << "\'";
        std::string theString = SStream.str();
        return theString;
    }

    std::string toCleverString(const char *data)
    {
        std::stringstream SStream;
        SStream << "\"" << data << "\"";
        std::string theString = SStream.str();
        return theString;
    }

    std::string toCleverString(char *data)
    {
        std::stringstream SStream;
        SStream << "\"" << data << "\"";
        std::string theString = SStream.str();
        return theString;
    }

    template <typename DataType1, typename DataType2>
    std::string toCleverString(std::pair<DataType1, DataType2> thePair)
    {
        std::stringstream SStream;
        SStream << toCleverString(thePair.first) << ":" << toCleverString(thePair.second);
        std::string theString = SStream.str();
        return theString;
    }

    template <typename DataType1, typename DataType2>
    std::string toCleverString(std::map<DataType1, DataType2> theMap)
    {
        std::stringstream SStream;
        SStream << "(" << toCleverString(*(theMap.begin()));
        for (auto it = ++theMap.begin(); it != theMap.end(); it++)
        {
            SStream << "," << toCleverString(*it);
        }
        SStream << ")";
        std::string theString = SStream.str();
        return theString;
    }

    template <typename DataType>
    std::string toCleverString(DataType data, uint32_t flag)
    {
        std::stringstream SStream;
        std::bitset<sizeof(data) * 8> b(data);
        switch (flag)
        {
        case 1:
            SStream << "0x" << std::hex << data;
            break;
        case 2:
            SStream << "0b" << b.to_string();
            break;
        case 3:
            SStream << "Octx" << std::oct << data;
            break;
        default:
            SStream << toCleverString(data);
            break;
        }
        std::string theString = SStream.str();
        return theString;
    }

    template <typename DataType>
    std::string toCleverString(std::vector<DataType> vect)
    {
        std::stringstream SStream;
        SStream << "(";
        for (uint32_t i = 0; i < vect.size() - 1; i++)
        {
            SStream << toCleverString(vect[i]) << ",";
        }
        SStream << toCleverString(vect[vect.size() - 1]);
        SStream << ")";
        std::string theString = SStream.str();
        return theString;
    }

    template <typename DataType>
    std::string toCleverString(std::initializer_list<DataType> initList)
    {
        std::stringstream SStream;
        auto iTinitList = initList.begin();
        SStream << "(" << toCleverString(*iTinitList);
        for (iTinitList = ++iTinitList; iTinitList != initList.end(); ++iTinitList)
        {
            SStream << "," << toCleverString(*iTinitList);
        }
        SStream << ")";
        std::string theString = SStream.str();
        return theString;
    }

#if ListIncluded
    template <typename DataType>
    std::string toCleverString(std::list<DataType> TheList)
    {
        std::stringstream SStream;
        auto itList = TheList.begin();
        SStream << "(";
        for (uint32_t i = 0; i < TheList.size() - 1; i++)
        {
            SStream << toCleverString(*itList) << ",";
            itList++;
        }
        SStream << toCleverString(*itList);
        SStream << ")";
        std::string theString = SStream.str();
        return theString;
    }
#endif

#if ArrayIncluded
    template <typename DataType, std::size_t Size>
    std::string toCleverString(std::array<DataType, Size> TheArray)
    {
        std::stringstream SStream;
        SStream << "(";
        for (uint32_t i = 0; i < TheArray.size() - 1; i++)
        {
            SStream << toCleverString(TheArray[i]) << ",";
        }
        SStream << toCleverString(TheArray[TheArray.size() - 1]);
        SStream << ")";
        std::string theString = SStream.str();
        return theString;
    }
#endif

#if ForwardListIncluded
    template <typename DataType>
    std::string toCleverString(std::forward_list<DataType> TheForwardList)
    {
        std::stringstream SStream;
        auto itForwardList = TheForwardList.begin();
        SStream << "(" << toCleverString(*itForwardList);
        for (itForwardList = ++TheForwardList.begin(); itForwardList != TheForwardList.end(); itForwardList++)
        {
            SStream << "," << toCleverString(*itForwardList);
        }
        SStream << ")";
        std::string theString = SStream.str();
        return theString;
    }
#endif

    //Map
    double Map(double value, double start1, double stop1, double start2, double stop2)
    {
        double outgoing = start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
        return outgoing;
    }

    //Abs
    template <class DataType>
    DataType Abs(DataType data)
    {
        if (data < 0)
            return -1 * data;
        return data;
    }

    //PolygonArea
    template <class DataType, class DataType2>
    void PolygonArea(std::vector<DataType> X, std::vector<DataType> Y, DataType2 &temp)
    {
        // Initialze area
        DataType2 area = 0.0;

        // Calculate value of shoelace formula
        int j = X.size() - 1;
        for (int i = 0; i < X.size(); i++)
        {
            area += (X[j] + X[i]) * (Y[j] - Y[i]);
            j = i; // j is previous vertex to i
        }

        // Return absolute value
        temp = Abs(area / 2.0);
    }

    //StringToNumber
    template <class DataType, class RandomAccesIterator>
    void StringToNumber(RandomAccesIterator begin, RandomAccesIterator end, DataType &toData)
    {
        std::stringstream sS;
        for (auto i = begin; i != end; i++)
        {
            sS << *(i);
        }
        sS >> toData;
    }

    //NumberSize
    template <class DataType>
    uint32_t NumberSize(DataType Number)
    {
        unsigned int size = 0;
        do
        {
            Number /= 10;
            size++;
        } while (Number != 0);
        return size;
    }

    //SimplePow
    template <class DataType>
    DataType SimplePow(DataType Number, uint32_t exp)
    {
        if (exp == 0)
            return (Number / Number);
        if (exp == 1)
            return Number;
        DataType temp = Number;
        uint32_t MomentExp = 1;
        std::vector<DataType> Numbers;   //0->2^1, 1->2^2
        std::vector<uint32_t> Exponents; //0->1,1->2
        while (MomentExp * 2 <= exp)
        {
            Number *= Number;
            MomentExp *= 2;
            Exponents.push_back(MomentExp);
            Numbers.push_back(Number);
            if (MomentExp >= std::numeric_limits<uint32_t>::max() / 2)
            {
                Numbers.pop_back();
                Exponents.pop_back();
                break;
            }
        }
        for (uint32_t i = 0; i < Numbers.size(); i++)
        {
            if (MomentExp + Exponents[Numbers.size() - i - 1] < exp)
            {
                Number *= Numbers[Numbers.size() - i - 1];
                MomentExp += Exponents[Numbers.size() - i - 1];
            }
        }
        while (MomentExp < exp)
        {
            Number *= temp;
            MomentExp++;
        }
        return Number;
    }

    //DeleteDigit
    template <class DataType>
    DataType DeleteDigit(DataType Number, uint32_t pos)
    {
        return Number / SimplePow(10, NumberSize(Number) - pos) * SimplePow(10, NumberSize(Number) - pos - 1) + Number % SimplePow(10, NumberSize(Number) - pos - 1);
    }

    //PrimeCheck
    template <class DataType>
    bool PrimeCheck(DataType prime)
    {
        if (prime <= 1)
            return false;
        if (prime == 2)
            return true;
        if (prime % 2 == 0)
            return false;
        for (DataType i = 3; i <= sqrt(prime); i += 2)
        {
            if (prime % i == 0)
                return false;
        }
        return true;
    }

    //MergeSort
    template <class Iter, class Comper>
    void MergeSort(Iter beg, Iter end, Comper comp)
    {
        int size = std::distance(beg, end);
        if (size <= 1)
        {
            return;
        }

        auto mid = std::next(beg, size / 2);
        MergeSort(beg, mid, comp);
        MergeSort(mid, end, comp);
        std::inplace_merge(beg, mid, end, comp);
    }

    template <class Iter>
    void MergeSort(Iter beg, Iter end)
    {
        int size = std::distance(beg, end);
        if (size <= 1)
        {
            return;
        }

        auto mid = std::next(beg, size / 2);
        MergeSort(beg, mid);
        MergeSort(mid, end);
        std::inplace_merge(beg, mid, end);
    }
} // namespace MadLibrary
#endif