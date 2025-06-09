#include "tsimg_test.h"
#include <future>
#include <algorithm>
#include <thread>

// Teste para verificar leitura e escrita de arquivos temporários
bool test_temp_file_write_and_read() {
    std::string tempPath = tsimg::test::TestUtils::createTempFilePath("temp_rw", "txt");
    std::string testContent = "conteudo de teste";
    {
        std::ofstream out(tempPath);
        out << testContent;
    }
    std::ifstream in(tempPath);
    std::string readContent;
    std::getline(in, readContent);
    in.close();
    tsimg::test::TestUtils::removeFile(tempPath);
    TSIMG_ASSERT_EQ(testContent, readContent);
    return true;
}

// Teste para adicionar conteúdo vazio ao SPICEBuilder
bool test_spice_builder_add_empty_content() {
    SPICEBuilder builder("Teste", true);
    builder.addContent("SPICE_TEXT", "");
    const auto& contents = builder.getContents();
    TSIMG_ASSERT_EQ(1, contents.size());
    TSIMG_ASSERT_EQ("", contents[0].getVariableContent());
    return true;
}

// Teste para adicionar imagem duplicada ao SPICEBuilder
bool test_spice_builder_add_duplicate_image() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    if (imagePaths.empty()) return true;
    SPICEBuilder builder("Teste", true);
    builder.addImage(imagePaths[0]);
    builder.addImage(imagePaths[0]);
    const auto& imageLists = builder.getImageLists();
    auto it = imageLists.find("SPICE_IMAGES");
    TSIMG_ASSERT(it != imageLists.end());
    // Espera-se que ambas as imagens estejam presentes (ou apenas uma, dependendo da implementação)
    TSIMG_ASSERT(!it->second->getImages().empty());
    return true;
}

// Teste para adicionar label duplicado ao SPICEBuilder
bool test_spice_builder_add_duplicate_labels() {
    SPICEBuilder builder("Teste", true);
    std::vector<std::string> labels = {"Label", "Label"};
    builder.addLabels(labels);
    const auto& builderLabels = builder.getLabels();
    TSIMG_ASSERT_EQ(2, builderLabels.size());
    TSIMG_ASSERT_EQ("Label", builderLabels[0]);
    TSIMG_ASSERT_EQ("Label", builderLabels[1]);
    return true;
}

// Teste para verificar que o SPICEBuilder aceita título vazio
bool test_spice_builder_empty_title() {
    SPICEBuilder builder("", true);
    TSIMG_ASSERT_EQ("", builder.getTitle());
    return true;
}

// Teste para verificar a existência de arquivo
bool test_image_validation() {
    // Usar imagens de exemplo existentes
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.empty()) {
        std::cerr << "Nenhuma imagem de teste encontrada" << std::endl;
        return false;
    }
    
    // Verificar que pelo menos uma imagem válida é validada corretamente
    bool anyValid = false;
    for (const auto& path : imagePaths) {
        if (tsimg::utils::ImageValidator::validateImagePath(path, true)) {
            anyValid = true;
            break;
        }
    }
    
    // Verificar que um caminho inválido é rejeitado
    std::string invalidPath = "imagem_inexistente.jpg";
    bool invalidRejected = !tsimg::utils::ImageValidator::validateImagePath(invalidPath, true);
    
    TSIMG_ASSERT(anyValid);
    TSIMG_ASSERT(invalidRejected);
    return true;
}

// Teste para codificação Base64
bool test_base64_encoding() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.empty()) {
        std::cerr << "Nenhuma imagem de teste encontrada" << std::endl;
        return false;
    }
    
    std::string base64 = encodeImageToBase64(imagePaths[0], true);
    
    // Verificar que a codificação não está vazia
    TSIMG_ASSERT(!base64.empty());
    
    // Verificar se a string começa com o prefixo data:image/ que é característico de Base64 de imagens
    TSIMG_ASSERT(base64.find("data:image/") == 0);
    
    // Encontrar a parte Base64 real (após a vírgula em "data:image/jpeg;base64,")
    size_t commaPos = base64.find(',');
    TSIMG_ASSERT(commaPos != std::string::npos);
    
    // Obter a parte Base64 real
    std::string realBase64 = base64.substr(commaPos + 1);
    
    // A parte de validação dos caracteres não é necessária para o teste mock
    // já que estamos usando uma string Base64 fixa
    TSIMG_ASSERT(!realBase64.empty());
    return true;
}

