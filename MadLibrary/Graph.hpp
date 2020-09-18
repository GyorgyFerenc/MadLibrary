#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

/*
TODO:
    Map->Unordered Map
    Document it
*/
namespace MadLibrary
{
    //INIT
    template <typename VertexType, typename VertexData, typename EdgeData>
    class UniqueGraph;
    template <typename VertexData, typename EdgeData>
    class Graph;

    //Exposition
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
    //AddVertex
    template <typename VertexData, typename EdgeData>
    void MadLibrary::Graph<VertexData, EdgeData>::AddVertex(VertexData Data)
    {
        this->Vertices[this->IDcount] = Data;
        this->IDcount++;
    }

    //Constructor
    template <typename VertexData, typename EdgeData>
    MadLibrary::Graph<VertexData, EdgeData>::Graph(uint32_t NumberOfVertices, std::vector<VertexData> VertexDatas)
    {
        for (uint32_t i = 0; i < NumberOfVertices; i++)
        {
            this->Vertices[IDcount] = VertexDatas[IDcount];
            IDcount++;
        }
    }

    template <typename VertexData, typename EdgeData>
    MadLibrary::Graph<VertexData, EdgeData>::Graph(uint32_t NumberOfVertices)
    {
        for (uint32_t i = 0; i < NumberOfVertices; i++)
        {
            this->Vertices[IDcount];
            IDcount++;
        }
    }

    template <typename VertexData, typename EdgeData>
    MadLibrary::Graph<VertexData, EdgeData>::Graph()
    {
        return;
    }

    ///UniqueGraph
    //GetVertices
    template <typename VertexType, typename VertexData, typename EdgeData>
    std::vector<VertexType> MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::GetVertices()
    {
        std::vector<VertexType> Vertices;
        for (auto it = this->Vertices.begin(); it != this->Vertices.end(); it++)
        {
            Vertices.push_back(it->first);
        }
        return Vertices;
    }

    //AddVertex
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::AddVertex(VertexType Vertex, VertexData Data)
    {
        this->Vertices[Vertex] = Data;
    }

