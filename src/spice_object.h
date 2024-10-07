#ifndef spice_object_h
#define spice_object_h

#include <string>
#include <vector>
#include <map>

/**
 * @brief The SPICE class is responsible for creating and managing a SPICE presentation.
 * 
 * A SPICE (Serialized Picture Interactive Content Environment) object can manage images, 
 * text content, and labels, then generate a SPICE HTML file using a provided template.
 */
class SPICE {
public:
    /**
     * @brief Constructs a new SPICE object with an optional title.
     * 
     * @param title The title of the SPICE presentation. Defaults to an empty string.
     * @param debug If true, enables debug logging for tracking the process.
     */
    SPICE(const std::string& title = "", bool debug = false);

    /**
     * @brief Adds an image to the SPICE presentation by encoding it to Base64 format.
     * 
     * @param imagePath The file path of the image to add.
     * @param debug If true, logs the process of adding and encoding the image.
     */
    void addImage(const std::string& imagePath, bool debug = false);

    /**
     * @brief Adds custom content to be inserted into a specific tag within the SPICE HTML template.
     * 
     * @param tag The tag in the HTML template to replace (e.g., "SPICE_TEXT", "SPICE_TITLE").
     * @param content The content to insert into the specified tag.
     * @param debug If true, logs the content addition process.
     */
    void addContent(const std::string& tag, const std::string& content, bool debug = false);

    /**
     * @brief Adds labels to the SPICE presentation, corresponding to images or other content.
     * 
     * @param labelList A vector of strings representing the labels to add.
     * @param debug If true, logs the label addition process.
     */
    void addLabels(const std::vector<std::string>& labelList, bool debug = false);

    /**
     * @brief Generates the final SPICE HTML file based on the provided template.
     * 
     * The function reads the provided template file, replaces the tags with the corresponding
     * content, and writes the final HTML file to the specified output path.
     * 
     * @param templateFile The path to the HTML template file.
     * @param outputFile The path where the final SPICE HTML file will be saved.
     * @param debug If true, logs the generation process.
     */
    void generateSPICEFileFromTemplate(const std::string& templateFile, const std::string& outputFile, bool debug = false);

private:
    /// Title of the SPICE presentation.
    std::string title;

    /// List of images in Base64 format to be embedded in the SPICE HTML file.
    std::vector<std::string> base64Images;

    /// Custom slider controls content for the SPICE presentation.
    std::string sliderControls;

    /// List of content elements to be inserted into the SPICE HTML file.
    std::vector<std::string> contentElements;

    /// List of labels for the images or other content in the SPICE presentation.
    std::vector<std::string> labels;

    /// Map for associating HTML tags with the content to be inserted in the SPICE file.
    std::map<std::string, std::string> contentMap;

    /**
     * @brief Encodes an image into Base64 format for embedding into HTML.
     * 
     * @param imagePath The file path of the image to encode.
     * @param debug If true, logs the encoding process.
     * @return The Base64-encoded string of the image.
     */
    std::string encodeImageToBase64(const std::string& imagePath, bool debug = false);

    /**
     * @brief Generates HTML `<img>` tags for all embedded Base64-encoded images.
     * 
     * @return A string containing the HTML `<img>` tags for all images.
     */
    std::string generateImageTags();

    /**
     * @brief Generates HTML tags for all labels in the SPICE presentation.
     * 
     * @return A string containing the HTML tags for all labels.
     */
    std::string generateLabelTags();
};

/**
 * @brief Replaces a specific tag in a source string with a given replacement.
 * 
 * @param source The original string containing the tag.
 * @param tag The tag to replace.
 * @param replacement The replacement content for the tag.
 * @return A new string with the tag replaced by the replacement content.
 */
std::string replaceTag(const std::string& source, const std::string& tag, const std::string& replacement);

/**
 * @brief Reads the content of a file into a string.
 * 
 * @param filepath The path to the file to read.
 * @param debug If true, logs the file reading process.
 * @return The content of the file as a string.
 */
std::string readFileToString(const std::string& filepath, bool debug = false);

#endif
