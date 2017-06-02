#pragma once
#include <LibSL/LibSL.h>
#include "NodeWindow.h"
#include "tinyxml.h"


//the windows. this is used to save and load the xml
class NodeGraph{
public:
    std::map<std::string,int> numberOfWindowsByType;//number of windows created by importing a single lua file
    std::vector<NodeWindow*> nodeWindows;
};

struct connection{
  std::string nodeInputName;
  std::string nodeOutputName;
  std::string inputName;
  std::string outputName;

};

//save and load the xml file that represent the graph.
class GraphSaver{
public:
    static void saveGraph(std::string path, NodeGraph& nodeGraph);
    static void loadGraph(Project& project, std::__cxx11::string path, NodeGraph& nodeGraph);
    static void nameWindow(Project& project, NodeGraph &nodeGraph, NodeWindow* n);
    static bool loadNode(Project& project, std::map<std::string, NodeWindow *>& nodebyName, std::vector<connection> &connections, TiXmlElement* element,NodeGraph& nodeGraph);
    static bool WriteValue(Project& project, NodeWindow* nw,std::vector<connection>& connections ,TiXmlElement* element,NodeGraph& nodeGraph);

};