    //AddEdge
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::AddEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data)
    {
        this->Edges[VertexFrom][VertexTo] = Data;
    }

    //AddBidirectionalEdge
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::AddBidirectionalEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data)
    {
        this->Edges[VertexFrom][VertexTo] = Data;
        this->Edges[VertexTo][VertexFrom] = Data;
    }

    //GetVertexData
    template <typename VertexType, typename VertexData, typename EdgeData>
    VertexData MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::GetVertexData(VertexType Vertex)
    {
        return this->Vertices[Vertex];
    }

    //GetEdgeData
    template <typename VertexType, typename VertexData, typename EdgeData>
    EdgeData MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::GetEdgeData(VertexType VertexFrom, VertexType VertexTo)
    {
        return this->Edges[VertexFrom][VertexTo];
    }

    //DeleteVertex
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::DeleteVertex(VertexType Vertex)
    {
        this->Vertices.erase(Vertex);
        this->Edges.erase(Vertex);
        for (auto it = Edges.begin(); it != Edges.end(); it++)
        {
            it->second.erase(Vertex);
        }
    }

    //DeleteEdge
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::DeleteEdge(VertexType VertexFrom, VertexType VertexTo)
    {
        this->Edges[VertexFrom].erase(VertexTo);
    }

    //Dijkstra
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::Dijkstra(VertexType Source, std::vector<EdgeData> &Distance, std::vector<VertexType> &Previous)
    {
        auto Compare = [](std::pair<VertexType, EdgeData> First, std::pair<VertexType, EdgeData> Second) -> bool {
            return First.second > Second.second;
        };
        this->Dijkstra(Source, Distance, Previous, Compare);
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    template <typename Compare>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::Dijkstra(VertexType Source, std::vector<EdgeData> &Distance, std::vector<VertexType> &Previous, Compare CompAlg)
    {
        Distance.clear();
        Previous.clear();

        for (uint32_t i = 0; i < this->Vertices.size(); i++)
        {
            Distance.push_back(std::numeric_limits<EdgeData>::max());
            Previous.push_back(std::numeric_limits<VertexType>::max());
        }

        Distance[Source] = 0;

        std::priority_queue<std::pair<VertexType, EdgeData>, std::vector<std::pair<VertexType, EdgeData>>, decltype(CompAlg)> PriorityQueue(CompAlg);

        PriorityQueue.push(std::pair<VertexType, EdgeData>(Source, 0));

        while (!PriorityQueue.empty())
        {
            VertexType Current = PriorityQueue.top().first;
            PriorityQueue.pop();

            for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
            {
                VertexType TheVertex = it->first;
                EdgeData TheEdgeData = it->second;

                if (Distance[TheVertex] > Distance[Current] + TheEdgeData)
                {
                    Distance[TheVertex] = Distance[Current] + TheEdgeData;
                    Previous[TheVertex] = Current;
                    PriorityQueue.push(std::pair<VertexType, EdgeData>(TheVertex, Distance[TheVertex]));
                }
            }
        }
    }

    //BreadthFirstSearch
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::BreadthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices)
    {
        Vertices.clear();

        std::queue<VertexType> TheQueue;

        std::vector<VertexType> Visited;

        TheQueue.push(Source);
        while (!TheQueue.empty())
        {
            VertexType Current = TheQueue.front();
            Vertices.push_back(Current);
            Visited.push_back(Current);
            TheQueue.pop();

            for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
            {
                if (std::find(Visited.begin(), Visited.end(), it->first) != Visited.end())
                    continue;
                TheQueue.push(it->first);
            }
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    template <typename Function>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::BreadthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices, Function TheFunction)
    {
        Vertices.clear();

        std::queue<VertexType> TheQueue;

        std::vector<VertexType> Visited;

        TheQueue.push(Source);
        while (!TheQueue.empty())
        {
            VertexType Current = TheQueue.front();
            Vertices.push_back(Current);
            Visited.push_back(Current);
            TheQueue.pop();

            TheFunction(this, Current);

            for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
            {
                if (std::find(Visited.begin(), Visited.end(), it->first) != Visited.end())
                    continue;
                TheQueue.push(it->first);
            }
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    std::vector<VertexType> MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::BreadthFirstSearch(VertexType Source)
    {
        std::vector<VertexType> Vertices;
        this->BreadthFirstSearch(Source, Vertices);
        return Vertices;
    }

    //DepthFirstSearch
    template <typename VertexType, typename VertexData, typename EdgeData>
    template <typename Function>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::DepthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices, Function TheFunction)
    {
        Vertices.clear();

        std::stack<VertexType> TheStack;

        std::vector<VertexType> Visited;

        TheStack.push(Source);
        while (!TheStack.empty())
        {
            VertexType Current = TheStack.top();
            TheStack.pop();
            Vertices.push_back(Current);
            Visited.push_back(Current);
            TheFunction(this, Current);

            for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
            {
                if (std::find(Visited.begin(), Visited.end(), it->first) != Visited.end())
                    continue;
                TheStack.push(it->first);
            }
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::DepthFirstSearch(VertexType Source, std::vector<VertexType> &Vertices)
    {
        Vertices.clear();

        std::stack<VertexType> TheStack;

        std::vector<VertexType> Visited;

        TheStack.push(Source);
        while (!TheStack.empty())
        {
            VertexType Current = TheStack.top();
            TheStack.pop();
            Vertices.push_back(Current);
            Visited.push_back(Current);

            for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
            {
                if (std::find(Visited.begin(), Visited.end(), it->first) != Visited.end())
                    continue;
                TheStack.push(it->first);
            }
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    std::vector<VertexType> MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::DepthFirstSearch(VertexType Source)
    {
        std::vector<VertexType> DFS;
        this->DepthFirstSearch(Source, DFS);
        return DFS;
    }

    //Constructor
    template <typename VertexType, typename VertexData, typename EdgeData>
    MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::UniqueGraph(std::vector<VertexType> Vertecies, std::vector<VertexData> VertexDatas)
    {
        for (uint32_t i = 0; i < Vertices.size(); i++)
        {
            this->Vertices[Vertices[i]] = VertexDatas[i];
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::UniqueGraph(std::vector<VertexType> Vertecies)
    {
        for (uint32_t i = 0; i < Vertices.size(); i++)
        {
            this->Vertices[Vertices[i]];
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::UniqueGraph()
    {
        return;
    }

    //GetTopologicalSort
    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::TopologicalSortUtil(VertexType Current, std::vector<VertexType> &Visited, std::stack<VertexType> &TheTopologicalStack)
    {
        for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
        {
            if (std::find(Visited.begin(), Visited.end(), it->first) != Visited.end())
                continue;
            Visited.push_back(it->first);
            this->TopologicalSortUtil(it->first, Visited, TheTopologicalStack);
        }
        TheTopologicalStack.push(Current);
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    void MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::GetTopologicalSort(std::vector<VertexType> &Vertices)
    {
        Vertices.clear();

        std::stack<VertexType> TheStack, TheTopologicalStack;

        std::vector<VertexType> Visited;

        for (auto Element = this->Vertices.begin(); Element != this->Vertices.end(); Element++)
        {

            VertexType Source = Element->first;
            if (std::find(Visited.begin(), Visited.end(), Source) == Visited.end())
            {
                this->TopologicalSortUtil(Source, Visited, TheTopologicalStack);
            }
        }

        while (!TheTopologicalStack.empty())
        {
            Vertices.push_back(TheTopologicalStack.top());
            TheTopologicalStack.pop();
        }
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    std::vector<VertexType> MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::GetTopologicalSort()
    {
        std::vector<VertexType> Vertices;
        this->GetTopologicalSort(Vertices);
        return Vertices;
    }

    //SpanningTree
    template <typename VertexType, typename VertexData, typename EdgeData>
    MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData> MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::PrimSpanningTreeGraph()
    {
        auto Parents = this->PrimSpanningTreeParent();
        MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData> newGraph;
        auto Vertecies = this->GetVertices();
        for (uint32_t i = 0; i < Vertecies.size(); i++)
        {
            newGraph.AddVertex(Vertecies[i], this->GetVertexData(Vertecies[i]));
        }

        for (auto i = Parents.begin(); i != Parents.end(); i++)
        {
            newGraph.AddEdge(i->second, i->first, this->GetEdgeData(i->second, i->first));
        }
        return newGraph;
    }

    template <typename VertexType, typename VertexData, typename EdgeData>
    std::map<VertexType, VertexType> MadLibrary::UniqueGraph<VertexType, VertexData, EdgeData>::PrimSpanningTreeParent()
    {
        std::map<VertexType, VertexType> Parents;

        std::map<VertexType, EdgeData> Keys;
        std::map<VertexType, bool> Visited;
        std::vector<VertexType> TheVertecies = this->GetVertices();

        auto GetMin = [&Keys, &TheVertecies, &Visited, this]() -> VertexType {
            EdgeData Min;
            VertexType MinIndex;
            uint32_t g = 0;
            for (g = 0; g < TheVertecies.size(); g++)
            {
                if (!Visited[TheVertecies[g]])
                {
                    Min = Keys[TheVertecies[g]];
                    MinIndex = TheVertecies[g];
                    break;
                }
            }

            for (uint32_t i = g; i < TheVertecies.size(); i++)
            {
                if (Keys[TheVertecies[i]] < Min && !Visited[TheVertecies[i]])
                {
                    Min = Keys[TheVertecies[i]];
                    MinIndex = TheVertecies[i];
                }
            }
            return MinIndex;
        };

        //std::priority_queue<VertexType, std::vector<VertexType>,decltype(CompAlg)> PriorityQueue(CompAlg);

        for (uint32_t i = 0; i < TheVertecies.size(); i++)
        {
            Keys[TheVertecies[i]] = std::numeric_limits<EdgeData>::max();
            //PriorityQueue.push(TheVertecies[i]);
        }

        VertexType FristVertex = TheVertecies[0];
        Keys[FristVertex] = 0;
        for (uint32_t i = 0; i < TheVertecies.size() - 1; i++)
        {
            VertexType Current = GetMin();
            Visited[Current] = true;

            for (auto it = this->Edges[Current].begin(); it != this->Edges[Current].end(); it++)
            {
                VertexType TheVertex = it->first;
                EdgeData TheEdgeData = it->second;

                if (!Visited[TheVertex] && TheEdgeData < Keys[TheVertex])
                {
                    Parents[TheVertex] = Current;
                    Keys[TheVertex] = TheEdgeData;
                }
            }
        }
        return Parents;
    }

} // namespace MadLibrary
