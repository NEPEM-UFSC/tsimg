#ifndef TSIMG_TEST_H
#define TSIMG_TEST_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <future>
#include <thread>
#include <algorithm>

// Definições para testes
#define TSIMG_ASSERT(condition) \
    if (!(condition)) { \
        std::cerr << "Assertion falhou: " << #condition << " em " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    }

#define TSIMG_ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        std::cerr << "Assertion falhou: " << #expected << " == " << #actual << ", valores: " \
                  << (expected) << " != " << (actual) << " em " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    }

// Declaração antecipada de classes
class VariableContent;
class ImageList;

// Forward declarations das classes principais do projeto
class SPICEBuilder;
class TemplateWriter;

// Função mock para codificação Base64
std::string encodeImageToBase64(const std::string& imagePath, bool debug);

// Função mock para criação de GIF
bool createGif(const std::string& output_filename, 
               const std::vector<std::string>& image_paths, 
               bool debug, 
               int delay = 100, 
               bool loop = true,
               int quality = 75);

// Classe para variáveis de conteúdo
class VariableContent {
private:
    std::string tag;
    std::string content;
    
public:
    VariableContent(const std::string& tag, const std::string& content)
        : tag(tag), content(content) {}
    
    const std::string& getTag() const {
        return tag;
    }
    
    const std::string& getVariableContent() const {
        return content;
    }
};

// Classe ImageList
class ImageList {
private:
    std::vector<std::string> images;
    
public:
    void addImage(const std::string& imagePath) {
        images.push_back(imagePath);
    }
    
    const std::vector<std::string>& getImages() const {
        return images;
    }
};

// Namespaces para testes
namespace tsimg {
    namespace test {
        
        // Utilitários para testes
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
                // Procura imagens na pasta de testes ou no diretório atual
                std::vector<std::string> imagePaths;
                
                try {
                    // Busca primordialmente no diretório de teste/images
                    std::vector<std::filesystem::path> searchPaths = {
                        std::filesystem::current_path().parent_path() / "test" / "images",     // Caminho absoluto a partir do diretório build
                        std::filesystem::current_path() / "test" / "images",                  // Caminho relativo a partir do diretório atual
                        std::filesystem::current_path().parent_path() / "test" / "data",
                        std::filesystem::current_path() / "test" / "data",
                        std::filesystem::current_path().parent_path() / "data" / "test",
                        std::filesystem::current_path().parent_path() / "example",
                        std::filesystem::current_path().parent_path() / "_example",
                        std::filesystem::current_path()
                    };
                    
                    for (const auto& searchPath : searchPaths) {
                        if (!std::filesystem::exists(searchPath)) {
                            std::cout << "Diretório não encontrado: " << searchPath.string() << std::endl;
                            continue;
                        }
                        
                        std::cout << "Buscando imagens em: " << searchPath.string() << std::endl;
                        int count = 0;
                        
                        for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                            if (!entry.is_regular_file()) continue;
                            
                            std::string ext = entry.path().extension().string();
                            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                            
                            if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".gif") {
                                imagePaths.push_back(entry.path().string());
                                count++;
                                std::cout << "Imagem encontrada: " << entry.path().string() << std::endl;
                                if (count >= 5) break; // Limite de 5 imagens para não sobrecarregar os testes
                            }
                        }
                        
                        if (!imagePaths.empty()) {
                            std::cout << "Total de " << imagePaths.size() << " imagens encontradas." << std::endl;
                            break;
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Erro ao procurar imagens de teste: " << e.what() << std::endl;
                }
                
