#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <optional>
#include <nlohmann/json.hpp>

namespace tsimg {

/**
 * @brief Classe para gerenciamento de configurações do TSIMG
 */
class ConfigManager {
public:
    // Singleton para acesso global
    static ConfigManager& getInstance();

    // Métodos de configuração básica
    void setDefaultOutputFormat(const std::string& format);
    void setDefaultTemplate(const std::string& templatePath);
    void setDefaultOutputDirectory(const std::string& directory);
    void setDefaultDebugMode(bool debugMode);
    void setDefaultImageQuality(int quality);
    void setDefaultGifDelay(int delay);
    void setDefaultGifLoop(bool loop);
    
    std::string getDefaultOutputFormat() const;
    std::string getDefaultTemplate() const;
    std::string getDefaultOutputDirectory() const;
    bool getDefaultDebugMode() const;
    int getDefaultImageQuality() const;
    int getDefaultGifDelay() const;
    bool getDefaultGifLoop() const;
    
    bool loadFromFile(const std::string& filePath);
    bool saveToFile(const std::string& filePath) const;
    bool loadFromJson(const nlohmann::json& jsonConfig);
    nlohmann::json saveToJson() const;
    bool saveToJsonFile(const std::string& filePath) const;
    void savePreset(const std::string& name, const nlohmann::json& config);
    nlohmann::json getPreset(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    bool deletePreset(const std::string& name);
    void setValue(const std::string& key, const nlohmann::json& value);
    nlohmann::json getValue(const std::string& key) const;
    bool hasKey(const std::string& key) const;
    void removeKey(const std::string& key);
    
    // Reset de configurações
    void resetToDefaults();

private:
    // Construtor privado para singleton
    ConfigManager();
    ~ConfigManager();
    
    // Prevenir cópia/atribuição
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Inicializar com valores padrão
    void initializeDefaults();
    
    // Armazenamento de configurações
    nlohmann::json config;
    std::map<std::string, nlohmann::json> presets;
    
    // Caminhos de arquivos
    std::string configFilePath;
};

/**
 * @brief Interface para estratégias de processamento de imagem
 */
class IProcessingStrategy {
public:
    virtual ~IProcessingStrategy() = default;
    virtual bool process(const std::string& inputPath, const std::string& outputPath) = 0;
    virtual std::string getName() const = 0;
    virtual void configure(const nlohmann::json& options) = 0;
};

/**
 * @brief Gerenciador de estratégias de processamento
 */
class ProcessingStrategyManager {
public:
    static ProcessingStrategyManager& getInstance();
    
    void registerStrategy(const std::string& name, std::unique_ptr<IProcessingStrategy> strategy);
    IProcessingStrategy* getStrategy(const std::string& name);
    std::vector<std::string> getAvailableStrategies() const;
    
private:
    ProcessingStrategyManager();
    ~ProcessingStrategyManager();
    
    ProcessingStrategyManager(const ProcessingStrategyManager&) = delete;
    ProcessingStrategyManager& operator=(const ProcessingStrategyManager&) = delete;
    
    std::map<std::string, std::unique_ptr<IProcessingStrategy>> strategies;
};

/**
 * @brief Classe para perfil do usuário
 */
class UserProfile {
public:
    UserProfile(const std::string& name);
    ~UserProfile();
    
    // Métodos de perfil
    void setName(const std::string& name);
    void setAuthorImage(const std::string& imagePath);
    void setPreferredTemplate(const std::string& templatePath);
    void setPreferredSettings(const nlohmann::json& settings);
    
    // Getters
    std::string getName() const;
    std::string getAuthorImage() const;
    std::string getPreferredTemplate() const;
    nlohmann::json getPreferredSettings() const;
    
    // Persistência
    bool saveToFile(const std::string& filePath) const;
    static std::unique_ptr<UserProfile> loadFromFile(const std::string& filePath);
    
private:
    std::string name;
    std::string authorImage;
    std::string preferredTemplate;
    nlohmann::json preferredSettings;
};

/**
 * @brief Gerenciador de perfis de usuário
 */
class UserProfileManager {
public:
    static UserProfileManager& getInstance();
    
    void addProfile(std::unique_ptr<UserProfile> profile);
    UserProfile* getProfile(const std::string& name);
    std::vector<std::string> getProfileNames() const;
    void setActiveProfile(const std::string& name);
    UserProfile* getActiveProfile();
    bool removeProfile(const std::string& name);
    
    // Persistência
    bool loadProfiles(const std::string& directory);
    bool saveProfiles(const std::string& directory) const;
    
private:
    UserProfileManager();
    ~UserProfileManager();
    
    UserProfileManager(const UserProfileManager&) = delete;
    UserProfileManager& operator=(const UserProfileManager&) = delete;
    
    std::map<std::string, std::unique_ptr<UserProfile>> profiles;
    std::string activeProfileName;
};

} // namespace tsimg