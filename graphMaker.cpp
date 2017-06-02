
#include "graphMaker.h"
#include <stack>
#include "NodeLua.h"
#include "hashlibpp.h"
using namespace std;


//------------------------------------------------------------------
void GraphMaker::onChange()
{
    //create master script for emitNode
    ofstream file;
    file.open("master.lua");
    file << loadFileIntoString(PATHTOSRC"/lua_constant/header.lua");
    ForIndex(i,m_NodeGraph.nodeWindows.size()){
        Node* n = m_NodeGraph.nodeWindows[i]->getNode();
        if(n->isEmitingNode()){
            n->writeMaster(file);
        }
    }
    file.close();
    Updater up;
    up.val = 0;
    Messaging::getInstance().send_update(up);
}

//------------------------------------------------------------------
void GraphMaker::deleteNodeWindow(NodeWindow* nw)
{
    //remove all the connectiion
    Node* toDelete = nw->getNode();
    ForIndex(i,m_NodeGraph.nodeWindows.size()){
        Node* n = m_NodeGraph.nodeWindows[i]->getNode();
        //test if n is connected to the node to delete
        n->removeConnectionTo(toDelete);
        m_NodeGraph.nodeWindows[i]->removeConnectionTo(nw);
    }
    ForIndex(i,m_NodeGraph.nodeWindows.size()){
        if(m_NodeGraph.nodeWindows[i] == nw){
            m_NodeGraph.nodeWindows.erase(m_NodeGraph.nodeWindows.begin()+i);
            break;
        }
    }
    delete toDelete;
    delete nw;
}

//------------------------------------------------------------------
void GraphMaker::checkError(script_error &err)
{
    GraphError er;
    ForIndex(i,m_NodeGraph.nodeWindows.size()){
        Node* n = m_NodeGraph.nodeWindows[i]->getNode();
        n->setErrorState(false);
    }
    if(err.code == 0){
        //console.clear();
        return;
    }
    if(err.line == -1){ // error line isn't given by IceSL

        console.append(err.msg);
        return;
    }
    ForIndex(i,m_NodeGraph.nodeWindows.size()){
        Node* n = m_NodeGraph.nodeWindows[i]->getNode();
        if(n->isEmitingNode()){
            std::vector<Node*> orderedNode;
            n->orderNode(orderedNode);
            er.computeMap(orderedNode);
            Node* nodeErr = er.getNodeByErrorLine(err.line);
            nodeErr->setErrorState(true);
            console.append(err.msg);
        }
    }
}

//------------------------------------------------------------------
void GraphMaker::makeNewNode(string path,v2i pos){
    NodeWindow* n = nullptr;
    int s = 0;
    string name = project.relativePath(path);
    string type = name;
    bool isEmitting = false;
    name = name.substr(0,name.size()-4);
    if(m_NodeGraph.numberOfWindowsByType.find(type)!=m_NodeGraph.numberOfWindowsByType.end()){
        s = ++m_NodeGraph.numberOfWindowsByType.at(type);
        name = string(name)+std::to_string(s);
    }else{
        m_NodeGraph.numberOfWindowsByType[type] = 0;
    }

    n = new NodeWindow(new Node(path,type),name.c_str(),pos);

    cerr << name << " is Emitting " << n->getNode()->isEmitingNode() << endl;

    if(n != nullptr)m_NodeGraph.nodeWindows.push_back(n);
}


//------------------------------------------------------------------
void GraphMaker::convert_error(script_error &err){

    if(err.code == 1)//lua error
    {
        checkError(err);
    }
    if(err.code == 2)//...
    {

    }
}

//------------------------------------------------------------------
void GraphMaker::saveGraph(string path)
{
    GraphSaver::saveGraph(path,m_NodeGraph);
}

//------------------------------------------------------------------
void GraphMaker::loadGraph(string path)
{
    GraphSaver::loadGraph(project,path,m_NodeGraph);
    this->activeProject = true;
    console.clear();
    this->onChange();
}

//-------------------------------------
void GraphMaker::RenderMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Quit", "CTRL+W")) {}
            ImGui::EndMenu();
        }
        bool activeProject= hasActiveProject();
        if (ImGui::BeginMenu("Project")) {
            if (ImGui::MenuItem("New..", "CTRL+N")) {
                string path = openPathDialog();
                createProject(path);

            }
            if (ImGui::MenuItem("Save", "CTRL+S" ,false,activeProject)) {
                saveGraph(getProject().path+"/graph.xml");
            }
            if (ImGui::MenuItem("Save As..", "" ,false,activeProject)) {
                string path = saveFileDialog("test");
                saveGraph(path);
            }
            if (ImGui::MenuItem("Open", "CTRL+L")) {
                string ext = string("Xml graph (*.xml);; All Files (*.*)");
                string path = openFileDialog(ext);
                loadGraph(path);
            }
            if (ImGui::MenuItem("Import lua", "CTRL+I")) {
                string ext = string("Lua Script (*.lua);; All Files (*.*)");
                string path = openFileDialog(ext);
                if(strcmp(path.c_str(),"")!=0)getProject().importLua(path);

            }
            ImGui::EndMenu();

        }
        ImGui::EndMainMenuBar();
    }
}

//------------------------------------------------------------------
void GraphMaker::onIdle(){
    ForIndex(i,m_NodeGraph.nodeWindows.size()){
        Node* ni = m_NodeGraph.nodeWindows[i]->getNode();
        if(ni->hasChange()){
            ni->reloadProgram();
            console.append(string(ni->getRelativePath() + " reloaded."));
        }
    }
}

//-------------------------------------
void GraphMaker::renderImgui(){
    bool mouseDown = ImGui::IsMouseClicked(0);
    onIdle();
    if(m_showRMenu){
        if(hasActiveProject()){
            string fileName = getProject().renderFileSelecter(m_mousePos);
            if(fileName.size() > 0){
                makeNewNode(fileName,m_mousePos);
                m_showRMenu = false;
            }
        }
    }
    RenderMenu();
    ForIndex(i,getNodeWindows().size()){
       bool toDelete = getNodeWindows()[i]->display();
       if(toDelete){//clicked on the little cross
            deleteNodeWindow(getNodeWindows()[i]);
       }
       getNodeWindows()[i]->renderAndPick(m_nodeSelecter,mouseDown);
    }
    //nothing has been picked but mouse was Down
    if(mouseDown && (!m_nodeSelecter.outputHasBeenPicked && !m_nodeSelecter.inputHasBeenPicked)){
        m_nodeSelecter.reset();
    }
    m_nodeSelecter.connect();
    m_nodeSelecter.outputHasBeenPicked = false;
    m_nodeSelecter.inputHasBeenPicked = false;
    console.display();
    ImGui::Render();

}
