#pragma once

#include "imgui/imgui.h"
#include <iostream>
#include <LibSL/LibSL.h>
#ifndef WIN32
#include <boost/filesystem.hpp>
#endif

#ifdef WIN32
#include "Windows.h"
#endif

class Project{
public:
    std::string path;
    std::set<std::string> openFolders;

#ifndef WIN32
    //---------------------------------------------------
    void importLua( std::string path){
        std::string nodeDir = this->path + "/node/";
        boost::filesystem::path src_path(path);
        nodeDir+=  src_path.filename().generic_string();
        boost::filesystem::path dest_path(nodeDir);
        boost::filesystem::copy_file(src_path,dest_path,boost::filesystem::copy_option::overwrite_if_exists);
    }

    //---------------------------------------------------
    void createNodefolder(){
        std::string NodeDir = path + "/node";
        boost::filesystem::path dir(NodeDir);
        boost::filesystem::detail::create_directory(dir);
    }

    //---------------------------------------------------
    void listLuaFileInDir(std::vector<std::string>& files){
        std::string NodeDir = path + "/node";
        boost::filesystem::path dir(NodeDir);
        for ( boost::filesystem::directory_iterator itr(dir); itr!= boost::filesystem::directory_iterator(); ++itr)
        {
            boost::filesystem::path file = itr->path();
            if(!is_directory(file))files.push_back(file.filename().generic_string());
        }
    }

    //---------------------------------------------------
    void listLuaFileInDir(std::vector<std::string>& files,std::string directory){
        boost::filesystem::path dir(directory);

        for ( boost::filesystem::directory_iterator itr(dir); itr!= boost::filesystem::directory_iterator(); ++itr)
        {
            boost::filesystem::path file = itr->path();
            if(!is_directory(file))files.push_back(file.filename().generic_string());
        }
    }

    //---------------------------------------------------
    void listFolderinDir(std::vector<std::string>& files, std::string folder){
        boost::filesystem::path dir(folder);

        for ( boost::filesystem::directory_iterator itr(dir); itr!= boost::filesystem::directory_iterator(); ++itr)
        {
            boost::filesystem::path file = itr->path();
            if(is_directory(file))files.push_back(file.filename().generic_string());
        }
    }

    //---------------------------------------------------
    bool copyDir(std::string source, std::string destination )
    {
        boost::filesystem::path src(source);
        boost::filesystem::path dest(destination);
        try
        {
            // Create the destination directory
            boost::filesystem::create_directory(dest);
            // Check whether the function call is valid
            if(!boost::filesystem::exists(src) || !boost::filesystem::is_directory(dest)){
                return false;
            }

        }catch(boost::filesystem::filesystem_error const & e){
            std::cerr << e.what() << '\n';
        }
        // Iterate through the source directory
        for(boost::filesystem::directory_iterator file(src);file != boost::filesystem::directory_iterator(); ++file){
            try{
                boost::filesystem::path current(file->path());
                if(boost::filesystem::is_directory(current)){
                    std::string next = destination + current.filename().string();
                    bool b = copyDir(current.string(),next);
                    if(!b){
                        return false;
                    }
                }else{
                    boost::filesystem::copy_file(current.string(),destination +"/"+ current.filename().string());
                }
            }catch(boost::filesystem::filesystem_error const & e){
                std:: cerr << e.what() << '\n';
            }
        }
        return true;
    }

    //---------------------------------------------------
    void exctractPathFromXml(std::string& s){
        boost::filesystem::path p(s.c_str());
        boost::filesystem::path dir = p.parent_path();
        path = dir.generic_string();
    }
#endif
#ifdef WIN32
    void importLua(std::string srcPath) {
        std::string nodeDir = this->path + "/node/";
        nodeDir += Project::fileName(srcPath);

        CopyFile(LPCWSTR(srcPath.c_str()), LPCWSTR(nodeDir.c_str()), FALSE);
    }

    void createNodefolder() {
        std::string NodeDir = path + "/node";
        createDirectory(NodeDir.c_str());
    }


    void listLuaFileInDir(std::vector<std::string>& files) {
        std::string NodeDir = path + "/node";
        listFiles(NodeDir.c_str(), files);
    }

    void exctractPathFromXml(std::string& s) {
        std::string directory;
        const size_t last_slash_idx = s.rfind('\\');
        if (std::string::npos != last_slash_idx)
        {
            directory = s.substr(0, last_slash_idx);
            path = directory;
        }
    }

    void listFolderinDir(std::vector<std::string>& files, std::string folder){

    }

    bool copyDir(std::string source, std::string destination )
    {
        return false;
    }
#endif
    //---------------------------------------------------
    std::string relativePath(std::string& path) {
        int nfsize = nodefolder().size();
        std::string name = path.substr(nfsize);
        return name;
    }

    //---------------------------------------------------
    std::string nodefolder(){
        std::string NodeDir = path + "/node/";
        return NodeDir;
    }
    //---------------------------------------------------
    void copyEmitNode(){
        importLua(std::string(PATHTOSRC"/lua_constant/emit.lua"));
    }
    //---------------------------------------------------
    std::string recursiveFileSelecter(std::string current_dir){
        std::vector<std::string> files;
        listLuaFileInDir(files,current_dir);
        std::vector<std::string> directories;
        std::string nameDir = "";
        listFolderinDir(directories,current_dir);
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(0.7,0.7,1.0,1));
        ForIndex(i,directories.size()){
            if(ImGui::BeginMenu(directories[i].c_str())){
                nameDir = recursiveFileSelecter(current_dir+"/"+directories[i]);
                ImGui::EndMenu();
            }
        }
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(1.,1.,1.0,1));
        ForIndex(i,files.size()){
            if(ImGui::MenuItem(files[i].c_str())){
                nameDir = current_dir+"/"+ files[i].c_str();
            }
        }
        ImGui::PopStyleColor();

        return nameDir;
    }

    //---------------------------------------------------
    std::string renderFileSelecter(v2i pos){
        std::string nameDir = "";
        ImGui::Begin("Menu");
        nameDir = recursiveFileSelecter(path+"/node");
        ImGui::SetWindowPos(ImVec2(pos[0],pos[1]));
        ImGui::End();
        return nameDir;

    }
};

