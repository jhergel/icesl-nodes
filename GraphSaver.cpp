#include "GraphSaver.h"


using namespace std;

//------------------------------------------------------------------
void GraphSaver::nameWindow(Project& project, NodeGraph& nodeGraph, NodeWindow* n)
{

    string name = n->getNode()->getPath();
    name = project.relativePath(name);
    string type = name;
    bool isEmitting = false;
    if(name.size() == 8){
        isEmitting = (strcmp(name.c_str(),"emit.lua") ==0);
    }
    cerr << name << endl;
    name = name.substr(0,name.size()-4);
    int s = 0;
    if(nodeGraph.numberOfWindowsByType.find(type)!=nodeGraph.numberOfWindowsByType.end()){
        s = ++nodeGraph.numberOfWindowsByType.at(type);
        name = string(name)+std::to_string(s);
    }else{
        nodeGraph.numberOfWindowsByType[type] = 0;
    }
    n->getNode()->setEmitingNode(isEmitting);

    n->setName(name);
}

//------------------------------------------------------------------
void GraphSaver::saveGraph(string path,NodeGraph& nodeGraph)
{
    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    doc.LinkEndChild(decl);
    std::vector<NodeWindow*>& nodeWindows = nodeGraph.nodeWindows;
    //for each nodes
    ForIndex(i,nodeWindows.size()){
        Node* n = nodeWindows[i]->getNode();
        string id = "node"+to_string((int64_t)n);
        TiXmlElement * element = new TiXmlElement(id.c_str());
        element->SetAttribute("type","node");
        TiXmlElement * elementFile = new TiXmlElement("file");

        string relativePath = string("node/")+n->getRelativePath();
        elementFile->SetAttribute("path",relativePath.c_str());
        elementFile->SetAttribute("hash",n->hashProgram().c_str());
        TiXmlElement * elementPos = new TiXmlElement("position");
        elementPos->SetAttribute("x",nodeWindows[i]->getPos()[0]);
        elementPos->SetAttribute("y",nodeWindows[i]->getPos()[1]);
        TiXmlElement * elementSize = new TiXmlElement("size");
        elementSize->SetAttribute("x",nodeWindows[i]->getSize()[0]);
        elementSize->SetAttribute("y",nodeWindows[i]->getSize()[1]);

        element->LinkEndChild(elementFile);
        element->LinkEndChild(elementPos);
        element->LinkEndChild(elementSize);

        for(auto connect: n->getPrevNamed()){
            TiXmlElement * elementIn = new TiXmlElement("input");
            string s0 = connect.first;
            if(connect.second.first == nullptr)continue;
            string s1 = to_string((int64_t)connect.second.first);
            string tesIn = string("node")+to_string((int64_t)n);

            string s2 = connect.second.second;
            string tesOut = string("node")+s1;
            elementIn->SetAttribute("inputNode", tesIn.c_str());
            elementIn->SetAttribute("inputField", s0.c_str());
            elementIn->SetAttribute("outputNode", tesOut.c_str());
            elementIn->SetAttribute("outputField", s2.c_str());
            element->LinkEndChild(elementIn);

        }
        TiXmlElement * elementTweaks = new TiXmlElement("Tweaks");
        for(auto tweakAndName: n->getTweaks()){
            TiXmlElement * currentTweak = new TiXmlElement(tweakAndName.first.c_str());
            string s = std::to_string(tweakAndName.second->getType());
            currentTweak->SetAttribute("type", s.c_str());
            currentTweak->SetAttribute("name", tweakAndName.first.c_str());
            currentTweak->SetAttribute("value", tweakAndName.second->getValueOnString().c_str());
            elementTweaks->LinkEndChild(currentTweak);

        }
        element->LinkEndChild(elementTweaks);

        doc.LinkEndChild(element);
    }
    doc.SaveFile(path.c_str());
}

//------------------------------------------------------------------
void strToVecXi(std::vector<int>& vec, string& value ){
    int start = 0;
    ForIndex(i,value.size()){
        if(value[i] == ','){
            string item = value.substr(start,i);
            vec.push_back(stoi(item));
            start = i+1;
        }
    }
    string item = value.substr(start,value.size()-1);
    vec.push_back(stoi(item));
}

//------------------------------------------------------------------
void strToVecXf(std::vector<float>& vec, string& value ){
    int start = 0;
    ForIndex(i,value.size()){
        if(value[i] == ','){
            string item = value.substr(start,i);
            vec.push_back(stof(item));
            start = i+1;
        }
    }
    string item = value.substr(start,value.size()-1);
    vec.push_back(stof(item));
}

//------------------------------------------------------------------
void loadTweak(NodeWindow* nw, string& type, string& name, string& value){
    Node* n = nw->getNode();
    tweak_type itype = (tweak_type)stoi(type);
    Tweak* tw = nullptr;
    std::vector<float> vecf;
    std::vector<int> veci;

    switch (itype) {
    case t_string:
        tw = new TweakString(n,name,value);
        break;
    case t_path:
        tw = new TweakPath(n,name,value);
        break;
    case t_int:
        tw = new TweakInt(n,name,stoi(value));
        break;
    case t_scalar:
        tw = new TweakScalar(n,name,stof(value));
        break;
    case t_slider:
        strToVecXf(vecf,value);
        tw = new TweakSlider(n,name,vecf[0],vecf[1],vecf[2]);
        break;
    case t_v3f:
        strToVecXf(vecf,value);
        tw = new TweakVec3f(n,name,v3f(vecf[0],vecf[1],vecf[2]));
        break;
    case t_color:
        break;
    default:
        sl_assert(false);
        break;
    }
    if(tw!=nullptr)n->getTweaks()[name] = tw;

}

