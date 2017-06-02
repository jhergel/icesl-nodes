#include "NodeWindow.h"
#include <LibSL.h>

//this class implement everything to handle the iceSL error that are coming back through TCP
class GraphError{
private:
    std::vector<std::pair<int,Node*> > nodesByLine;//the end line of each node
    int m_start;
    int numberOfLine(std::string path){
        int nol= 0;
        std::string line;
        std::ifstream file(path);
        while (std::getline(file, line))nol++;
        return nol;
    }

    int numberOfLineBefore(Node* n){
        int nol= 0;
        std::string line;
        std::istringstream stream(n->codeBefore());
        while (std::getline(stream, line))nol++;
        return nol;
    }
    int numberOfLineAfter(Node* n){
        int nol= 0;
        std::string line;
        std::istringstream stream(n->codeAfter());
        while (std::getline(stream, line))nol++;
        return nol;
    }
public:
    GraphError(){
        m_start = 0;
    }

    void computeMap(std::vector<Node*>& orderedNode){
        std::string s(PATHTOSRC"/lua_constant/header.lua");
        int start = m_start+numberOfLine(s);

        int end = start;
        for(Node* n: orderedNode){
            start += numberOfLineBefore(n);
            end = start+numberOfLine(n->getPath());
            end += numberOfLineAfter(n);

            nodesByLine.push_back(std::make_pair(end,n));
            start = end;
        }
        m_start = end;
    }
    //get the node in function of the line of error in the master script
    Node* getNodeByErrorLine(int line){
        ForIndex(i,nodesByLine.size()){
            std::pair<int,Node*> pa = nodesByLine[i];
            if(pa.first > line)return pa.second;
        }
        sl_assert(false);
        return nullptr;
    }
};



