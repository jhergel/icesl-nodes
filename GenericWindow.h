#pragma once

#include <LibSL/LibSL.h>

#include "imgui/imgui.h"
#include "NodeLua.h"


/************************************/
/************************************/
/************************************/


class GenericWindow{
protected:
    bool m_show;
    v2i m_size;
    v2i m_pos;
    bool m_isNew;
    std::string m_name;
    ImDrawList* m_drawList;
public:
    GenericWindow():m_size(v2i(100,100)),m_pos(v2i(100,100))
    {
        m_show = true;
        m_isNew = true;
    }

    GenericWindow(const char *name,v2i pos):m_show(true),m_size(v2i(100,100)),m_pos(pos),m_name(name)
    {
        m_show = true;
        m_isNew = true;
    }
    GenericWindow(const char *name,v2i pos,v2i size):m_show(true),m_size(size),m_pos(pos),m_name(name){

    }

    GenericWindow(const char *name):m_show(true),m_size(v2i(100,100)),m_pos(v2i(100,100)),m_name(name)
    {
        m_show = true;
        m_isNew = true;
    }

    v2i getSize(){return m_size;}
    v2i getPos(){return m_pos;}
    void setSize(v2i s){m_size = s; m_isNew = true;}
    void setPos(v2i p){m_pos = p;  m_isNew = true;}

    void handlePosAndSize(){
        if(m_isNew){
            ImVec2 posVec2 = ImVec2(m_pos[0],m_pos[1]);
            ImVec2 sizeVec2 = ImVec2(m_size[0],m_size[1]);
            ImGui::SetWindowPos(posVec2);
            ImGui::SetWindowSize(sizeVec2);
            m_isNew =false;
        }
    }
    std::string getName(){return m_name;}
    void setName(std::string str){m_name = str;}
    virtual bool display() = 0;
};


//write the error message comming back from IceSL
//at the bottom of the main window
class ConsoleWindow :public GenericWindow{
    std::string m_message;
    bool m_scrollToBottom;
public:
    ConsoleWindow():GenericWindow(){m_message = std::string();}
    ConsoleWindow(v2i pos, v2i size):GenericWindow("console",pos,size),m_message(""){}
    void append(std::string str){
        m_message+= "\n";
        m_message+= str;
        m_scrollToBottom = true;
    }

    void clear(){
        m_message = "";
        m_scrollToBottom = true;
    }
    bool display(){
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        if(strcmp(m_message.c_str(),"") == 0){
            ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.7,1.0,0.7,1));
        }else{
            ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1.0,0.0,0.0,1));

        }
        ImGui::Begin(m_name.c_str(),&m_show,ImVec2(m_size[0],m_size[1]),-1.0f, ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings|
                ImGuiWindowFlags_NoTitleBar
                );
        handlePosAndSize();

        ImGui::TextWrapped(m_message.c_str());
        ImGui::Separator();
        if(m_scrollToBottom){
            ImGui::SetScrollHere();
        }
            m_scrollToBottom = false;
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        return true;
    }
};
