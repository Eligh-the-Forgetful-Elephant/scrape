#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

// Callback function to handle HTTP response
size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

int main() {
    // Initialize cURL
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL to scrape
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/directory");

        // Set callback function to receive HTML content
        std::string htmlContent;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlContent);

        // Perform the HTTP request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to fetch URL: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return 1;
        }

        // Parse HTML content (using libxml2)
        htmlDocPtr doc = htmlReadMemory(htmlContent.c_str(), htmlContent.length(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
        if (doc == NULL) {
            std::cerr << "Failed to parse HTML" << std::endl;
            curl_easy_cleanup(curl);
            return 1;
        }

        // Extract URLs of downloadable files (using XPath with libxml2)
        xmlXPathContextPtr context = xmlXPathNewContext(doc);
        if (context == NULL) {
            std::cerr << "Failed to create XPath context" << std::endl;
            xmlFreeDoc(doc);
            curl_easy_cleanup(curl);
            return 1;
        }

        // Define the XPath expression to select links pointing to downloadable files
        const xmlChar* xpathExpr = (const xmlChar*)"//a[contains(@href,'.pdf') or contains(@href,'.zip')]";
        xmlXPathObjectPtr result = xmlXPathEvalExpression(xpathExpr, context);
        if (result == NULL) {
            std::cerr << "Failed to evaluate XPath expression" << std::endl;
            xmlXPathFreeContext(context);
            xmlFreeDoc(doc);
            curl_easy_cleanup(curl);
            return 1;
        }

        // Iterate over the XPath result and download each file
        xmlNodeSetPtr nodes = result->nodesetval;
        for (int i = 0; i < nodes->nodeNr; ++i) {
            xmlNodePtr node = nodes->nodeTab[i];
            if (node->type == XML_ELEMENT_NODE) {
                xmlChar* href = xmlGetProp(node, (const xmlChar*)"href");
                if (href != NULL) {
                    // Construct URL and download file using cURL
                    std::string fileUrl = "https://example.com" + std::string(reinterpret_cast<char*>(href));
                    std::cout << "Downloading file: " << fileUrl << std::endl;
                    // Add logic here to download file using cURL and save it locally
                    // ...

                    xmlFree(href);
                }
            }
        }

        // Cleanup
        xmlXPathFreeObject(result);
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize cURL" << std::endl;
        return 1;
    }

    return 0;
}
