# TSIMG <img src="doc/logo/tsimg.png" align="right" height="120/"/>

![NEPEMVERSE](https://img.shields.io/endpoint?url=https://nepemufsc.com/.netlify/functions/verser?project=tsimg-stamp&label=LatestVersion:&labelColor=5288ce&logo=nepemverse&logoColor=white&style=metallic&color=#9e2621&cacheSeconds=3600)


TSIMG é uma aplicação C++ desenvolvida para facilitar a criação e manipulação de séries temporais de imagens, com o objetivo de simplificar a exibição e análise visual em diferentes contextos. A biblioteca atualmente suporta a geração de arquivos `.spice`, um formato interativo que permite a visualização e inspeção de imagens seriadas em navegadores web, além de suportar exportação em `.gif`. Planejamos adicionar outras funcionalidades úteis para análises visuais e temporais em breve.

## Manifesto do Arquivo .SPICE

### O que é um arquivo .SPICE?

Um arquivo `.spice` é um formato customizado que oferece uma solução multiplataforma para exibição de imagens seriadas interativas em navegadores modernos, sem a necessidade de plugins ou extensões. Ele é especialmente útil em cenários que demandam a apresentação de mudanças temporais ou variações dimensionais de um objeto ou fenômeno.

O nome "SPICE" é um acrônimo para **Serialized Picture Interactive Content Environment**. Esse formato permite incorporar imagens, textos e outros elementos multimídia em um único arquivo HTML, acessível em qualquer navegador moderno.

### Usos e Aplicações

O formato `.spice` é ideal para criar apresentações interativas em diversos campos, com destaque para as seguintes aplicações:

- **Geociências**: Visualização de mapas que mostram a concentração de minerais em diferentes profundidades, análise de evolução urbana através de timelapse de cidades, e mudanças em infraestruturas ao longo do tempo.

- **Medicina**: Exibição de imagens de ressonância magnética em múltiplas camadas, facilitando a análise de condições médicas e permitindo visualizações detalhadas.

- **Agronomia**: Monitoramento de índices vegetativos de lavouras ao longo do tempo para análise de desenvolvimento e eficácia de práticas agrícolas.

- **Educação**: Criação de tutoriais interativos para ensinar conceitos complexos com descrições e imagens detalhadas.

- **Portfólios de artistas**: Galerias interativas que proporcionam uma experiência visual imersiva para exibir obras.

- **Documentação técnica**: Visualizações detalhadas que explicam processos complexos em engenharia e desenvolvimento.

O formato `.spice` é otimizado para compatibilidade ampla em dispositivos móveis e desktops, o que o torna uma excelente ferramenta para visualização de dados em várias plataformas. Esse formato é continuamente aprimorado pela comunidade TSIMG para oferecer uma experiência de visualização de alta qualidade.

### Exemplo de Uso

Imagine que você precisa exibir uma série de imagens que mostram a evolução de um fenômeno ao longo do tempo, como o desenvolvimento urbano de uma cidade ou a variação de índices vegetativos de uma plantação. Ao invés de utilizar múltiplos arquivos de imagem que dificultam a manipulação e análise, você pode utilizar o script TSIMG para gerar um arquivo `.spice`. Esse arquivo organiza todas as imagens e descrições em um documento HTML interativo, permitindo que o usuário navegue pelas camadas temporais usando um controle deslizante.

O arquivo `.spice` permite uma análise intuitiva e fluida das imagens seriadas, com transições suaves e descrições associadas a cada camada temporal. O resultado é um documento interativo que agrega valor ao seu trabalho e melhora a comunicação visual dos dados.

## Compilação e Instalação

### Pré-requisitos

- CMake 3.15 ou superior
- Compilador C++ compatível com C++17 (GCC, Clang, MSVC)

### Instruções de Build

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Para gerar o pacote de instalação (se configurado):

```bash
cpack
```

## Uso

O TSIMG pode ser utilizado via linha de comando (CLI) ou através de um arquivo de configuração JSON.

### Linha de Comando

```bash
tsimg -n <output_filename> -i <image1.jpg,image2.png,...> [opções]
```

**Opções Disponíveis:**

- `-n <filename>`: Nome do arquivo de saída (ex: `resultado.html`).
- `-i <paths>`: Lista de imagens separadas por vírgula.
- `-l <labels>`: Lista de rótulos para as imagens (opcional).
- `-f <format>`: Formato de exportação: `spice` (padrão) ou `gif`.
- `-config <file.json>`: Carrega opções de um arquivo JSON.
- `-debug`: Ativa modo de debug com logs detalhados.
- `-labelbyname`: Gera rótulos automaticamente baseados nos nomes dos arquivos.
- `-title <text>`: Define o título da apresentação.
- `-authorimage <path>`: Caminho para imagem do autor.
- `-template <path>`: Caminho para um template HTML customizado.

### Arquivo de Configuração (JSON)

Você pode salvar suas configurações em um arquivo JSON para reutilização.

**Exemplo (`config.json`):**

```json
{
  "output_filename": "meu_projeto.html",
  "images": [
    "img/jan.jpg",
    "img/fev.jpg",
    "img/mar.jpg"
  ],
  "labels": [
    "Janeiro",
    "Fevereiro",
    "Março"
  ],
  "export_format": "spice",
  "title": "Evolução Temporal 2024",
  "createLabelsFromImages": false,
  "debug": false
}
```

Para usar:

```bash
tsimg -config config.json
```

## Documentação Técnica

A documentação do código pode ser gerada via Doxygen. Consulte a pasta `doc/` para mais detalhes.