//------------------------------------------------------------------
void loadTweaks(NodeWindow* nw, TiXmlElement* element){

    for(auto subElem = element->FirstChildElement();  subElem != NULL; subElem = subElem->NextSiblingElement()){
        TiXmlElement* elTweak = subElem;
        std::map<string,string> keyVal;
        for(TiXmlAttribute* i = elTweak->FirstAttribute(); i != NULL;i = i->Next()){
            keyVal[string(i->Name())] = i->Value();
        }
        string type = keyVal["type"];
        string name = keyVal["name"];
        string value = keyVal["value"];
        loadTweak(nw,type,name,value);
    }

}

//------------------------------------------------------------------
bool GraphSaver::WriteValue(Project& project, NodeWindow* nw,std::vector<connection>& connections ,TiXmlElement* element,NodeGraph& nodeGraph)
{
    std::map<string,string> keyVal;
    for(TiXmlAttribute* i = element->FirstAttribute(); i != NULL;i = i->Next()){
        keyVal[string(i->Name())] = i->Value();
    }
    if(strcmp(element->Value(),"file") == 0){
        hashwrapper* myWrapper = new md5wrapper();
        string relative = keyVal["path"];//this is a relative path from the xml to the lua

        string storedHash = keyVal[string("hash")];
        string luaPath = project.path + "/" + relative;
        string hash = myWrapper->getHashFromString(loadFileIntoString(luaPath.c_str()));
        delete myWrapper;
        //if(strcmp(hash.c_str(), storedHash.c_str()) != 0)return false;
        nw->getNode()->changePath(luaPath,project);
        nameWindow(project,nodeGraph,nw);
        nw->nodeChange();
    }
    if(strcmp(element->Value(),"position") == 0){
        v2i pos = v2i(stoi(keyVal["x"]),stoi(keyVal["y"]));
        nw->setPos(pos);
    }
    if(strcmp(element->Value(),"size") == 0){
        v2i size = v2i(stoi(keyVal["x"]),stoi(keyVal["y"]));
        nw->setSize(size);
    }
    if(strcmp(element->Value(),"input") == 0){
        connection c;
        c.inputName = keyVal["inputField"];
        c.nodeInputName = keyVal["inputNode"];
        c.outputName = keyVal["outputField"];
        c.nodeOutputName = keyVal["outputNode"];
        connections.push_back(c);
    }
    if(strcmp(element->Value(),"Tweaks") == 0){
        loadTweaks(nw,element);

    }
    return true;
}

//------------------------------------------------------------------
bool GraphSaver::loadNode(Project& project, map<string,NodeWindow*>& nodebyName,std::vector<connection>& connections,TiXmlElement* element,NodeGraph& nodeGraph)
{
    string name = element->Value();
    if(nodebyName.find(name) != nodebyName.end())return true;
    NodeWindow* nodeW = new NodeWindow();
    nodeW->setNode(new Node());
    bool loadConnection = true;
    for(auto subElem = element->FirstChildElement();  subElem != NULL; subElem = subElem->NextSiblingElement()){
        loadConnection = loadConnection && WriteValue(project,nodeW,connections,subElem,nodeGraph);
    }
    nodebyName[name] = nodeW;
    nodeGraph.nodeWindows.push_back(nodeW);
    return loadConnection;

}

//------------------------------------------------------------------
void GraphSaver::loadGraph(Project& project, std::string path, NodeGraph& nodeGraph)
{

    project.exctractPathFromXml(path);
    nodeGraph.nodeWindows.clear();
    nodeGraph.numberOfWindowsByType.clear();
    cout << "loading graph '" + path + "'." << endl;

    if (!LibSL::System::File::exists(path.c_str())) {
        cout << "No file found '" + path + "' found." << endl;
        return;
    }
    TiXmlDocument doc(path.c_str());
    if (!doc.LoadFile()) {
        cerr << Console::yellow;
        cerr << "[load] Warning: unreadable file '" << path << endl;
        cerr << Console::gray;
        return;
    }
    std::stack<TiXmlNode*> nodes;
    std::map<string,NodeWindow*> nodeByName;

    nodes.push(&doc);
    std::vector<connection> connections;
    bool loadConnection = true;
    while (!nodes.empty()) {
        TiXmlNode *current = nodes.top();
        nodes.pop();
        if (current->Type() == TiXmlNode::TINYXML_ELEMENT) {
            TiXmlElement* elem = current->ToElement();
            std::map<string,string> keyVal;

            for(TiXmlAttribute* i = elem->FirstAttribute(); i != NULL;i = i->Next()){
                keyVal[string(i->Name())] = i->Value();
            }
            if((keyVal.find(string("type")) != keyVal.end()) && strcmp(keyVal[string("type")].c_str(),"node") == 0){
                loadNode(project,nodeByName,connections,elem,nodeGraph);
            }
        }
        for (TiXmlNode *pChild = current->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
            nodes.push(pChild);
        }
    }
    //we go here only if all file doesn't change between save and load
    if(loadConnection){
        ForIndex(i,connections.size()){
            NodeWindow* nw1 = nodeByName[connections[i].nodeInputName];
            NodeWindow* nw2 = nodeByName[connections[i].nodeOutputName];
            nw1->connectPreviousWindow(nw2,connections[i].inputName,connections[i].outputName);
        }
    }

}
