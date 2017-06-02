#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>

#include "node.h"
#include "node_lua.h"

#include "imgui/imgui.h"

using namespace std;

void NodeWindow::displayNode(NodeSelecter& ns,bool mouseDown){

  ImVec2 offsetGUI = ImVec2(size[0],size[1]);

  ImGui::Begin(name.c_str(), &show,offsetGUI );
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  if(isNew){
      ImVec2 posVec2 = ImVec2(pos[0],pos[1]);
      ImVec2 sizeVec2 = ImVec2(size[0],size[1]);
      ImGui::SetWindowPos(posVec2);
      ImGui::SetWindowSize(sizeVec2);
      isNew =false;
  }
  pos = v2i(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y);
  size = v2i(ImGui::GetWindowSize().x,ImGui::GetWindowSize().y);

  for(auto st: node->getTweaks()){
      st.second->drawUi();
  }
  ImGui::End();

  ImColor color = ImColor(150,150,150,150);
  v2i Mpos = v2i(ImGui::GetMousePos().x,ImGui::GetMousePos().y);

  ForIndex(i,node->getInputName().size()){
    v2i Cpos = v2i(GetInputSlotPos(i).x,GetInputSlotPos(i).y);
    if(sqLength(Cpos-Mpos) < 100){
        color =ImColor(150,0,0,150);
        if(mouseDown){
            if(!ns.inputHasBeenPicked){
                ns.nodePickedInput.nodeWindow = this;
                ns.nodePickedInput.pos = i;
                ns.inputHasBeenPicked = true;
              }
          }
      }
    if(ns.nodePickedInput.nodeWindow == this && ns.nodePickedInput.pos == i)color =ImColor(0,150,0,150);
    draw_list->AddCircleFilled(GetInputSlotPos(i), 10, color,64);
    color = ImColor(150,150,150,150);
  }
  ForIndex(i,node->getoutputName().size()){
    v2i Cpos = v2i(GetOutputSlotPos(i).x,GetOutputSlotPos(i).y);
    if(sqLength(Cpos-Mpos) < 100){
        color =ImColor(150,0,0,150);
        if(mouseDown){
            if(!ns.outputHasBeenPicked){
                ns.nodePickedOutput.nodeWindow = this;
                ns.nodePickedOutput.pos = i;
                ns.outputHasBeenPicked = true;
              }
          }
      }
    if(ns.nodePickedOutput.nodeWindow == this && ns.nodePickedOutput.pos == i)color =ImColor(0,150,0,150);

    draw_list->AddCircleFilled(GetOutputSlotPos(i), 10, color,64);
    color = ImColor(150,150,150,150);
  }


  //Node* n = node;
  ForIndex(i,previousConnectedWindow.size()){
    NodeWindow* w= previousConnectedWindow[i];
    if(w == nullptr)continue;
    string s = node->getPrevNamed()[node->getInputName()[i]].second;
    Node* n = w->node;
    int outputSlot = n->getIndiceOutByName(s);
    ImVec2 p1 = w->GetOutputSlotPos(outputSlot);//TODO
    ImVec2 p2 = GetInputSlotPos(i);
    ImVec2 p3 = ImVec2(p1.x+50,p1.y);
    ImVec2 p4 = ImVec2(p2.x-50,p2.y);
    draw_list->AddBezierCurve(p1, p3, p4, p2, ImColor(200,200,100), 3.0f);
  }
  if(ns.nodePickedInput.nodeWindow == this){
      int pos =  ns.nodePickedInput.pos;
      ImVec2 p1 = GetInputSlotPos(pos);//TODO
      ImVec2 p2 = ImGui::GetMousePos();
      ImVec2 p3 = ImVec2(p1.x-50,p1.y);
      ImVec2 p4 = ImVec2(p2.x+50,p2.y);
      draw_list->AddBezierCurve(p1, p3, p4, p2, ImColor(200,200,100), 3.0f);
    }
  if(ns.nodePickedOutput.nodeWindow == this){
      int pos =  ns.nodePickedInput.pos;
      ImVec2 p1 = GetOutputSlotPos(pos);//TODO
      ImVec2 p2 = ImGui::GetMousePos();
      ImVec2 p3 = ImVec2(p1.x+50,p1.y);
      ImVec2 p4 = ImVec2(p2.x-50,p2.y);
      draw_list->AddBezierCurve(p1, p3, p4, p2, ImColor(200,200,100), 3.0f);
    }
}

void NodeWindow::connectPreviousWindow(NodeWindow* prev,int inpos,int outpos){
  if(prev->node->isAscendent(node))return;
  previousConnectedWindow[inpos] = prev;

  Node* n = prev->node;
  n->onChange();
  node->connect(n,n->getoutputName()[outpos],inpos);
}

void NodeWindow::connectPreviousWindow(NodeWindow* prev,string in,string out){

  int outpos = prev->node->getIndiceOutByName(out);
  int inpos = this->node->getIndiceInByName(in);
  connectPreviousWindow(prev,inpos,outpos);

}
