#include "JM.h"
#include "GLTexture2D.h"

#include "Platform/GraphicsAPI/OpenGL/GL.h"

#include "Utilities/LoadImage.h"
#include "Platform/GraphicsAPI/OpenGL/GLShader.h"

namespace jm
{

	GLTexture2D::GLTexture2D() : m_Width(0), m_Height(0)
	{
		glGenTextures(1, &m_Handle);
	}

	GLTexture2D::GLTexture2D(uint width, uint height, const TextureParameters parameters, const TextureLoadOptions loadOptions)
		: m_FileName("NULL")
	{
		m_Width = width;
		m_Height = height;
		m_Parameters = parameters;
		m_LoadOptions = loadOptions;
		m_Handle = Load();
	}

	GLTexture2D::GLTexture2D(uint width, uint height, TextureParameters parameters, TextureLoadOptions loadOptions,
		void* data)
		: m_FileName("NULL")
	{
		m_Width = width;
		m_Height = height;
		m_Parameters = parameters;
		m_LoadOptions = loadOptions;
		m_Handle = LoadTexture(data);
	}

	GLTexture2D::GLTexture2D(uint width, uint height, uint color, const TextureParameters parameters, const TextureLoadOptions loadOptions)
		: m_FileName("NULL")
	{
		m_Width = width;
		m_Height = height;
		m_Parameters = parameters;
		m_LoadOptions = loadOptions;
		m_Handle = Load();

		GLTexture2D::SetData(color);
	}

	GLTexture2D::GLTexture2D(const String& name, const String& filename, const TextureParameters parameters, const TextureLoadOptions loadOptions)
		: m_FileName(filename)
	{
		m_Name = name;
		m_Parameters = parameters;
		m_LoadOptions = loadOptions;
		m_Handle = Load();
	}

	uint CreateTextureAttachment(int width, int height, const void* pixels)
	{
		uint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height,
			0, GL_RED, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		return texture;
	}

	GLTexture2D::GLTexture2D(int width, int height, const void* pixels) : m_Width(0), m_Height(0)
	{
		m_Handle = CreateTextureAttachment(width, height, pixels);
	}

	GLTexture2D::~GLTexture2D()
	{
		GLCall(glDeleteTextures(1, &m_Handle));
	}

	uint GLTexture2D::LoadTexture(void* data) const
	{
		uint handle;
		GLCall(glGenTextures(1, &handle));
		GLCall(glBindTexture(GL_TEXTURE_2D, handle));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Parameters.filter == TextureFilter::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Parameters.filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(m_Parameters.wrap)));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(m_Parameters.wrap)));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, TextureFormatToGL(m_Parameters.format), m_Width, m_Height, 0, TextureFormatToGL(m_Parameters.format), GL_UNSIGNED_BYTE, data ? data : NULL));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
