#pragma once

#include <LibSL/LibSL.h>

#include "imgui/imgui.h"
#include "node_lua.h"


/************************************/
/************************************/
/************************************/


struct NodeSelecter;


class NodeWindow{
    friend class GraphMaker;
    NodeWindow():size(v2i(100,100)),pos(v2i(100,100))
    {
        isNew = true;
    }

    NodeWindow(Node*n, const char *name,v2i pos):show(true),size(v2i(100,100)),pos(pos),name(name),node(n)
    {
        isNew = true;
        previousConnectedWindow.resize(n->getInputName().size());
    }

    NodeWindow(Node*n, const char *name):show(true),size(v2i(100,100)),pos(v2i(100,100)),name(name),node(n)
    {
        isNew = true;
        previousConnectedWindow.resize(n->getInputName().size());
    }
    Node* node;
    bool show;
    v2i size;
    v2i pos;
    bool isNew;
    std::string name;
    std::vector<NodeWindow*> previousConnectedWindow;

public:
    void drawInputValue();
    void drawOutputValue();

    v2i getSize(){return size;}
    v2i getPos(){return pos;}
    void setSize(v2i s){size = s; isNew = true;}
    void setPos(v2i p){pos = p;  isNew = true;}

    void connectPreviousWindow(NodeWindow* prev, int inpos, int outpos);
    void connectPreviousWindow(NodeWindow* prev,std::string in, std::string out);
    void nodeChange(){
        previousConnectedWindow.resize(node->getInputName().size());
    }

    Node* getNode(){return node;}
    void setNode(Node* n){
        node =n;
        previousConnectedWindow.resize(n->getInputName().size());
    }
    ImVec2 GetInputSlotPos(int slot_no)   const { return ImVec2(pos[0], pos[1] + size[1] * ((float)slot_no+1) / (node->getPrevNamed().size()+1.0)); }
    ImVec2 GetOutputSlotPos(int slot_no)  const { return ImVec2(pos[0] + size[0], pos[1] + size[1] * ((float)slot_no+1) / (node->getNextNamed().size()+1.0)); }
    std::string& getName(){return name;}

    void displayNode(NodeSelecter &ns, bool mouseDown);
};

struct NodePickedInfo{
    NodeWindow* nodeWindow;
    int pos;
    NodePickedInfo(){
        nodeWindow = nullptr;
        pos = 0;
    }
    NodePickedInfo(NodeWindow* nw, int pos){
        nodeWindow = nw;
        pos = pos;
    }
} ;

struct NodeSelecter{
    NodePickedInfo nodePickedInput;
    NodePickedInfo nodePickedOutput;
    bool inputHasBeenPicked;
    bool outputHasBeenPicked;
    NodeSelecter(){
        inputHasBeenPicked  = false;
        outputHasBeenPicked = false;
    }
    void reset(){
        nodePickedInput.nodeWindow  = nullptr;
        nodePickedOutput.nodeWindow = nullptr;
        nodePickedInput.pos  = 0;
        nodePickedOutput.pos = 0;
    }
    void connect(){
        if(nodePickedInput.nodeWindow == nullptr)return;
        if(nodePickedOutput.nodeWindow == nullptr)return;
        if(nodePickedInput.pos < 0)return;
        if(nodePickedOutput.pos < 0)return;
        nodePickedInput.nodeWindow->connectPreviousWindow(nodePickedOutput.nodeWindow,nodePickedInput.pos,nodePickedOutput.pos);

        reset();
    }
    void drawBezierCurve(){
        NodeWindow* n = nullptr;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        int pos;

    }
};
