#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>
#include <stack>
#include <fstream>

struct Network
{
public:
    struct WayInfo
    {
        char to;
        int capacity;
        int flow;
    };

    struct VertexInfo
    {
        char prev = 0;
        bool is_used = false;
        std::vector<WayInfo> ways;
    };

private:
    char start = 0;
    char end = 0;
    size_t min_capacity = 0;

    std::map<char, VertexInfo> d;

public:
    Network() = default;
    Network(Network const& other)
    {
        start = other.start;
        end = other.end;
        d = other.d;
    }

    void addElem(char vertexName, WayInfo way)
    {
        d[vertexName].ways.push_back(way);
    }

    void setEnds(char start, char end)
    {
        this->start = start;
        this->end = end;
    }

    bool findWay(std::ostream& out)
    {
        out << "Wide search: \n";

        std::queue<char> q;
        q.push(start);

        while (!q.empty())
        {
            char cur = q.front();
            q.pop();

            out << "Cur vertex - " << cur << "\n";

            d[cur].is_used = true;

            for (auto& next : d[cur].ways)
            {
                if (d[next.to].is_used == true)
                    continue;

                if (next.capacity <= 0)
                    continue;

                if (next.to == end)
                {
                    d[next.to].prev = cur;
                    return true;
                }

                q.push(next.to);
                d[next.to].prev = cur;
            }
        }

        out << "Way not found\n";

        return false;
    }

    void findMin(std::ostream& out)
    {
        out << "Way - ";
        std::string way;

        char cur = end;
        while (cur != start)
        {
            char prev = d[cur].prev;
            for (auto& way : d[prev].ways)
            {
                if (way.to == cur)
                {
                    if (min_capacity == 0 || min_capacity > way.capacity)
                        min_capacity = way.capacity;
                    break;
                }
            }

            way.push_back(cur);
            cur = prev;
        }
        way.push_back(start);

        std::reverse(std::begin(way), std::end(way));
        out << way;
        out << "\nMin capacity - " << min_capacity << "\n\n";
    }

    void modifyFlow()
    {
        char cur = end;
        while (cur != start)
        {
            char prev = d[cur].prev;
            for (auto& way : d[prev].ways)
            {
                if (way.to == cur)
                {
                    way.flow += min_capacity;
                    break;
                }
            }

            for (auto& reverse_way : d[cur].ways)
            {
                if (reverse_way.to == prev)
                {
                    reverse_way.flow -= min_capacity;
                    break;
                }
            }

            cur = prev;
        }
    }

    void modifyCapacity()
    {
        char cur = end;
        while (cur != start)
        {
            char prev = d[cur].prev;
            for (auto& way : d[prev].ways)
            {
                if (way.to == cur)
                {
                    way.capacity -= way.flow;
                    break;
                }
            }

            for (auto& reverse_way : d[cur].ways)
            {
                if (reverse_way.to == prev)
                {
                    reverse_way.capacity -= reverse_way.flow;
                    break;
                }
            }

            cur = prev;
        }

        min_capacity = 0;

        for (auto& i : d)
            i.second.is_used = false;
    }

    friend std::ostream& operator<<(std::ostream& out, Network net)
    {
        out << "\nAnswer:\n";

        net.sort();

        size_t res_flow = 0;
        for (auto& start_way : net.d[net.start].ways)
            res_flow += std::max(0, start_way.flow);
        out << res_flow << "\n";

        for (auto& elem : net.d)
        {
            for (auto& way : elem.second.ways)
                out << elem.first << " " << way.to << " " << std::max(0, way.flow) << "\n";
        }

        return out;
    }

    void sort()
    {
        for (auto& i : d)
        {
            std::sort(i.second.ways.begin(), i.second.ways.end(), [](WayInfo i1, WayInfo i2) {return i1.to < i2.to; });
        }
    }

    void reverse_sort()
    {
        for (auto& i : d)
        {
            std::sort(i.second.ways.begin(), i.second.ways.end(), [](WayInfo i1, WayInfo i2) {return i1.capacity > i2.capacity; });
        }
    }

};

int main()
{
    Network StartNetwork;

    char start = 0;
    char end = 0;
    int n = 0;

    std::ifstream in("input.txt");
    std::ofstream out("output.txt");

    in >> n >> start >> end;
    StartNetwork.setEnds(start, end);

    char p1 = 0;
    char p2 = 0;
    int len = 0;

    for (int i = 0; i < n; ++i)
    {
        in >> p1 >> p2 >> len;

        Network::WayInfo way = { p2, len, 0 };
        StartNetwork.addElem(p1, way);
    }

    Network CurrentNetwork(StartNetwork);
    CurrentNetwork.reverse_sort();
    while (true)
    {
        bool res = CurrentNetwork.findWay(out);
        if (res == false)
            break;

        CurrentNetwork.findMin(out);
        CurrentNetwork.modifyFlow();
        CurrentNetwork.modifyCapacity();
    }

    out << CurrentNetwork;

    return 0;
}