                return imagePaths;
            }
        };
        
        // Executor de testes
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
            
            bool runTest(const std::string& name) {
                if (tests.find(name) == tests.end()) {
                    std::cerr << "Teste não encontrado: " << name << std::endl;
                    return false;
                }
                
                std::cout << "Executando teste: " << name << std::endl;
                bool result = false;
                
                try {
                    result = tests[name]();
                } catch (const std::exception& e) {
                    std::cerr << "Exceção no teste " << name << ": " << e.what() << std::endl;
                    return false;
                } catch (...) {
                    std::cerr << "Exceção desconhecida no teste " << name << std::endl;
                    return false;
                }
                
                if (result) {
                    std::cout << "PASSOU: " << name << std::endl;
                } else {
                    std::cerr << "FALHOU: " << name << std::endl;
                }
                
                return result;
            }
            
            void runAll() {
                std::cout << "=== Executando todos os testes ===" << std::endl;
                int passed = 0;
                int total = 0;
                
                for (const auto& [name, _] : tests) {
                    ++total;
                    if (runTest(name)) {
                        ++passed;
                    }
                }
                
                std::cout << "=== Resultado final: " << passed << "/" << total 
                          << " testes passaram (" << (total > 0 ? (passed * 100 / total) : 0) 
                          << "%) ===" << std::endl;
            }
            
            void runGroup(const std::string& groupPattern) {
                std::cout << "=== Executando testes do grupo: " << groupPattern << " ===" << std::endl;
                int passed = 0;
                int total = 0;
                
                for (const auto& [name, _] : tests) {
                    if (name.find(groupPattern) != std::string::npos || 
                        testGroups[name].find(groupPattern) != std::string::npos) {
                        ++total;
                        if (runTest(name)) {
                            ++passed;
                        }
                    }
                }
                
                std::cout << "=== Resultado do grupo " << groupPattern << ": " << passed << "/" 
                          << total << " testes passaram (" << (total > 0 ? (passed * 100 / total) : 0) 
                          << "%) ===" << std::endl;
            }
        };
    } // namespace test
    
    namespace utils {
        // Mock da classe ImageValidator para testes
        class ImageValidator {
        public:
            static bool validateImagePath(const std::string& path, bool debug) {
                if (debug) {
                    std::cout << "Validando imagem: " << path << std::endl;
                }
                
                return std::filesystem::exists(path);
            }
        };
        
        // Mock da classe FileHandler para testes
        class FileHandler {
        public:
            static bool isFileReadable(const std::string& path) {
                std::ifstream file(path);
                return file.good();
            }
        };
        
        // Mock da classe ImageProcessor para testes
        class ImageProcessor {
        public:
            static std::vector<std::future<bool>> processImagesAsync(
                const std::vector<std::string>& imagePaths, bool debug) {
                std::vector<std::future<bool>> futures;
                
                for (const auto& path : imagePaths) {
                    futures.push_back(std::async(std::launch::async, [path, debug]() {
                        if (debug) {
                            std::cout << "Processando imagem: " << path << std::endl;
                        }
                        // Simulação de processamento
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        return true;
                    }));
                }
                
                return futures;
            }
        };
    } // namespace utils
} // namespace tsimg

// Função mock para criação de GIF - implementação
inline bool createGif(const std::string& output_filename, 
                     const std::vector<std::string>& image_paths, 
                     bool debug, 
                     int delay, 
                     bool loop,
                     int quality) {
    if (debug) {
        std::cout << "Criando GIF: " << output_filename << std::endl;
    }
    
    if (image_paths.empty()) {
        std::cerr << "Nenhuma imagem fornecida para o GIF" << std::endl;
        return false;
    }
    
    // Simulação de criação de GIF
    std::ofstream outFile(output_filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Falha ao criar arquivo GIF: " << output_filename << std::endl;
        return false;
    }
    
    // Escrever alguns bytes no arquivo para simular um GIF
    outFile << "GIF89a";
    outFile.close();
    
    return true;
}

// Função mock para codificação Base64 - implementação
inline std::string encodeImageToBase64(const std::string& imagePath, bool debug) {
    if (debug) {
        std::cout << "Codificando imagem em Base64: " << imagePath << std::endl;
    }
    
    // Verificar se o arquivo existe
    if (!std::filesystem::exists(imagePath)) {
        std::cerr << "Arquivo não encontrado: " << imagePath << std::endl;
        return "";
    }
    
    // Simulação de codificação Base64 (simplificada)
    return "data:image/jpeg;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7";
}

// Mock de SPICEBuilder
class SPICEBuilder {
private:
    std::string title;
    bool debug;
    std::map<std::string, std::shared_ptr<ImageList>> imageLists;
    std::vector<VariableContent> contents;
    std::vector<std::string> labels;
    std::string authorImageBase64;
    std::string templatePath;

public:
    SPICEBuilder(const std::string& title, bool debug) : title(title), debug(debug) {
        if (debug) {
            std::cout << "SPICEBuilder criado com título: " << title << std::endl;
        }
    }
    
    void addTitle(const std::string& newTitle) {
        title = newTitle;
        addContent("SPICE_TITLE", newTitle);
    }
    
    void addContent(const std::string& tag, const std::string& content) {
        contents.push_back(VariableContent(tag, content));
    }
    
    void addImage(const std::string& imagePath) {
        if (!imageLists.count("SPICE_IMAGES")) {
            imageLists["SPICE_IMAGES"] = std::make_shared<ImageList>();
        }
        
        imageLists["SPICE_IMAGES"]->addImage(imagePath);
    }
    
    void addImageToList(const std::string& listTag, const std::string& imagePath) {
        if (!imageLists.count(listTag)) {
            imageLists[listTag] = std::make_shared<ImageList>();
        }
        
        imageLists[listTag]->addImage(imagePath);
    }
    
