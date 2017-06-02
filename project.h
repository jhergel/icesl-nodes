#pragma once

#include "imgui/imgui.h"
#include <iostream>
#include <LibSL/LibSL.h>
#ifndef WIN32
#include <boost/filesystem.hpp>



class Project{
public:
    std::string path;
    std::set<std::string> openFolders;
    void importLua( std::string path){
        std::string nodeDir = this->path + "/node/";
        boost::filesystem::path src_path(path);
        nodeDir+=  src_path.filename().generic_string();
        std::cerr << nodeDir << std::endl;
        std::cerr << path << std::endl;

        boost::filesystem::path dest_path(nodeDir);
        boost::filesystem::copy_file(src_path,dest_path,boost::filesystem::copy_option::overwrite_if_exists);
    }

    std::string nodefolder(){
        std::string NodeDir = path + "/node/";
        return NodeDir;
    }
    void createNodefolder(){
        std::string NodeDir = path + "/node";
        boost::filesystem::path dir(NodeDir);
        boost::filesystem::detail::create_directory(dir);
    }

    void copyEmitNode(){
        importLua(std::string(PATHTOSRC"/lua_constant/emit.lua"));
    }

    void listLuaFileInDir(std::vector<std::string>& files){
        std::string NodeDir = path + "/node";
        boost::filesystem::path dir(NodeDir);

        for ( boost::filesystem::directory_iterator itr(dir); itr!= boost::filesystem::directory_iterator(); ++itr)
        {
            boost::filesystem::path file = itr->path();
            if(!is_directory(file))files.push_back(file.filename().generic_string());
        }
    }

    void listLuaFileInDir(std::vector<std::string>& files,std::string directory){
        boost::filesystem::path dir(directory);

        for ( boost::filesystem::directory_iterator itr(dir); itr!= boost::filesystem::directory_iterator(); ++itr)
        {
            boost::filesystem::path file = itr->path();
            if(!is_directory(file))files.push_back(file.filename().generic_string());
        }
    }

    void listFolderinDir(std::vector<std::string>& files, std::string folder){
        boost::filesystem::path dir(folder);

        for ( boost::filesystem::directory_iterator itr(dir); itr!= boost::filesystem::directory_iterator(); ++itr)
        {
            boost::filesystem::path file = itr->path();
            if(is_directory(file))files.push_back(file.filename().generic_string());
        }
    }


    void exctractPathFromXml(std::string& s){
        boost::filesystem::path p(s.c_str());
        boost::filesystem::path dir = p.parent_path();
        path = dir.generic_string();
    }
    std::string relativePath(std::string& path) {
        int nfsize = nodefolder().size();
        std::string name = path.substr(nfsize);
        return name;
    }

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

    std::string renderFileSelecter(v2i pos){
        std::string nameDir = "";
        ImGui::Begin("Menu");
        nameDir = recursiveFileSelecter(path+"/node");
        ImGui::SetWindowPos(ImVec2(pos[0],pos[1]));
        ImGui::End();
        return nameDir;

    }
};
#endif
#ifdef WIN32
#include <Windows.h>
class Project {
public:
	std::string path;
	void importLua(std::string srcPath) {
		std::string nodeDir = this->path + "/node/";
		nodeDir += Project::fileName(srcPath);

		CopyFile(LPCWSTR(srcPath.c_str()), LPCWSTR(nodeDir.c_str()), FALSE);
	}

	std::string nodefolder() {
		std::string NodeDir = path + "/node/";
		return NodeDir;
	}
	void createNodefolder() {
		std::string NodeDir = path + "/node";
		createDirectory(NodeDir.c_str());
	}

	void copyEmitNode() {
		importLua(std::string(PATHTOSRC"/lua_constant/emit.lua"));
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
    std::string relativePath(std::string& path) {
        int nfsize = nodefolder().size();
        std::string name = path.substr(nfsize);
        return name;
    }

};

#endif
