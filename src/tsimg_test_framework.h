#ifndef TSIMG_TEST_FRAMEWORK_H
#define TSIMG_TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>

// Macros de Assert
#define TSIMG_ASSERT(condition) \
    if (!(condition)) { \
        std::cerr << "Assertion Failed: (" << #condition << ") is false in " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    }

#define TSIMG_ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        std::cerr << "Assertion Failed: Expected (" << #expected << ") to be equal to Actual (" << #actual \
                  << "), but Expected was: " << (expected) << " and Actual was: " << (actual) \
                  << " in " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    }

#define TSIMG_ASSERT_THROWS(expression, ExceptionType) \
    try { \
        expression; \
        std::cerr << "Assertion Failed: Expected exception " << #ExceptionType << " not thrown in " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    } catch (const ExceptionType&) { \
    } catch (...) { \
        std::cerr << "Assertion Failed: Caught unexpected exception type in " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    }

namespace tsimg::test {

    class TestUtils {
    public:
        static std::string createTempFilePath(const std::string& prefix, const std::string& extension) {
            auto tempDir = std::filesystem::temp_directory_path();
            auto tempFile = tempDir / (prefix + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "." + extension);
            return tempFile.string();
        }
        
        static bool removeFile(const std::string& filePath) {
            try {
                return std::filesystem::remove(filePath);
            } catch (const std::exception& e) {
                std::cerr << "Erro ao remover arquivo: " << e.what() << std::endl;
                return false;
            }
        }
        
        static bool fileExists(const std::string& filePath) {
            return std::filesystem::exists(filePath);
        }
        
        static std::vector<std::string> getTestImagePaths() {
            std::vector<std::string> imagePaths;
            try {
                std::vector<std::filesystem::path> searchPaths = {
                    std::filesystem::current_path().parent_path() / "test" / "images",
                    std::filesystem::current_path() / "test" / "images",
                    std::filesystem::current_path().parent_path() / "test" / "data",
                    std::filesystem::current_path() / "test" / "data"
                };
                
                for (const auto& searchPath : searchPaths) {
                    if (!std::filesystem::exists(searchPath)) continue;
                    
                    int count = 0;
                    for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                        if (!entry.is_regular_file()) continue;
                        std::string ext = entry.path().extension().string();
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".gif") {
                            imagePaths.push_back(entry.path().string());
                            count++;
                            if (count >= 5) break;
                        }
                    }
                    if (!imagePaths.empty()) break;
                }
            } catch (...) {}
            return imagePaths;
        }
    };

    class TestRunner {
    private:
        std::map<std::string, std::function<bool()>> tests;
        std::map<std::string, std::string> testGroups;
        
        TestRunner() = default;
        
    public:
        static TestRunner& getInstance() {
            static TestRunner instance;
            return instance;
        }
        
        void addUnitTest(const std::string& name, std::function<bool()> testFunc) {
            tests[name] = testFunc;
            testGroups[name] = "unit";
        }
        
        void addIntegrationTest(const std::string& name, std::function<bool()> testFunc) {
            tests[name] = testFunc;
            testGroups[name] = "integration";
        }
        
        void addPerformanceTest(const std::string& name, std::function<bool()> testFunc) {
            tests[name] = testFunc;
            testGroups[name] = "performance";
        }

        void addFailureTest(const std::string& name, std::function<bool()> testFunc) {
            tests[name] = testFunc;
            testGroups[name] = "failure";
        }
        
        bool runTest(const std::string& name) {
            if (tests.find(name) == tests.end()) return false;
            std::cout << "Executando teste: " << name << std::endl;
            bool result = false;
            try {
                result = tests[name]();
            } catch (const std::exception& e) {
                std::cerr << "Exceção no teste " << name << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Exceção desconhecida no teste " << name << std::endl;
            }
            if (result) std::cout << "PASSOU: " << name << std::endl;
            else std::cerr << "FALHOU: " << name << std::endl;
            return result;
        }
        
        void runAll() {
            std::cout << "=== Executando todos os testes ===" << std::endl;
            int passed = 0, total = 0;
            for (const auto& [name, _] : tests) {
                ++total;
                if (runTest(name)) ++passed;
            }
            std::cout << "=== Resultado final: " << passed << "/" << total 
                      << " testes passaram (" << (total > 0 ? (passed * 100 / total) : 0) 
                      << "%) ===" << std::endl;
        }

        void runGroup(const std::string& groupPattern) {
            std::cout << "=== Executando testes do grupo: " << groupPattern << " ===" << std::endl;
            int passed = 0, total = 0;
            for (const auto& [name, _] : tests) {
                if (testGroups[name].find(groupPattern) != std::string::npos) {
                    ++total;
                    if (runTest(name)) ++passed;
                }
            }
            std::cout << "=== Resultado do grupo " << groupPattern << ": " << passed << "/" 
                      << total << " testes passaram (" << (total > 0 ? (passed * 100 / total) : 0) 
                      << "%) ===" << std::endl;
        }
    };
}

#endif // TSIMG_TEST_FRAMEWORK_H
