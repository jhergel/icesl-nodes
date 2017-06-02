#pragma once

#include <LibSL/LibSL.h>

#include "imgui/imgui.h"
#include "NodeLua.h"
#include "GenericWindow.h"


#ifndef WIN32
#include <sys/stat.h>
#include <fcntl.h>
#endif

/************************************/
/************************************/
/************************************/


struct NodeSelecter;


class NodeWindow:public GenericWindow{
    Node* node;
    std::vector<NodeWindow*> previousConnectedWindow;

public:
    NodeWindow():GenericWindow(){}
    NodeWindow(Node*n, const char *name,v2i pos):GenericWindow(name,pos),node(n)
    {
        previousConnectedWindow.resize(n->getInputName().size());
    }

    NodeWindow(Node*n, const char *name):GenericWindow(name),node(n)
    {
        m_show = true;
        previousConnectedWindow.resize(n->getInputName().size());
    }

    void drawInputValue();
    void drawOutputValue();

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
    ImVec2 GetInputSlotPos(int slot_no)   const { return ImVec2(m_pos[0], m_pos[1] + m_size[1] * ((float)slot_no+1) / (node->getPrevNamed().size()+1.0)); }
    ImVec2 GetOutputSlotPos(int slot_no)  const { return ImVec2(m_pos[0] + m_size[0], m_pos[1] + m_size[1] * ((float)slot_no+1) / (node->getNextNamed().size()+1.0)); }

    bool display();
    bool renderAndPick(NodeSelecter &ns, bool mouseDown);
    void displayNodeName();

    void removeConnectionTo(NodeWindow* nw){
        ForIndex(i,previousConnectedWindow.size()){
            if(previousConnectedWindow[i] == nw){
                previousConnectedWindow[i] = nullptr;
            }
        }
    }
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


//used to select two nodes to connect them.
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

};
