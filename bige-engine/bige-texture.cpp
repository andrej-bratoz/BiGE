#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <bige-texture.h>
#include <bige-logging.h>
#include <glad/glad.h>

namespace bige {
	
    std::shared_ptr<texture> texture::load_from_data(int w, int h, int channels, const unsigned char* data) {

        auto tex = std::shared_ptr<texture>(new texture());
        tex->m_width = w;
        tex->m_height = h;

        glGenTextures(1, &tex->m_id);
        glBindTexture(GL_TEXTURE_2D, tex->m_id);

		// set some defaults for wrapping and filtering - these can be changed later by the user
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (channels == 4) ? GL_RGBA
            : (channels == 3) ? GL_RGB
            : (channels == 1) ? GL_RED
            : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return tex;
    }

    std::shared_ptr<texture> texture::load_from_file(const std::string& path) {
        stbi_set_flip_vertically_on_load(true);
        int w, h, channels;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 0);
        if (!data) {
			_log("texture");
			_l("failed to load texture from file '{}', error: {}", path, stbi_failure_reason());
            return nullptr;
        }
		auto tex = load_from_data(w, h, channels, data);
        stbi_image_free(data);
		return tex;
    }

    texture::~texture() { if (m_id != 0) glDeleteTextures(1, &m_id); }

    void texture::bind(uint32_t texture_unit) const {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

}
