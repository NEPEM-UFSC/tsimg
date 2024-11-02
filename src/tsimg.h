#pragma once

#include <string>
#include <vector>

/**
 * @brief Concatena uma lista de strings em uma única string.
 * 
 * @param vec Vetor de strings.
 * @return Uma string concatenada.
 */
std::string concatenateStrings(const std::vector<std::string>& vec);

/**
 * @brief Lê o conteúdo de um arquivo e o retorna como uma string.
 * 
 * @param filepath Caminho para o arquivo.
 * @param debug Ativa o modo de depuração.
 * @return O conteúdo do arquivo como string.
 */
std::string readFileToString(const std::string& filepath, bool debug);