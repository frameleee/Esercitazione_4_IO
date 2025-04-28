
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>
#include <string>

namespace PolygonalLibrary {

struct Cell0D {
    int id;
    int marker;
    double x, y;
};

struct Cell1D {
    int id;
    int marker;
    int origin;
    int end;
};

struct Cell2D {
    int id;
    int marker;
    std::vector<int> vertices;
    std::vector<int> edges;
};

class PolygonalMesh {
public:
    unsigned int NumCell0Ds = 0;
    unsigned int NumCell1Ds = 0;
    unsigned int NumCell2Ds = 0;

    std::map<unsigned int, Cell0D> Cell0Ds;
    std::map<unsigned int, Cell1D> Cell1Ds;
    std::map<unsigned int, Cell2D> Cell2Ds;

    bool ImportMesh(const std::string& folderPath)
    {
        if (!LoadCell0Ds(folderPath + "/Cell0Ds.csv")) return false;
        if (!LoadCell1Ds(folderPath + "/Cell1Ds.csv")) return false;
        if (!LoadCell2Ds(folderPath + "/Cell2Ds.csv")) return false;

        NumCell0Ds = Cell0Ds.size();
        NumCell1Ds = Cell1Ds.size();
        NumCell2Ds = Cell2Ds.size();

        return true;
    }

    void ValidateMesh() const
    {
        for (const auto& [id, edge] : Cell1Ds)
        {
            assert(EdgeLength(edge) > 0.0 && "Found edge with zero length!");
        }
        for (const auto& [id, poly] : Cell2Ds)
        {
            assert(PolygonArea(poly) > 0.0 && "Found polygon with zero area!");
        }
        std::cout << "Mesh validation passed!" << std::endl;
    }

private:
    bool LoadCell0Ds(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        std::getline(file, line); // skip header
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string token;
            Cell0D c;
            std::getline(ss, token, ';'); c.id = std::stoi(token);
            std::getline(ss, token, ';'); c.marker = std::stoi(token);
            std::getline(ss, token, ';'); c.x = std::stod(token);
            std::getline(ss, token, ';'); c.y = std::stod(token);
            Cell0Ds[c.id] = c;
        }
        return true;
    }

    bool LoadCell1Ds(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        std::getline(file, line); // skip header
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string token;
            Cell1D c;
            std::getline(ss, token, ';'); c.id = std::stoi(token);
            std::getline(ss, token, ';'); c.marker = std::stoi(token);
            std::getline(ss, token, ';'); c.origin = std::stoi(token);
            std::getline(ss, token, ';'); c.end = std::stoi(token);
            Cell1Ds[c.id] = c;
        }
        return true;
    }

    bool LoadCell2Ds(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        std::string line;
        std::getline(file, line); // skip header
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            std::string token;
            Cell2D c;
            std::getline(ss, token, ';'); c.id = std::stoi(token);
            std::getline(ss, token, ';'); c.marker = std::stoi(token);
            std::getline(ss, token, ';'); int numVertices = std::stoi(token);
            for (int i = 0; i < numVertices; ++i)
            {
                std::getline(ss, token, ';');
                c.vertices.push_back(std::stoi(token));
            }
            std::getline(ss, token, ';'); int numEdges = std::stoi(token);
            for (int i = 0; i < numEdges; ++i)
            {
                std::getline(ss, token, ';');
                c.edges.push_back(std::stoi(token));
            }
            Cell2Ds[c.id] = c;
        }
        return true;
    }

    double EdgeLength(const Cell1D& edge) const
    {
        const Cell0D& o = Cell0Ds.at(edge.origin);
        const Cell0D& e = Cell0Ds.at(edge.end);
        return std::sqrt((o.x - e.x)*(o.x - e.x) + (o.y - e.y)*(o.y - e.y));
    }

    double PolygonArea(const Cell2D& cell) const
    {
        double area = 0;
        int n = cell.vertices.size();
        for (int i = 0; i < n; ++i)
        {
            const Cell0D& p1 = Cell0Ds.at(cell.vertices[i]);
            const Cell0D& p2 = Cell0Ds.at(cell.vertices[(i+1)%n]);
            area += (p1.x * p2.y - p2.x * p1.y);
        }
        return std::abs(area) / 2.0;
    }
};

} // namespace PolygonalLibrary

int main()
{
    PolygonalLibrary::PolygonalMesh mesh;

    if (!mesh.ImportMesh("./"))
    {
        std::cerr << "Error loading mesh files" << std::endl;
        return 1;
    }

    mesh.ValidateMesh();

    return 0;
}
