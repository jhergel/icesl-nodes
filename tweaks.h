#pragma once
#include <LibSL/LibSL.h>
#include "imgui/imgui.h"
#include "FileDialog.h"

//JH: NOTE: I haven't templated this because the drawUI() function will change for every version of the class
// If you want to implement new tweaks you have to: - implement the class here
//                                                  - create the regexp in NodeLua
//                                                  - add the lua function in set_up_env.lua

class Node;
enum tweak_type{t_string,t_int,t_scalar,t_slider,t_color,t_v3f,t_v3i,t_path,t_notype} ;

class Tweak{
public:
    virtual void drawUi() = 0;//draw Imgui stuff inside the node
    virtual std::string getValueOnString() = 0;
    virtual std::string getValueToLua() = 0;

    std::string& getName(){return name;}
    Tweak(Node* node, std::string name,tweak_type tp):m_Node(node),name(name),m_type(tp){}

    tweak_type getType(){
        return m_type;
    }

protected:
    Node* m_Node;
    std::string name;
    tweak_type m_type;
    void onChange();//called if the value of the tweak change
};


class TweakString: public Tweak{
    char value[512];//because imgui doesnt handle string
public:
    TweakString(Node* node,std::string name, std::string svalue):Tweak(node,name,t_string){
        memcpy(value,svalue.c_str(),svalue.size());

    }

    void drawUi(){
        if(ImGui::InputText(name.c_str(),value,512)){
            onChange();
        }
    }

    std::string getValueOnString(){
        return std::string(value);
    }
    std::string getValueToLua(){
        return std::string("\"") + std::string(value) + std::string("\"");
    }
};

class TweakInt: public Tweak{
    int m_value;//because imgui doesnt handle string
public:
    TweakInt(Node* node,std::string name, int value):Tweak(node,name,t_int){
        m_value = value;
    }

    void drawUi(){
        if(ImGui::InputInt(name.c_str(),&m_value)){
            onChange();
        }
    }
    std::string getValueOnString(){
        return std::to_string(m_value);
    }
    std::string getValueToLua(){
        return  std::to_string(m_value);
    }
};

//tweaks for a single float
class TweakScalar: public Tweak{
    float m_value;

public:
    TweakScalar(Node* node,std::string name, float value):m_value(value),Tweak(node,name,t_scalar){}

    void drawUi(){
        if(ImGui::InputFloat(name.c_str(),&m_value)){
            onChange();
        }
        //if(ImGui::SliderFloat(name.c_str(),&m_value))
    }
    std::string getValueOnString(){
        return std::to_string(m_value);
    }
    std::string getValueToLua(){
        return  std::to_string(m_value);
    }
};

//for a single float but with min and max values
class TweakSlider: public Tweak{
    float m_value;
    float m_min;
    float m_max;

public:
    TweakSlider(Node* node,std::string name, float value,float min, float max):m_value(value),m_min(min),m_max(max),Tweak(node,name,t_slider){}

    void drawUi(){
        if(ImGui::SliderFloat(name.c_str(),&m_value,m_min,m_max)){
            onChange();
        }
    }
    std::string getValueOnString(){
        return std::to_string(m_value)+ "," + std::to_string(m_min) + "," + std::to_string(m_max);
    }
    std::string getValueToLua(){
        return  std::to_string(m_value) ;
    }
};

//to write a path(It use fileDialog to chose the file).
class TweakPath: public Tweak{
    std::string m_value;
public:
    TweakPath(Node* node,std::string name, std::string svalue):Tweak(node,name,t_path){
        m_value = svalue;
    }

    void drawUi(){
        if(ImGui::Button(name.c_str(),ImVec2(40,15))){
            std::string xt = extractExtension(m_value);
            std::string ext = std::string("") + xt + "(*." + xt + ");; All Files (*.*)";
            std::string local = openFileDialog(ext);
            if(strcmp(local.c_str(),"") !=0){
                m_value = local;
                onChange();
            }
        }
        ImGui::TextWrapped(m_value.c_str());
    }

    std::string getValueOnString(){
        return m_value;
    }
    std::string getValueToLua(){
        return std::string("\"") + m_value + std::string("\"");
    }
};



//not fully implemented: missing regexp!
class TweakColor: public Tweak{
    v4f m_value;
public:
    TweakColor(Node* node,std::string name, v4f value):m_value(value),Tweak(node,name,t_color){}

    void drawUi(){
        if(ImGui::ColorEdit3(name.c_str(), (float*)&m_value)){
            onChange();
        }

    }
    std::string getValueOnString(){
        std::string ret = std::string("v(");
        ret+= std::to_string(m_value[0]) + "," + std::to_string(m_value[1]) + "," + std::to_string(m_value[2]) + ","+ std::to_string(m_value[3]) + ")";
        return ret;
    }
    std::string getValueToLua(){
        std::string ret = std::string("v(");
        ret+= std::to_string(m_value[0]) + "," + std::to_string(m_value[1]) + "," + std::to_string(m_value[2]) + ","+ std::to_string(m_value[3]) + ")";
        return ret;
    }
};

//not fully implemented: missing regexp!
class TweakVec3f: public Tweak{
    v3f m_value;

public:
    TweakVec3f(Node* node,std::string name, v3f value):m_value(value),Tweak(node,name,t_v3f){}

    void drawUi(){
        ImGui::InputFloat3(name.c_str(),(float*)&m_value);
    }

    std::string getValueOnString(){
        return std::string("\"") + std::to_string(m_value[0]) + "," + std::to_string(m_value[1])+ "," + std::to_string(m_value[2])+ std::string("\"");
    }
    std::string getValueToLua(){
        std::string ret = std::string("v(");
        ret+= std::to_string(m_value[0]) + "," + std::to_string(m_value[1]) + "," + std::to_string(m_value[2]) + ","+ std::to_string(m_value[3]) + ")";
        return ret;
    }
};

//not fully implemented: missing regexp!
class TweakVec3i: public Tweak{
    v3i m_value;
public:
    TweakVec3i(Node* node,std::string name, v3i value):m_value(value),Tweak(node,name,t_v3i){

    }

    void drawUi(){
        ImGui::InputInt3(name.c_str(),(int*)&m_value);
    }

    std::string getValueOnString(){
        return std::string("\"") + std::to_string(m_value[0]) + "," + std::to_string(m_value[1])+ "," + std::to_string(m_value[2])+ std::string("\"");
    }
};
