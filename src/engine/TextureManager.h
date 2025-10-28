#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

namespace zelda::game
{
    class TextureManager
    {
    public:
        TextureManager() = default;
        ~TextureManager() = default;

        // Load and cache a texture under a string key.
        // Returns true on success, false on failure.
        bool loadTexture(const std::string& key,
                         const std::string& path,
                         SDL_Renderer* renderer)
        {
            // If already loaded, no need to do it again.
            if (m_textures.find(key) != m_textures.end())
                return true;

            SDL_Texture* tex = createTextureFromFile(path, renderer);
            if (!tex)
            {
                SDL_Log("TextureManager: Failed to load '%s' for key '%s': %s",
                        path.c_str(),
                        key.c_str(),
                        IMG_GetError());
                return false;
            }

            m_textures[key] = tex;
            SDL_Log("TextureManager: Loaded '%s' as key '%s'", path.c_str(), key.c_str());
            return true;
        }

        // Retrieve a loaded texture or nullptr if missing.
        SDL_Texture* get(const std::string& key) const
        {
            auto it = m_textures.find(key);
            if (it == m_textures.end())
                return nullptr;
            return it->second;
        }

        // Free all textures, to be called before renderer is destroyed.
        void clear()
        {
            for (auto& kv : m_textures)
            {
                if (kv.second)
                {
                    SDL_DestroyTexture(kv.second);
                }
            }
            m_textures.clear();
        }

    private:
        // Internal helper that loads a PNG/etc using SDL2_image and converts it to an SDL_Texture.
        SDL_Texture* createTextureFromFile(const std::string& path,
                                           SDL_Renderer* renderer)
        {
            SDL_Surface* surface = IMG_Load(path.c_str());
            if (!surface)
            {
                SDL_Log("IMG_Load failed for %s: %s", path.c_str(), IMG_GetError());
                return nullptr;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            if (!texture)
            {
                SDL_Log("SDL_CreateTextureFromSurface failed for %s: %s",
                        path.c_str(),
                        SDL_GetError());
            }

            return texture;
        }

        std::unordered_map<std::string, SDL_Texture*> m_textures;
    };
}