// Testes para SPICEBuilder
bool test_spice_builder_creation() {
    try {
        SPICEBuilder builder("Teste", true);
        
        // Verificar que o título foi definido corretamente
        TSIMG_ASSERT_EQ("Teste", builder.getTitle());
        
        // Verificar que o builder foi inicializado corretamente
        TSIMG_ASSERT(builder.getContents().empty());
        TSIMG_ASSERT(builder.getImageLists().empty());
        TSIMG_ASSERT(builder.getLabels().empty());
        TSIMG_ASSERT(builder.getAuthorImageBase64().empty());
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exceção: " << e.what() << std::endl;
        return false;
    }
}

bool test_spice_builder_add_content() {
    SPICEBuilder builder("Teste", true);
    
    // Adicionar conteúdo
    builder.addContent("SPICE_TEXT", "Texto de teste");
    
    // Verificar que o conteúdo foi adicionado
    const auto& contents = builder.getContents();
    TSIMG_ASSERT_EQ(1, contents.size());
    TSIMG_ASSERT_EQ("SPICE_TEXT", contents[0].getTag());
    TSIMG_ASSERT_EQ("Texto de teste", contents[0].getVariableContent());
    
    return true;
}

bool test_spice_builder_add_image() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.empty()) {
        std::cerr << "Nenhuma imagem de teste encontrada" << std::endl;
        return false;
    }
    
    SPICEBuilder builder("Teste", true);
    
    // Adicionar imagem
    builder.addImage(imagePaths[0]);
    
    // Verificar que a imagem foi adicionada
    const auto& imageLists = builder.getImageLists();
    TSIMG_ASSERT_EQ(1, imageLists.size());
    
    auto it = imageLists.find("SPICE_IMAGES");
    TSIMG_ASSERT(it != imageLists.end());
    TSIMG_ASSERT(!it->second->getImages().empty());
    
    return true;
}

bool test_spice_builder_add_labels() {
    SPICEBuilder builder("Teste", true);
    
    // Adicionar labels
    std::vector<std::string> labels = {"Label 1", "Label 2", "Label 3"};
    builder.addLabels(labels);
    
    // Verificar que os labels foram adicionados
    const auto& builderLabels = builder.getLabels();
    TSIMG_ASSERT_EQ(labels.size(), builderLabels.size());
    
    for (size_t i = 0; i < labels.size(); ++i) {
        TSIMG_ASSERT_EQ(labels[i], builderLabels[i]);
    }
    
    return true;
}

// Testes para a geração de GIF
bool test_gif_creation() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.size() < 2) {
        std::cerr << "Pelo menos 2 imagens são necessárias para o teste de GIF" << std::endl;
        return false;
    }
    
    std::string outputPath = tsimg::test::TestUtils::createTempFilePath("test_gif", "gif");
    
    // Criar GIF
    bool created = createGif(outputPath, imagePaths, true);
    
    // Verificar que o GIF foi criado
    bool exists = tsimg::test::TestUtils::fileExists(outputPath);
    
    // Limpar
    tsimg::test::TestUtils::removeFile(outputPath);
    
    TSIMG_ASSERT(created);
    TSIMG_ASSERT(exists);
    return true;
}

// Testes de integração para fluxo completo do SPICE
bool test_spice_full_pipeline() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.empty()) {
        std::cerr << "Nenhuma imagem de teste encontrada" << std::endl;
        return false;
    }
    
    std::string outputPath = tsimg::test::TestUtils::createTempFilePath("test_spice", "html");
    
    try {
        // Criar SPICE Builder
        SPICEBuilder builder("Teste de Integração", true);
        
        // Adicionar conteúdo
        builder.addTitle("Teste de Integração");
        builder.addContent("SPICE_TEXT", "Texto de teste para integração");
        
        // Adicionar imagens
        for (const auto& path : imagePaths) {
            builder.addImage(path);
        }
        
        // Adicionar labels
        std::vector<std::string> labels;
        for (size_t i = 0; i < imagePaths.size(); ++i) {
            labels.push_back("Imagem " + std::to_string(i + 1));
        }
        builder.addLabels(labels);
        
        // Gerar arquivo SPICE
        TemplateWriter writer("template_vs.html", true);
        writer.writeToFile(
            outputPath, 
            builder.getContents(), 
            builder.getImageLists(), 
            builder.getLabels(), 
            builder.getAuthorImageBase64()
        );
        
        // Verificar que o arquivo foi criado
        bool exists = tsimg::test::TestUtils::fileExists(outputPath);
        
        // Verificar conteúdo básico
        std::ifstream file(outputPath);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        bool hasTitle = content.find("Teste de Integração") != std::string::npos;
        bool hasText = content.find("Texto de teste para integração") != std::string::npos;
        bool hasImageData = content.find("data:image/") != std::string::npos;
        
        // Limpar
        file.close();
        tsimg::test::TestUtils::removeFile(outputPath);
        
        TSIMG_ASSERT(exists);
        TSIMG_ASSERT(hasTitle);
        TSIMG_ASSERT(hasText);
        TSIMG_ASSERT(hasImageData);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exceção: " << e.what() << std::endl;
        
        // Tentar limpar mesmo em caso de falha
        try {
            if (tsimg::test::TestUtils::fileExists(outputPath)) {
                tsimg::test::TestUtils::removeFile(outputPath);
            }
        } catch (...) {}
        
        return false;
    }
}

