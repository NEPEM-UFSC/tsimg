#include "tsimg_test_framework.h"
#include "utils/JsonUtils.h"
#include "tsimg_gif.h"
#include <fstream>

// Teste para JSON inválido
bool test_invalid_json() {
    std::string tempPath = tsimg::test::TestUtils::createTempFilePath("invalid_config", "json");
    
    // Criar arquivo JSON inválido
    {
        std::ofstream out(tempPath);
        out << "{ \"key\": \"value\", "; // JSON incompleto
    }
    
    bool exceptionThrown = false;
    try {
        tsimg::utils::read_json_file(tempPath, false);
    } catch (const std::exception&) {
        exceptionThrown = true;
    }
    
    tsimg::test::TestUtils::removeFile(tempPath);
    
    TSIMG_ASSERT(exceptionThrown);
    return true;
}

// Teste para arquivo de imagem corrompido (GIF)
bool test_corrupted_gif_image() {
    std::string tempImagePath = tsimg::test::TestUtils::createTempFilePath("corrupted_image", "jpg");
    std::string tempGifPath = tsimg::test::TestUtils::createTempFilePath("output", "gif");
    
    // Criar arquivo de imagem corrompido (conteúdo aleatório)
    {
        std::ofstream out(tempImagePath);
        out << "This is not a valid image file content.";
    }
    
    std::vector<std::string> images = {tempImagePath};
    
    // Tentar criar GIF com imagem corrompida
    // createGif deve retornar false e não crashar
    bool result = createGif(tempGifPath, images, false, 100, true, 75);
    
    tsimg::test::TestUtils::removeFile(tempImagePath);
    if (tsimg::test::TestUtils::fileExists(tempGifPath)) {
        tsimg::test::TestUtils::removeFile(tempGifPath);
    }
    
    TSIMG_ASSERT_EQ(false, result);
    return true;
}

// Função para registrar os testes de falha
void registerFailureTests() {
    auto& runner = tsimg::test::TestRunner::getInstance();
    
    runner.addFailureTest("InvalidJsonConfig", test_invalid_json);
    runner.addFailureTest("CorruptedGifImage", test_corrupted_gif_image);
}
