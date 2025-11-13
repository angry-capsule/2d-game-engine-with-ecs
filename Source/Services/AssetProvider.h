#ifndef ASSETPROVIDER_H
#define ASSETPROVIDER_H

#include <string>
#include <filesystem>

class AssetProvider 
{
	public:
		static std::string GetAssetPath(const std::string& relPath)
		{
			std::filesystem::path root = PROJECT_SOURCE_DIR;
			return (root / relPath).string();
		}
};

#endif