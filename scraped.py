import os
import requests
from bs4 import BeautifulSoup

# URL of the directory to scrape
url = "https://example.com/directory"

# Make HTTP GET request
response = requests.get(url)
if response.status_code == 200:
    # Parse HTML content
    soup = BeautifulSoup(response.content, "html.parser")
    
    # Find all links pointing to downloadable files (PDFs and ZIPs)
    for link in soup.find_all("a", href=lambda href: (".pdf" in href) or (".zip" in href)):
        file_url = link.get("href")
        file_name = os.path.basename(file_url)
        
        # Download file
        with open(file_name, "wb") as file:
            file_response = requests.get(file_url)
            if file_response.status_code == 200:
                file.write(file_response.content)
                print(f"Downloaded: {file_name}")
            else:
                print(f"Failed to download: {file_name}")
else:
    print(f"Failed to fetch URL: {url}")
