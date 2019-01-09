#pragma once
#include "JM.h"
#include "Graphics/API/FrameBuffer.h"
#include "Platform/GraphicsAPI/OpenGL/GL.h"
#include "Textures/GLTexture2D.h"

namespace jm
{

	class JM_EXPORT GLFramebuffer : public Framebuffer
	{
	public:

		GLFramebuffer();
		GLFramebuffer(FrameBufferInfo bufferInfo);
		~GLFramebuffer();

		inline uint GetFrameBuffer() const { return m_Handle; }

		void GenerateFrameBuffer() override;

		void Bind(uint width, uint height) const override;
		void Bind() const override;
		void UnBind() const override;
		void Clear() override {}
		uint GetWidth() const override { return m_Width; }
		uint GetHeight() const override { return m_Height; }

		inline void SetClearColour(const maths::Vector4& colour) override { m_ClearColour = colour; }

		void AddTextureAttachment(Attachment attachmentType, Texture* texture) override;
		void AddCubeTextureAttachment(Attachment attachmentType, CubeFace face, TextureCube* texture) override;

		void AddShadowAttachment(Texture*  texture) override;
		void AddTextureLayer(int index, Texture*  texture) override;

		void Validate() override;

	private:

		uint m_Handle;
		uint m_Width, m_Height, m_AttachmentCount;
		maths::Vector4 m_ClearColour;
	};
}