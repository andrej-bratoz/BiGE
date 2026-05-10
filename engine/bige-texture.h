#pragma once
#include <glad/glad.h>
#include <memory>
#include <string>
#include <map>

namespace bige {
	struct texture {
        static std::shared_ptr<texture> load_from_file(const std::string & path);
        static std::shared_ptr<texture> load_from_data(int w, int h, int ch, const unsigned char* data);
        //
        void bind(uint32_t texture_unit = 0) const;
        virtual ~texture();

        uint32_t id()     const { return m_id; }
        int      width()  const { return m_width; }
        int      height() const { return m_height; }
        uint32_t m_id = 0;
    private:
        texture() = default;
       
        int      m_width = 0;
        int      m_height = 0;
    };

    /*
    class texture_cache {
    public:
        static texture_cache* instance();
        std::shared_ptr<texture> get(const std::string& path);
    private:
        std::map<std::string, std::shared_ptr<texture>> m_cache;
    };
    */
}