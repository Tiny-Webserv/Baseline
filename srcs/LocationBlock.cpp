#include "LocationBlock.hpp"

LocationBlock::LocationBlock() {
    _UploadPass = "";
    _LocationTarget = "";
    _Root = "";
    _AutoIndex = -1;
}

LocationBlock::~LocationBlock() {}

void LocationBlock::SetLocationTarget(std::string locationtarget) {
    this->_LocationTarget = locationtarget;
}

void LocationBlock::SetRoot(std::string root) { this->_Root = root; }

void LocationBlock::SetUploadPass(std::string uploadpass) {
    this->_UploadPass = uploadpass;
}

void LocationBlock::SetReturn(std::pair<int, std::string> ret) {
    this->_Return = ret;
}

void LocationBlock::SetAutoIndex(int autoindex) {
    this->_AutoIndex = autoindex;
}

//// vector
void LocationBlock::AddLimitExcept(std::string limitexcept) {
    this->_LimitExcept.push_back(limitexcept);
}

void LocationBlock::AddIndex(std::string index) {
    this->_Index.push_back(index);
}

void LocationBlock::SetLimitExcept(std::vector<std::string> limitexcept){
	this->_LimitExcept = limitexcept;
}
void LocationBlock::SetIndex(std::vector<std::string> index) {
	this->_Index = index;
}
////

////////////////////Getter/////////////
std::string LocationBlock::GetLocationTarget() { return this->_LocationTarget; }
std::string LocationBlock::GetRoot() { return this->_Root; }
std::string LocationBlock::GetUploadPass() { return this->_UploadPass; }
std::pair<int, std::string> LocationBlock::GetReturn() { return this->_Return; }
std::vector<std::string> LocationBlock::GetLimitExcept() {
    return this->_LimitExcept;
}
std::vector<std::string> LocationBlock::GetIndex() { return this->_Index; }
int LocationBlock::GetAutoIndex() { return this->_AutoIndex; }