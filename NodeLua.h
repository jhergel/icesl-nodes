#pragma once
#include <LibSL/LibSL.h>
#include "tweaks.h"
#include "hashlibpp.h"
#include "project.h"
class GraphMaker;

class Node{
private:
private:
#ifdef WIN32
  typedef FILETIME t_FileTime;
#else
  typedef time_t   t_FileTime;
#endif
  t_FileTime m_timeStamp;

protected:
  std::map<std::string,std::pair<Node*,std::string>> prevNamed;
  std::map<std::string,Node*> nextNamed;
  std::vector<std::string> inputName;
  std::vector<std::string> outputName;
  std::map<std::string,Tweak*> tweaks;

  void makeNewInput(std::string name);
  void makeNewOutput(std::string name);

  std::string m_Path;//absolute path
  std::string m_RelativePath;//path relative to the project directory
  std::string m_Program;//code

  bool m_emitingNode;//tag the special type of node that is emitting into the scene
  bool hasEmitAndNotOutput;//tag the node that are using emit and not output
  bool errorState;//tag the node if it doesn't compile(IceSL give the error back).
public:
  void onChange(); //call back on change on tweaks and others

  void setEmitingNode(bool b){
      m_emitingNode = b;
  }

  void setErrorState(bool b){
      errorState = b;
  }

  bool isInErrorState(){
      return errorState;
  }

  bool isEmitingNode(){
      return m_emitingNode;
  }

  std::map<std::string,Tweak*>& getTweaks(){ return tweaks;}
  Node()
  {
      m_emitingNode = false;
      errorState = false;
      hasEmitAndNotOutput = false;

  }
  Node(std::string path, std::string relativePath):m_Path(path)
  {
    m_RelativePath = relativePath;
    m_Program = loadFileIntoString(m_Path.c_str());
    m_emitingNode = strcmp(m_RelativePath.c_str(),"emit.lua") == 0;
    m_timeStamp = fileTimestamp(m_Path);
    parse();
    errorState = false;

  }

  int getIndiceOutByName(std::string s){
    ForIndex(i,outputName.size()){
      if(strcmp(s.c_str(),outputName[i].c_str()) == 0){
         return i;
        }
    }
    sl_assert(false);
  }

  int getIndiceInByName(std::string s){
    ForIndex(i,inputName.size()){
      if(strcmp(s.c_str(),inputName[i].c_str()) == 0){
         return i;
        }
    }
    sl_assert(false);
  }
  std::map<std::string,std::pair<Node*,std::string>>& getPrevNamed(){return prevNamed;}
  std::map<std::string,Node*>& getNextNamed(){return nextNamed;}
  std::vector<std::string>& getInputName(){return inputName;}
  std::vector<std::string>& getoutputName(){return outputName;  }
  std::string getPath(){return m_Path;}
  void changePath(std::string path,Project p);
  std::string getRelativePath(){return m_RelativePath;}

  std::string codeBefore(); //code to write before writing the node in the master script
  std::string codeAfter(); //code to write after writing the node in the master script
  void removeConnectionTo(Node* n);
  std::string hashProgram(); //hash the file
  bool writeMaster(std::ofstream& myfile); //write the master script of the node.
  void connect(Node* n,std::string outName,int pos); //connect to another node
  bool isAscendent(Node* toConnect); // check if a node is a parent
  bool isConnectedToInput(); //check if all input are conencted
  void orderNode(std::vector<Node*>& orderedNode); //order the nodes before writing master script
  void parse(); //parse to find input, output, gather emit
  void parseTweaks(); //parse to find the tweaks and add them in the interface.
  void parseInputAndOutput();
  void reloadProgram();
  t_FileTime fileTimestamp(std::string file) const;
  bool fileChanged(std::string file, t_FileTime& _last) const;
  bool hasChange();
};

