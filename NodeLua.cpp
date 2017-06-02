#include "graphMaker.h"
#include "NodeLua.h"
#include <regex>
using namespace std;



//-------------------------------------------------------
void Node::onChange(){
    GraphMaker::getInstance().onChange();
}

//-------------------------------------------------------
void Node::makeNewInput(string name){
    if(getPrevNamed().find(name) != getPrevNamed().end())return;
    getPrevNamed().insert(std::make_pair(name,make_pair(nullptr,"")));
    getInputName().push_back(name);
}

//-------------------------------------------------------
void Node::makeNewOutput(string name){

    if(getNextNamed().find(name) != getNextNamed().end())return;
    getNextNamed()[name] = nullptr;
    getoutputName().push_back(name);
    return;
}

//-------------------------------------------------------
void Node::removeConnectionTo(Node* n)
{
    for(auto edge: prevNamed){
        if(edge.second.first == n){
            prevNamed[edge.first] = make_pair(nullptr,"");
        }
    }
    for(auto edge: nextNamed){
        if(edge.second == n){
            nextNamed[edge.first] = nullptr;
        }
    }
}

//-------------------------------------------------------
std::string Node::codeBefore(){
    //write the current Id of the node

    string s = string("setfenv(1, _G0)  --go back to global initialization\n");
    s += string("__curentNodeId = ");
    s += to_string((int64_t)this);
    s += "\n";
    for(auto edge:this->prevNamed){
        string s2 = to_string((int64_t)edge.second.first);
        s+="__inputmap[\""+edge.first+"\"] = \"" + edge.second.second+s2+"\"\n";
    }

    s+= loadFileIntoString(PATHTOSRC"/lua_constant/set_up_env.lua");
    if(hasEmitAndNotOutput && !m_emitingNode){
        s+= loadFileIntoString(PATHTOSRC"/lua_constant/gather_emit.lua");
    }
    for(auto& st: tweaks){
        Tweak* t = st.second;
        s+="__tweaksTable[\""+t->getName()+"\"] = " +  t->getValueToLua() +"\n";
    }
    return s;
}

//-------------------------------------------------------
std::string Node::codeAfter()
{
    string s = "";

    if(hasEmitAndNotOutput && !m_emitingNode){//overwrite emit in node space in lua
        s += loadFileIntoString(PATHTOSRC"/lua_constant/gather_emit_end.lua");
    }
    return s;
}

//------------------------------------------------------------------
void Node::reloadProgram(){
    m_Program = loadFileIntoString(m_Path.c_str());
    m_timeStamp = fileTimestamp(m_Path);
    parse();
    onChange();
}

