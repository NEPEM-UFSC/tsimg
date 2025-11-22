#include "SpiceContent.h"

SpiceContent::SpiceContent(const std::string& tag, const std::string& content)
    : tag(tag), variableContent(content) {}

const std::string& SpiceContent::getTag() const {
    return tag;
}

const std::string& SpiceContent::getVariableContent() const {
    return variableContent;
}