    void addImagesAsync(const std::vector<std::string>& imagePaths) {
        for (const auto& path : imagePaths) {
            addImage(path);
        }
    }
    
    void addLabels(const std::vector<std::string>& newLabels) {
        labels.insert(labels.end(), newLabels.begin(), newLabels.end());
    }
    
    void generateLabelsFromImages() {
        // Simulação
        auto it = imageLists.find("SPICE_IMAGES");
        if (it != imageLists.end() && it->second) {
            int index = 1;
            for (const auto& _ : it->second->getImages()) {
                (void)_; // unused
                labels.push_back("Imagem " + std::to_string(index++));
            }
        }
    }
    
    void setHelp(const std::string& text, const std::string& link, const std::string& badgeUrl) {
        addContent("SPICE_HELP_TEXT", text);
        addContent("SPICE_HELP_LINK", link);
        addContent("SPICE_HELP_BADGE", badgeUrl);
    }
    
    void setAuthorImage(const std::string& imagePath) {
        authorImageBase64 = encodeImageToBase64(imagePath, debug);
    }
    
    void setTemplate(const std::string& path) {
        templatePath = path;
    }
    
    const std::map<std::string, std::shared_ptr<ImageList>>& getImageLists() const {
        return imageLists;
    }
    
    const std::vector<VariableContent>& getContents() const {
        return contents;
    }
    
    const std::vector<std::string>& getLabels() const {
        return labels;
    }
    
    const std::string& getAuthorImageBase64() const {
        return authorImageBase64;
    }
    
    const std::string& getTemplatePath() const {
        return templatePath;
    }
    
    const std::string& getTitle() const {
        return title;
    }
};

// Mock de TemplateWriter
class TemplateWriter {
private:
    std::string templatePath;
    bool debug;
    
public:
    TemplateWriter(const std::string& templatePath, bool debug)
        : templatePath(templatePath), debug(debug) {
        if (debug) {
            std::cout << "TemplateWriter criado com template: " << templatePath << std::endl;
        }
    }
    
    bool writeToFile(
        const std::string& outputPath,
        const std::vector<VariableContent>& contents,
        const std::map<std::string, std::shared_ptr<ImageList>>& imageLists,
        const std::vector<std::string>& labels,
        const std::string& authorImageBase64
    ) {
        if (debug) {
            std::cout << "Escrevendo arquivo: " << outputPath << std::endl;
        }
        
        // Simulação de escrita para arquivo HTML
        std::ofstream outFile(outputPath);
        if (!outFile) {
            if (debug) {
                std::cerr << "Falha ao criar arquivo: " << outputPath << std::endl;
            }
            return false;
        }
        
        outFile << "<!DOCTYPE html>\n<html>\n<head>\n";
        outFile << "<title>Teste de Integração</title>\n";
        outFile << "</head>\n<body>\n";
        
        // Adicionar conteúdos
        for (const auto& content : contents) {
            outFile << "<div data-tag=\"" << content.getTag() << "\">"
                    << content.getVariableContent() << "</div>\n";
        }
        
        // Adicionar imagens
        for (const auto& [tag, imageList] : imageLists) {
            outFile << "<div data-tag=\"" << tag << "\">\n";
            for (const auto& imagePath : imageList->getImages()) {
                outFile << "  <img src=\"data:image/jpeg;base64,...\" alt=\"" << imagePath << "\">\n";
            }
            outFile << "</div>\n";
        }
        
        // Adicionar labels
        outFile << "<div data-tag=\"SPICE_LABELS\">\n";
        for (const auto& label : labels) {
            outFile << "  <span>" << label << "</span>\n";
        }
        outFile << "</div>\n";
        
        // Adicionar imagem do autor
        if (!authorImageBase64.empty()) {
            outFile << "<div data-tag=\"SPICE_AUTHOR_IMAGE\">\n";
            outFile << "  <img src=\"" << authorImageBase64 << "\" alt=\"Author\">\n";
            outFile << "</div>\n";
        }
        
        outFile << "</body>\n</html>";
        outFile.close();
        
        return true;
    }
};

// Função para teste de existência de arquivo
bool test_file_exists() {
    std::string testFilePath = tsimg::test::TestUtils::createTempFilePath("test_file", "txt");
    
    // Criar arquivo
    {
        std::ofstream outFile(testFilePath);
        outFile << "Test content";
    }
    
    // Verificar que o arquivo existe
    bool exists = tsimg::utils::FileHandler::isFileReadable(testFilePath);
    
    // Limpar
    tsimg::test::TestUtils::removeFile(testFilePath);
    
    return exists;
}

#endif // TSIMG_TEST_H