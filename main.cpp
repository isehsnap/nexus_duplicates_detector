#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

namespace fs = std::filesystem;
using namespace std;

int levenshteinDistance(const std::string& s1, const std::string& s2)
{
    int m = s1.size();
    int n = s2.size();

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + min({dp[i][j - 1], dp[i - 1][j], dp[i - 1][j - 1]});
        }
    }

    return dp[m][n];
}

std::string getMetaFileId(const fs::path& filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::string line;
    while (std::getline(file, line))
    {
        const std::string key1 = "modID = ";
        auto pos1 = line.find(key1);
        if (pos1 != std::string::npos)
        {
            std::string modId = line.substr(pos1 + key1.length());
            return modId;
        }
        else
        {
            const std::string key2 = "modID=";
            auto pos2 = line.find(key2);
            if (pos2 != std::string::npos)
            {
                std::string modId = line.substr(pos2 + key2.length());
                return modId;
            }
        }
    }

    return "";
}

bool nexus_file_comp(const tuple<fs::path, string>& lhs, const tuple<fs::path, string>& rhs)
{
    return get<1>(lhs) < get<1>(rhs);
}

int main(const int argc, const char* argv[])
{

    assert((argc == 2 || argc == 3) && "Invalid arguments. Usage : .\\nexus_duplicates_detector.exe [DIRECTORY_PATH] or .\\nexus_duplicates_detector.exe [DIRECTORY_PATH] [MAXIMUM DIFFERENT CHARACTERS NUMBER]");

    assert(
        filesystem::is_directory(argv[1]) &&
        "Invalid directory path. Usage : .\\nexus_duplicates_detector.exe [DIRECTORY_PATH] or .\\nexus_duplicates_detector.exe [DIRECTORY_PATH] [MAXIMUM DIFFERENT CHARACTERS NUMBER]");

    std::unordered_multimap<std::string, fs::path> modIdMap;

    unsigned int maxDiff = argc == 3 ? std::stoi(argv[2]) : 17;

    for (const auto& entry : fs::directory_iterator(argv[1]))
    {
        if (entry.is_regular_file())
        {
            string ext = entry.path().extension().string();
            ranges::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".meta")
            {
                string modId = getMetaFileId(entry.path());
                modIdMap.insert({modId, entry.path()});
            }
        }
    }

    std::unordered_set<std::string> displayedModIds;


    for (const auto& [modId, _] : modIdMap)
    {
        if (displayedModIds.contains(modId))
        {
            // We have already displayed this modId, so skip it
            continue;
        }

        auto range = modIdMap.equal_range(modId);
        if (std::distance(range.first, range.second) > 1)
        {
            std::vector<fs::path> paths;
            for (auto it = range.first; it != range.second; ++it)
            {
                paths.push_back(it->second);
            }

            for (size_t i = 0; i < paths.size(); ++i)
            {
                for (size_t j = i + 1; j < paths.size(); ++j)
                {
                    if (levenshteinDistance(paths[i].string(), paths[j].string()) < maxDiff)
                    {
                        std::cout << "ModId: " << modId << std::endl;
                        std::cout << "  " << paths[i] << std::endl;
                        std::cout << "  " << paths[j] << std::endl;

                        // Add the modId to the set of displayed modIds
                        displayedModIds.insert(modId);
                        break;
                    }
                }
                if (displayedModIds.contains(modId))
                {
                    break;
                }
            }
        }
    }

    return 0;
}
