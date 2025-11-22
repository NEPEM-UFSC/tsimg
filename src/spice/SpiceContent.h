#pragma once
#include <string>

class SpiceContent {
public:
    SpiceContent(const std::string& tag, const std::string& content);
    const std::string& getTag() const;
    const std::string& getVariableContent() const;

private:
    std::string tag;
    std::string variableContent;
};