#ifdef JM_DEBUG
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
#endif

		return handle;
	}

	uint GLTexture2D::Load()
	{
		// TODO: Split this up into loading from file vs. generating from data
		byte* pixels = nullptr;
		if (m_FileName != "NULL")
		{
			uint bits;
			pixels = jm::LoadImageFromFile(m_FileName.c_str(), &m_Width, &m_Height, &bits, !m_LoadOptions.flipY);
			m_Parameters.format = BitsToTextureFormat(bits);
		}

		uint handle;
		GLCall(glGenTextures(1, &handle));
		GLCall(glBindTexture(GL_TEXTURE_2D, handle));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Parameters.filter == TextureFilter::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Parameters.filter == TextureFilter::LINEAR ? GL_LINEAR : GL_NEAREST));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(m_Parameters.wrap)));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(m_Parameters.wrap)));

		uint format = TextureFormatToGL(m_Parameters.format);
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, TextureFormatToInternalFormat(format), GL_UNSIGNED_BYTE, pixels ? pixels : NULL));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
#ifdef JM_DEBUG
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
#endif

		if (pixels != nullptr)
			delete[] pixels;

		return handle;
	}

	void GLTexture2D::SetData(const void* pixels)
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, m_Handle));
		GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, TextureFormatToGL(m_Parameters.format), GL_UNSIGNED_BYTE, pixels));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}

	void GLTexture2D::SetData(uint color)
	{
		JM_CORE_ASSERT(false, "Broken");
		uint stride = m_Parameters.format == TextureFormat::RGB ? 3 : 4;
		uint size = m_Width * m_Height * stride;
		byte* data = new byte[size];
		for (uint i = 0; i < size; i += stride)
		{
			data[i + 0] = (color & 0xff);
			data[i + 1] = (color & 0xff00) >> 8;
			data[i + 2] = (color & 0xff0000) >> 16;
			if (stride == 4)
				data[i + 3] = (color & 0xff000000) >> 24;
		}

		SetData(data);
		delete[] data;
	}

	void GLTexture2D::Bind(uint slot) const
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_Handle));
	}

	void GLTexture2D::Unbind(uint slot) const
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

	uint GLTexture2D::TextureFormatToGL(const TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGBA:				return GL_RGBA;
		case TextureFormat::RGB:				return GL_RGB;
		case TextureFormat::R8:				    return GL_R8;
		case TextureFormat::RG8:				return GL_RG8;
		case TextureFormat::RGB8:				return GL_RGB8;
		case TextureFormat::RGBA8:				return GL_RGBA8;
		case TextureFormat::LUMINANCE:			return GL_LUMINANCE;
		case TextureFormat::LUMINANCE_ALPHA:	return GL_LUMINANCE_ALPHA;
		default: JM_ERROR("[Texture] Unsupported image bit-depth!");  return 0;
		}
	}

	uint GLTexture2D::TextureWrapToGL(const TextureWrap wrap)
	{
		switch (wrap)
		{
#ifndef JM_PLATFORM_MOBILE
		case TextureWrap::CLAMP:			return GL_CLAMP;
		case TextureWrap::CLAMP_TO_BORDER:	return GL_CLAMP_TO_BORDER;
#endif
		case TextureWrap::CLAMP_TO_EDGE:	return GL_CLAMP_TO_EDGE;
		case TextureWrap::REPEAT:			return GL_REPEAT;
		case TextureWrap::MIRRORED_REPEAT:	return GL_MIRRORED_REPEAT;
		default: JM_ERROR("[Texture] Unsupported image bit-depth!");  return 0;
		}
	}

	TextureFormat GLTexture2D::BitsToTextureFormat(uint bits)
	{
		switch (bits)
		{
		case 8:		return TextureFormat::R8;
		case 16:	return TextureFormat::RG8;
		case 24:	return TextureFormat::RGB8;
		case 32:	return TextureFormat::RGBA8;

		default: JM_ERROR("[Texture] Unsupported image bit-depth! (", bits, ")");  return TextureFormat::RGB8;
		}
	}

	uint GLTexture2D::TextureFormatToInternalFormat(uint format)
	{
		switch (format)
		{
		case GL_RGBA:				return GL_RGBA;
		case GL_RGB:				return GL_RGB;
		case GL_R8:				    return GL_RED;
		case GL_RG8:				return GL_RG;
		case GL_RGB8:				return GL_RGB;
		case GL_RGBA8:				return GL_RGBA;
		case GL_LUMINANCE:			return GL_LUMINANCE;
		case GL_LUMINANCE_ALPHA:	return GL_LUMINANCE_ALPHA;
		default: JM_ERROR("[Texture] Unsupported Texture Format");  return 0;
		}
	}

	void GLTexture2D::BuildTexture(const TextureFormat internalformat, uint width, uint height, bool depth, bool samplerShadow)
	{
		m_Width = width;
		m_Height = height;
		m_Name = "Texture Attachment";

		uint Format;
		switch (internalformat)
		{
		case TextureFormat::RGB8:  Format = GL_RGB8; break;
		case TextureFormat::RGB16: Format = GL_RGB16F; break;
		case TextureFormat::RGBA:  Format = GL_RGBA; break;
		case TextureFormat::DEPTH: Format = GL_DEPTH24_STENCIL8; break;
		default: Format = GL_RGB8; break;
		}

		uint Format2;
		switch (internalformat)
		{
		case TextureFormat::RGB8:  Format2 = GL_RGB; break;
		case TextureFormat::RGB16: Format2 = GL_RGB; break;
		case TextureFormat::RGBA:  Format2 = GL_RGBA; break;
		case TextureFormat::DEPTH: Format2 = GL_DEPTH_COMPONENT; break;
		default: Format2 = GL_RGB; break;
		}

		glBindTexture(GL_TEXTURE_2D, m_Handle);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (samplerShadow)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
#ifndef JM_PLATFORM_MOBILE
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
#endif
		}

		if (depth)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, Format, width, height, 0, Format2, GL_UNSIGNED_BYTE, nullptr);
		}
		else
			glTexImage2D(GL_TEXTURE_2D, 0, Format, width, height, 0, Format2, GL_FLOAT, nullptr);
	}

	byte* GLTexture2D::LoadTextureData()
	{
		byte* pixels = nullptr;
		if (m_FileName != "NULL")
		{
			uint bits;
			pixels = jm::LoadImageFromFile(m_FileName.c_str(), &m_Width, &m_Height, &bits, !m_LoadOptions.flipY);
			m_Parameters.format = BitsToTextureFormat(bits);
		}
		return pixels;
	};

}