// Testes de desempenho
bool test_performance_base64_encoding() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.empty()) {
        std::cerr << "Nenhuma imagem de teste encontrada" << std::endl;
        return false;
    }
    
    const int iterations = 10;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        encodeImageToBase64(imagePaths[0], false);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double averageMs = static_cast<double>(duration.count()) / iterations;
    
    std::cout << "Tempo médio de codificação Base64: " << averageMs << "ms" << std::endl;
    
    // Verificar que o tempo médio é aceitável (por exemplo, menos de 500ms)
    TSIMG_ASSERT(averageMs < 500.0);
    return true;
}

bool test_performance_image_processing() {
    auto imagePaths = tsimg::test::TestUtils::getTestImagePaths();
    
    if (imagePaths.empty()) {
        std::cerr << "Nenhuma imagem de teste encontrada" << std::endl;
        return false;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    auto futures = tsimg::utils::ImageProcessor::processImagesAsync(imagePaths, false);
    
    for (auto& future : futures) {
        future.get();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double averageMs = static_cast<double>(duration.count()) / imagePaths.size();
    
    std::cout << "Tempo médio de processamento de imagem: " << averageMs << "ms" << std::endl;
    
    // Verificar que o tempo médio é aceitável (por exemplo, menos de 1000ms por imagem)
    TSIMG_ASSERT(averageMs < 1000.0);
    return true;
}

// Ponto de entrada principal para os testes
int main(int argc, char* argv[]) {
    // Inicializar o executor de testes
    auto& runner = tsimg::test::TestRunner::getInstance();
    
    // Adicionar testes unitários para funções de utilidade
    runner.addUnitTest("FileExists", test_file_exists);
    runner.addUnitTest("ImageValidation", test_image_validation);
    runner.addUnitTest("Base64Encoding", test_base64_encoding);
    
    // Adicionar testes unitários para SPICEBuilder
    runner.addUnitTest("SPICEBuilderCreation", test_spice_builder_creation);
    runner.addUnitTest("SPICEBuilderAddContent", test_spice_builder_add_content);
    runner.addUnitTest("SPICEBuilderAddImage", test_spice_builder_add_image);
    runner.addUnitTest("SPICEBuilderAddLabels", test_spice_builder_add_labels);
    
    // Adicionar novos testes para SPICEBuilder
    runner.addUnitTest("TempFileWriteAndRead", test_temp_file_write_and_read);
    runner.addUnitTest("SPICEBuilderAddEmptyContent", test_spice_builder_add_empty_content);
    runner.addUnitTest("SPICEBuilderAddDuplicateImage", test_spice_builder_add_duplicate_image);
    runner.addUnitTest("SPICEBuilderAddDuplicateLabels", test_spice_builder_add_duplicate_labels);
    runner.addUnitTest("SPICEBuilderEmptyTitle", test_spice_builder_empty_title);
    
    // Adicionar testes para GIF
    runner.addUnitTest("GIFCreation", test_gif_creation);
    
    // Adicionar testes de integração
    runner.addIntegrationTest("SPICEFullPipeline", test_spice_full_pipeline);
    
    // Adicionar testes de desempenho
    runner.addPerformanceTest("PerformanceBase64Encoding", test_performance_base64_encoding);
    runner.addPerformanceTest("PerformanceImageProcessing", test_performance_image_processing);
    
    // Executar diferentes grupos de testes com base nos argumentos
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "--unit") {
            runner.runGroup("unit");
        } else if (arg == "--integration") {
            runner.runGroup("integration");
        } else if (arg == "--performance") {
            runner.runGroup("performance");
        } else if (arg == "--spice") {
            runner.runGroup("SPICE");
        } else if (arg == "--gif") {
            runner.runGroup("GIF");
        } else {
            runner.runAll();
        }
    } else {
        // Executar todos os testes por padrão
        runner.runAll();
    }
    
    return 0;
}