//-------------------------------------------------------
//create the tweaks by parsing the lua file.
void Node::parseTweaks()
{
    //first remove comm's
    string toParse = m_Program;
    string uncommented;
    std::regex nocomment("(--\\[\\[(.|\n)*?\\]\\]--)");
    regex_replace(std::back_inserter(uncommented),toParse.begin(),toParse.end(),nocomment,"$2");

    std::regex nocomment2("(--.*\\n)");
    string uncommented2;
    regex_replace(std::back_inserter(uncommented2),uncommented.begin(),uncommented.end(),nocomment2,"$2");
    string float_expr = "[ \\n]*(-?[0-9]*[.]?[0-9]*(?:[Ee][+-]?[0-9]+)?)[ \\n]*";
    string name_expr = "[ \\n]*[\"\']([^\'\"]*)[\"\'][ \\n]*";
    //look for node_string
    try{
        string income = uncommented2;
        string expr = string("node_string\\(") + name_expr + "," + name_expr + "\\)";
        std::regex scalarEx(expr);
        std::regex_iterator<std::string::iterator> rit ( income.begin(), income.end(), scalarEx );
        std::regex_iterator<std::string::iterator> rend;
        while(rit != rend){
            if(tweaks.find(rit->str(1))==tweaks.end())tweaks[rit->str(1)] = new TweakString(this,rit->str(1),rit->str(2));
            rit++;
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    try{
        string income = uncommented2;
        string expr = string("node_scalar\\(") + name_expr + "," + float_expr + "\\)";
        std::regex scalarEx(expr);
        std::regex_iterator<std::string::iterator> rit ( income.begin(), income.end(), scalarEx );
        std::regex_iterator<std::string::iterator> rend;
        while(rit != rend){
            if(tweaks.find(rit->str(1))==tweaks.end())tweaks[rit->str(1)] = new TweakScalar(this,rit->str(1),stof(rit->str(2)));
            rit++;
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    try{
        string income = uncommented2;
        string expr = string("node_scalar\\(") + name_expr + "," + float_expr + "," + float_expr + "," + float_expr + "\\)";
        std::regex scalarEx(expr);
        std::regex_iterator<std::string::iterator> rit ( income.begin(), income.end(), scalarEx );
        std::regex_iterator<std::string::iterator> rend;
        while(rit != rend){
            if(tweaks.find(rit->str(1))==tweaks.end())tweaks[rit->str(1)] = new TweakSlider(this,rit->str(1),stof(rit->str(2)),stof(rit->str(3)),stof(rit->str(4)));
            rit++;
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    try{
        string income = uncommented2;
        const char* expr = "node_int\\([ \\n]*[\"\']([\\w_]*)[\"\'][ \\n]*,[ \\n]*(-?[0-9]*)[ \\n]*\\)";
        std::regex scalarEx(expr);
        std::regex_iterator<std::string::iterator> rit ( income.begin(), income.end(), scalarEx );
        std::regex_iterator<std::string::iterator> rend;
        while(rit != rend){
            if(tweaks.find(rit->str(1))==tweaks.end())tweaks[rit->str(1)] = new TweakInt(this,rit->str(1),stoi(rit->str(2)));
            rit++;
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    try{
        string income = uncommented2;
        string expr = string("node_path\\(") + name_expr + "," + name_expr + "\\)";
        std::regex scalarEx(expr);
        std::regex_iterator<std::string::iterator> rit ( income.begin(), income.end(), scalarEx );
        std::regex_iterator<std::string::iterator> rend;
        while(rit != rend){
            if(tweaks.find(rit->str(1))==tweaks.end())tweaks[rit->str(1)] = new TweakPath(this,rit->str(1),rit->str(2));
            rit++;
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
}


//------------------------------------------------------------------------------------
void Node::parseInputAndOutput()
{
    string toParse = m_Program;
    string uncommented;
    std::regex nocomment("(--\\[\\[(.|\n)*?\\]\\]--)");
    regex_replace(std::back_inserter(uncommented),toParse.begin(),toParse.end(),nocomment,"$2");

    std::regex nocomment2("(--.*\\n)");
    string uncommented2;
    regex_replace(std::back_inserter(uncommented2),uncommented.begin(),uncommented.end(),nocomment2,"$2");

    try{
        string outcome = uncommented2;
        std::regex outputEx("output\\([ \n]*[\"\']([\\w_]*)[\"\'][ \n]*,(.|\n)*?\\)");
        std::smatch sm;
        while(regex_search (outcome,sm,outputEx)){
            makeNewOutput(sm[1]);
            outcome = sm.suffix().str();
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    try{
        string income = uncommented2;
        std::regex inputEx("input\\([ \n]*[\"\']([\\w_]*)[\"\'][ \n]*\\)");
        std::smatch sm;
        while(regex_search (income,sm,inputEx)){
            makeNewInput(sm[1]);
            income = sm.suffix().str();
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }
    try{
        string income = uncommented2;
        std::regex outputEx("emit");
        std::smatch sm;
        if(regex_search(income,sm,outputEx)){
            hasEmitAndNotOutput = true;
            if(!m_emitingNode)makeNewOutput("emit");
        }
    }catch (const std::regex_error& e) {
        std::cout << "regex_error caught: " << e.what() << '\n';
    }

}

//-------------------------------------------------------
//parse the code to detect input and output code.
//it also detect if emit is used for retrocompatibility.
//also look at the tweaks.
void Node::parse(){
    parseTweaks();
    parseInputAndOutput();
}


//-------------------------------------------------------
//write the master lua file by traversing the graph.
bool Node::writeMaster(ofstream& myfile)
{
    std::vector<Node*> ordered_node;
    orderNode(ordered_node);
    ForIndex(i,ordered_node.size()){
        Node* currentNode = ordered_node[i];
        myfile << currentNode->codeBefore().c_str();
        myfile << currentNode->m_Program.c_str();
        myfile << currentNode->codeAfter().c_str();

    }
    return (false); // no error
}

//-------------------------------------------------------
void Node::changePath(std::string path,Project p){
    m_Path = path;
    m_RelativePath = p.relativePath(path);
    m_Program = loadFileIntoString(m_Path.c_str());
    m_emitingNode = strcmp(m_RelativePath.c_str(),"emit.lua") == 0;

    m_timeStamp = fileTimestamp(m_Path);
    parse();
}

//-------------------------------------------------------
std::string Node::hashProgram(){
    hashwrapper* myWrapper = new md5wrapper();
    return myWrapper->getHashFromString(m_Program);
    delete myWrapper;

}

//-------------------------------------------------------
void Node::connect(Node* n,std::string outName,int pos){
    prevNamed.at(inputName[pos]) = std::make_pair(n,outName);
    n->nextNamed[outName] = this;
}

//-------------------------------------------------------
//detect if a node is an Ascendent
bool Node::isAscendent(Node* toConnect){
    std::vector<Node*> toVisit;
    toVisit.push_back(this);
    while(toVisit.size()>0){
        Node* current = toVisit.back();
        toVisit.pop_back();
        if(current == toConnect)return true;
        for( auto& val: current->prevNamed){
            Node* a = val.second.first;
            if(a != nullptr)toVisit.push_back(a);
        }
    }
    return false;
}

//-------------------------------------------------------
//test if all input are connected
bool Node::isConnectedToInput(){
    bool b = true;
    for( auto& val: prevNamed){
        Node* a = val.second.first;
        if(a == nullptr)return false;
        b = b && a->isConnectedToInput();
    }

    return b;
}


//-------------------------------------------------------
//order the node to write the master script.
void Node::orderNode(std::vector<Node*>& orderedNode){
    std::vector<Node*> toVisit;
    toVisit.push_back(this);
    std::set<Node*> visited;
    while(toVisit.size()>0){
        Node* current = toVisit.back();
        if(visited.find(current) == visited.end()){
            orderedNode.push_back(current);
            visited.insert(current);
        }
        toVisit.pop_back();
        for( auto& val: current->prevNamed){
            Node* a = val.second.first;
            if(a == nullptr)continue;
            toVisit.push_back(a);
        }
    }
    std::reverse(orderedNode.begin(),orderedNode.end());
}

//------------------------------------------------------------------
//automatically detect file changes to reload lua files.
Node::t_FileTime Node::fileTimestamp(std::string file) const
{
#ifdef WIN32
  HANDLE f = CreateFileA(file.c_str(),
    GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (f == NULL) {
    t_FileTime z;
    memset(&z, 0, sizeof(t_FileTime));
    return z;
  }
  FILETIME creation, lastaccess, lastwrite;
  GetFileTime(f, &creation, &lastaccess, &lastwrite);
  CloseHandle(f);
  return lastwrite;
#else
  struct stat st;
  int fh = open(file.c_str(), O_RDONLY);
  if (fh < 0) return false;
  fstat(fh, &st);
  close(fh);
  return st.st_mtime;
#endif
}

// -----------------------------------------------
bool Node::fileChanged(std::string file, t_FileTime& _last) const
{
#ifdef WIN32
  HANDLE f = CreateFileA(file.c_str(),
    GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (f == NULL) return false;
  FILETIME creation, lastaccess, lastwrite;
  GetFileTime(f, &creation, &lastaccess, &lastwrite);
  CloseHandle(f);
  if (lastwrite.dwHighDateTime > _last.dwHighDateTime
    || (lastwrite.dwHighDateTime == _last.dwHighDateTime
    &&  lastwrite.dwLowDateTime > _last.dwLowDateTime)
    ) {
    _last = lastwrite;
    cerr << "[script] detected change in " << file << endl;
    return true;
  }
#else
  struct stat st;
  int fh = open(file.c_str(), O_RDONLY);
  if (fh < 0) return false;
  fstat(fh, &st);
  close(fh);
  if (st.st_mtime > _last) {
    _last = st.st_mtime;
    cerr << "[script] detected change in " << file << endl;
    return true;
  }
#endif
  return false;
}


//------------------------------------------------------------------
bool Node::hasChange(){
    return fileChanged(m_Path,m_timeStamp);
}
