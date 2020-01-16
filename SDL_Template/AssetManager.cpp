#include "AssetManager.h"

namespace SDLFramework {

	AssetManager * AssetManager::sInstance = nullptr;

	AssetManager * AssetManager::Instance() {
		if (sInstance == nullptr) {
			sInstance = new AssetManager();
		}

		return sInstance;
	}

	void AssetManager::Release() {
		delete sInstance;
		sInstance = nullptr;
	}

	AssetManager::AssetManager() {
	}

	AssetManager::~AssetManager() {
		for (auto tex : mTextures) {
			if (tex.second != nullptr) {
				SDL_DestroyTexture(tex.second);
			}
		}
		mTextures.clear();
	}

	SDL_Texture * AssetManager::GetTexture(std::string filename, bool managed) {
		std::string fullPath = SDL_GetBasePath();
		fullPath.append("Assets/" + filename);

		if (mTextures[fullPath] == nullptr) {
			mTextures[fullPath] = Graphics::Instance()->LoadTexture(fullPath);
		}

		if (mTextures[fullPath] != nullptr && managed) {
			mTextureRefCount[mTextures[fullPath]] += 1;
		}

		return mTextures[fullPath];
	}

	void AssetManager::DestroyTexture(SDL_Texture * texture) {
		if (mTextureRefCount.find(texture) != mTextureRefCount.end()) {
			mTextureRefCount[texture] -= 1;
			if (mTextureRefCount[texture] == 0) {
				for (auto elem : mTextures) {
					if (elem.second == texture) {
						SDL_DestroyTexture(elem.second);
						mTextures.erase(elem.first);
						return; // work finished, leave function
					}
				}
			}
		}
		else {
			SDL_DestroyTexture(texture);
		}
	}
}