#include "tsimg.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>

/**
 * @brief Concatena um vetor de strings em uma única string.
 * 
 * @param vec Vetor de strings.
 * @return Uma string concatenada com todos os elementos do vetor.
 */
std::string concatenateStrings(const std::vector<std::string>& vec) {
    std::ostringstream oss;
    for (const auto& str : vec) {
        oss << str;
    }
    return oss.str();
}

/**
 * @brief Lê o conteúdo de um arquivo e retorna como uma string.
 * 
 * @param filepath Caminho do arquivo.
 * @param debug Ativa o modo de depuração para imprimir detalhes da operação.
 * @return O conteúdo do arquivo em formato string.
 */
std::string readFileToString(const std::string& filepath, bool debug) {
    if (debug) std::cout << "Reading file: " << filepath << std::endl;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de template: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    if (debug) std::cout << "File read successfully." << std::endl;
    return buffer.str();
}

